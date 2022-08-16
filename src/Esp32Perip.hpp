#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>
#include <soc/pcnt_reg.h>
#include <soc/gpio_reg.h>
#include <soc/dport_reg.h>
#include <soc/dport_access.h>

void forzar_update()
{
    Serial.print("Valor del Registro MCPWM_UPDATE_CFG_REG: ");
    Serial.println(REG_READ(MCPWM_UPDATE_CFG_REG(0)));
    // REG_SET_BIT(MCPWM_UPDATE_CFG_REG(0),BIT0+BIT1);
    REG_WRITE(MCPWM_UPDATE_CFG_REG(0), BIT0 + BIT1);
    Serial.print("Valor del Registro MCPWM_UPDATE_CFG_REG: ");
    Serial.println(REG_READ(MCPWM_UPDATE_CFG_REG(0)));
}

void set_TMR0_PERIOD(uint16_t Periodo)
{
    REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD, Periodo);
    Serial.print("Tpwm: ");
    Serial.println(REG_GET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD));
}

void set_DutyCicle(uint16_t Duty)
{
    Serial.print("Valor del Registro A: ");
    Serial.println(REG_GET_FIELD(MCPWM_GEN0_TSTMP_A_REG(0), MCPWM_GEN0_A));
    REG_SET_FIELD(MCPWM_GEN0_TSTMP_A_REG(0), MCPWM_GEN0_A, Duty);
    Serial.print("Valor del Registro A: ");
    Serial.println(REG_GET_FIELD(MCPWM_GEN0_TSTMP_A_REG(0), MCPWM_GEN0_A));
}

void leer_TIMER0()
{
    Serial.print("Valor del Registro Timer0: ");
    Serial.println(REG_GET_FIELD(MCPWM_TIMER0_STATUS_REG(0), MCPWM_TIMER0_VALUE));
}

void enablePCOUNTERCLK()
{
    DPORT_REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_PCNT_CLK_EN);

    DPORT_REG_SET_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PCNT_RST);
    DPORT_REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PCNT_RST);

    // MPU/MMU registers are used for MPU/MMU configuration and operation control. They are listed in Section 5.4,
    // categorized as ”MPU/MMU registers”. For a detailed description of these registers, please refer to Chapter Memory
    // Management and Protection Units (MMU, MPU).
    DPORT_REG_WRITE(DPORT_AHBLITE_MPU_TABLE_PCNT_REG, 1);
}

