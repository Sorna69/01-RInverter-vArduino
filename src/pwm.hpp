#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>

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