/* CONFIGURACION GPIO-PCOUNTER I
To read GPIO pad X into peripheral signal Y, follow the steps below:
1. Configure the GPIO_FUNCy_IN_SEL_CFG register corresponding to peripheral signal Y in the GPIO Matrix:
• Set the GPIO_FUNCy_IN_SEL field in this register, corresponding to the GPIO pad X to read from. Clear
all other fields corresponding to other GPIO pads.
2. Configure the GPIO_FUNCx_OUT_SEL_CFG register and clear the GPIO_ENABLE_DATA[x] field corresponding
to GPIO pad X in the GPIO Matrix:
• Set the GPIO_FUNCx_OEN_SEL bit in the GPIO_FUNCx_OUT_SEL_CFG register to force the pin’s
output state to be determined always by the GPIO_ENABLE_DATA[x] field.
• The GPIO_ENABLE_DATA[x] field is a bit in either GPIO_ENABLE_REG (GPIOs 0-31) or GPIO_ENABLE1_REG
(GPIOs 32-39). Clear this bit to disable the output driver for the GPIO pad.
3. Configure the IO_MUX to select the GPIO Matrix. Set the IO_MUX_x_REG register corresponding to GPIO
pad X as follows:
• Set the function field (MCU_SEL) to the IO_MUX function corresponding to GPIO X (this is Function
2—numeric value 2—for all pins).
• Enable the input by setting the FUN_IE bit.
• Set or clear the
*/
void confGPIOPCNT()
{
    // Configuracion GPIO15/18 como Salida PWM0a (Func32/33)
    //  1) Configure the GPIO_FUNCy_IN_SEL_CFG register corresponding to peripheral signal Y in the GPIO Matrix:
    // Set the GPIO_FUNCy_IN_SEL field in this register, corresponding to the GPIO pad X to read from. Clear
    // all other fields corresponding to other GPIO pads.
    REG_SET_FIELD(GPIO_FUNC36_IN_SEL_CFG_REG, GPIO_FUNC36_IN_SEL, 39);
    REG_SET_FIELD(GPIO_FUNC39_IN_SEL_CFG_REG, GPIO_FUNC39_IN_SEL, 43);
    REG_SET_FIELD(GPIO_FUNC34_IN_SEL_CFG_REG, GPIO_FUNC34_IN_SEL, 47);

    // 2) Configure the GPIO_FUNCx_OUT_SEL_CFG register and clear the GPIO_ENABLE_DATA[x] field corresponding
    // to GPIO pad X in the GPIO Matrix:
    // 2a)Set the GPIO_FUNCx_OEN_SEL bit in the GPIO_FUNCx_OUT_SEL_CFG register to force the pin’s
    // output state to be determined always by the GPIO_ENABLE_DATA[x] field.
    REG_SET_FIELD(GPIO_FUNC36_OUT_SEL_CFG_REG, GPIO_FUNC36_OEN_SEL, 1);
    REG_SET_FIELD(GPIO_FUNC39_OUT_SEL_CFG_REG, GPIO_FUNC39_OEN_SEL, 1);
    REG_SET_FIELD(GPIO_FUNC34_OUT_SEL_CFG_REG, GPIO_FUNC34_OEN_SEL, 1);

    // 2b)
    // The GPIO_ENABLE_DATA[x] field is a bit in either GPIO_ENABLE_REG (GPIOs 0-31) or GPIO_ENABLE1_REG
    //(GPIOs 32-39). Clear this bit to disable the output driver for the GPIO pad.
    // REG_SET_FIELD(GPIO_ENABLE_REG, GPIO_ENABLE_DATA, BIT15 + BIT18);
    REG_CLR_BIT(GPIO_ENABLE_REG, BIT2 + BIT4 + BIT7);

    Serial.print("Valor de GPIO_FUNC34_OUT_SEL_CFG_REG: ");
    Serial.println(REG_READ(GPIO_FUNC34_OUT_SEL_CFG_REG));

    // 3) Configure the IO_MUX to select the GPIO Matrix. Set the IO_MUX_x_REG register corresponding to GPIO
    // pad X as follows:

    // 3a) Set the function field (MCU_SEL) to the IO_MUX function corresponding to GPIO X (this is Function
    // 2—numeric value 2—for all pins).
    // REG_SET_FIELD(IO_MUX_GPIO34_REG, MCU_SEL, 0);

    // 3b) Enable the input by setting the FUN_IE bit.
    REG_SET_FIELD(IO_MUX_GPIO34_REG, FUN_IE, 1);
    REG_SET_FIELD(IO_MUX_GPIO36_REG, FUN_IE, 1);
    REG_SET_FIELD(IO_MUX_GPIO39_REG, FUN_IE, 1);

    //  3c) Set or clear the FUN_WPU and FUN_WPD bits, as desired, to enable/disable internal pull-up/pull-down
    // resistors.
    REG_SET_BIT(IO_MUX_GPIO34_REG, BIT8);
    REG_SET_BIT(IO_MUX_GPIO36_REG, BIT8);
    REG_SET_BIT(IO_MUX_GPIO39_REG, BIT8);
}

void configPCOUNTER()
{
    REG_SET_FIELD(PCNT_U0_CONF0_REG, PCNT_CH0_POS_MODE_U0, 1);
    REG_SET_FIELD(PCNT_U0_CONF0_REG, PCNT_CH0_NEG_MODE_U0, 0);
    REG_SET_FIELD(PCNT_U0_CONF0_REG, PCNT_CH0_LCTRL_MODE_U0, 0);
    REG_SET_FIELD(PCNT_U0_CONF0_REG, PCNT_CH0_HCTRL_MODE_U0, 0);
    REG_SET_FIELD(PCNT_U0_CONF0_REG, PCNT_CNT_H_LIM_U0, 8);
}

void enableMCPWCLK()
{
    // HABILITACIÓN DEL PERIFÉRICO MCPWM MEDIANTE DPORT REGISTERS
    Serial.print(" REGISTRO HABILITACION MCPWM: ");
    Serial.println(DPORT_REG_READ(DPORT_PERIP_CLK_EN_REG));
    DPORT_REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_PWM0_CLK_EN);
    Serial.print(" REGISTRO HABILITACION MCPWM: ");
    Serial.println(DPORT_REG_READ(DPORT_PERIP_CLK_EN_REG));
    DPORT_REG_SET_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
    DPORT_REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
    DPORT_REG_READ(DPORT_AHBLITE_MPU_TABLE_PWM0_REG);
    DPORT_REG_WRITE(DPORT_AHBLITE_MPU_TABLE_PWM0_REG, 1);
}

/* CONFIGURACION GPIO-MCPWM O
To output peripheral signal Y to particular GPIO pad X, follow these steps:
1. Configure the GPIO_FUNCx_OUT_SEL_CFG register and GPIO_ENABLE_DATA[x] field corresponding to
GPIO X in the GPIO Matrix:
a) Set the GPIO_FUNCx_OUT_SEL field in GPIO_FUNCx_OUT_SEL_CFG to the numeric index (Y) of desired
peripheral output signal Y.
b) If the signal should always be enabled as an output, set the GPIO_FUNCx_OEN_SEL bit in the GPIO_FUN
Cx_OUT_SEL_CFG register and the GPIO_ENABLE_DATA[x] field in the GPIO_ENABLE_REG register
corresponding to GPIO pad X. To have the output enable signal decided by internal logic, clear the
GPIO_FUNCx_OEN_SEL bit instead.
c) The GPIO_ENABLE_DATA[x] field is a bit in either GPIO_ENABLE_REG (GPIOs 0-31) or GPIO_ENABLE1
_REG (GPIOs 32-39). Clear this bit to disable the output driver for the GPIO pad.

2. For an open drain output, set the GPIO_PINx_PAD_DRIVER bit in the GPIO_PINx register corresponding to
GPIO pad X. For push/pull mode (default), clear this bit.

3. Configure the IO_MUX to select the GPIO Matrix. Set the IO_MUX_x_REG register corresponding to GPIO
pad X as follows:
a) Set the function field (MCU_SEL) to the IO_MUX function corresponding to GPIO X (this is Function
2—numeric value 2—for all pins).
b) Set the FUN_DRV field to the desired value for output strength (0-3). The higher the drive strength, the
more current can be sourced/sunk from the pin.
c) If using open drain mode, set/clear the FUN_WPU and FUN_WPD bits to enable/disable the internal
pull-up/down resistors.
*/
void confGPIOMCPWM()
{
    // Configuracion GPIO15/18 como Salida PWM0a (Func32/33)
    //  1a)
    REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 32);
    REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 33);

    // 1b)
    REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OEN_SEL, 1);
    REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OEN_SEL, 1);

    // 1c)
    REG_SET_FIELD(GPIO_ENABLE_REG, GPIO_ENABLE_DATA, BIT15 + BIT18);

    Serial.print("Valor de GPIO_FUNC15_OUT_SEL_CFG_REG: ");
    Serial.println(REG_READ(GPIO_FUNC15_OUT_SEL_CFG_REG));

    Serial.print("Valor de GPIO_FUNC18_OUT_SEL_CFG_REG: ");
    Serial.println(REG_READ(GPIO_FUNC18_OUT_SEL_CFG_REG));

    // 2 Se omite,no es salida open drain

    // 3a)
    // REG_SET_FIELD(PERIPHS_IO_MUX_MTDO_U, MCU_SEL, 2);
    REG_SET_FIELD(IO_MUX_GPIO15_REG, MCU_SEL, 2);
    // 3b)
    // REG_SET_FIELD(IO_MUX_GPIO15_REG,FUN_DRV,1);
    //  3c) Se omite

    // REG_SET_FIELD(GPIO_ENABLE_REG, GPIO_ENABLE_DATA, BIT15 + BIT18);

    Serial.print("Valor de GPIO_FUNC18_OUT_SEL_CFG_REG: ");
    Serial.println(REG_READ(GPIO_FUNC18_OUT_SEL_CFG_REG));
}

void conf_MCPWM()
{
    // REGISTRO MCPWM_CLK_CFG_REG (Preescalado del CLK)
    REG_SET_FIELD(MCPWM_CLK_CFG_REG(0), MCPWM_CLK_PRESCALE, 0);
    Serial.print("Valor del Registro MCPWM_CLK_CFG_REG: ");
    Serial.println(REG_READ(MCPWM_CLK_CFG_REG(0)));

    // CONFIGURACION DEL TIMER0 [(3+2)R/W+1RO Registros]

    // MCPWM_TIMER0_CFG0_REG(0)
    REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PRESCALE, 0);
    set_TMR0_PERIOD(2667); // Para una frecuenca de 50kHz 2^15 = 1<<15¿?
    REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD_UPMETHOD, 0);
    // TIMER0_CFG1
    REG_SET_FIELD(MCPWM_TIMER0_CFG1_REG(0), MCPWM_TIMER0_START, 2); //  2: PWM timer0 starts and runs on
    REG_SET_FIELD(MCPWM_TIMER0_CFG1_REG(0), MCPWM_TIMER0_MOD, 1);   // Metodo cuenta incremetal
    // TIMER0_SYNC
    REG_SET_FIELD(MCPWM_TIMER0_SYNC_REG(0), MCPWM_TIMER1_SYNCO_SEL, 3); //  other-wise: sync_out is always 0.

    // TIMER0_SINCI
    REG_SET_FIELD(MCPWM_TIMER_SYNCI_CFG_REG(0), MCPWM_TIMER0_SYNCISEL, 7); // other values: no sync input selected.
    // MCPWM_OPERATOR_TIMERSEL_REG
    REG_SET_FIELD(MCPWM_OPERATOR_TIMERSEL_REG(0), MCPWM_OPERATOR0_TIMERSEL, 0); // Select the PWM timer for PWM operator0’s timing reference. 0:timer0

    // PWM Operator 0 Configuration and Status
    // PWM_GEN0_STMP_CFG_REG
    REG_SET_FIELD(MCPWM_GEN0_STMP_CFG_REG(0), MCPWM_GEN0_A_UPMETHOD, BIT0); // Updating method for PWM generator 0 time stamp A’s active register. When all bits are set to 0: immediately; when bit0 is set to 1: TEZ
    set_DutyCicle(1333);

    // MCPWM_GEN0_A_REG
    REG_SET_FIELD(MCPWM_GEN0_A_REG(0), MCPWM_GEN0_A_UTEZ, 2); // PWM-0A HIGH at UTEZ signal
    REG_SET_FIELD(MCPWM_GEN0_A_REG(0), MCPWM_GEN0_A_UTEA, 1); // PWM-0A LOW at UTEA signal
    // MCPWM_GEN0_B_REG
    REG_SET_FIELD(MCPWM_GEN0_B_REG(0), MCPWM_GEN0_B_UTEZ, 1); // PWM-0B LOW at UTEZ signal
    REG_SET_FIELD(MCPWM_GEN0_B_REG(0), MCPWM_GEN0_B_UTEA, 2); // PWM-0B HIGH at UTEA signal

    Serial.print("GEN0A: ");
    Serial.println(REG_READ(MCPWM_GEN0_A_REG(0)));
    Serial.print("GEN0B: ");
    Serial.println(REG_READ(MCPWM_GEN0_B_REG(0)));
    // MCPWN DeathTime conf
    REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_A_OUTBYPASS, 1);
    REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_B_OUTBYPASS, 1);
}
