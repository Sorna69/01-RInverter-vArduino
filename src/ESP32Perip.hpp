#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>
#include <soc/gpio_reg.h>
#include <soc/dport_reg.h>
#include <soc/dport_access.h>

void setTimer0Period(uint16_t Periodo)
{
  // Periodo PWM: Tpwm = Periodo*6.25ns
  REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD, Periodo);
  // Serial.print("Tpwm: ");
  // Serial.println(REG_GET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD));
}

void setDutyCicle(uint16_t Duty)
{
  // Ciclo de trabajo del pwm: Dtpwm = Duty*6.25ns
  REG_SET_FIELD(MCPWM_GEN0_TSTMP_A_REG(0), MCPWM_GEN0_A, Duty);
}

void setPwmDT(uint16_t DTime)
{
  // Tiempos de retardo de flanco de subida y bajada (RED y FED): FED = RED = Dtime*6.25ns
  REG_SET_FIELD(MCPWM_DT0_RED_CFG_REG(0), MCPWM_DT0_RED, DTime);
  REG_SET_FIELD(MCPWM_DT0_FED_CFG_REG(0), MCPWM_DT0_FED, DTime);
}

void enableMCPWMClck()
{
  // Habilitación del reloj del periférico PMW0 (Se tiene que utilizar las macros DPORT_REG_xxx)
  DPORT_REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_PWM0_CLK_EN);

  // Se debe resetear el periférico para que se guarde la configuración.REVISAR QUE HACE Y QU ENO HACE FALAT AAQJUI IMPORTANTE
  DPORT_REG_SET_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
  DPORT_REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
  // Revisar funcionalidad AHBLITE
  DPORT_REG_WRITE(DPORT_AHBLITE_MPU_TABLE_PWM0_REG, 1);
}

/** CONFIGURACION GPIO I/O
 * @brief
 * To output peripheral signal Y to particular GPIO pad X, follow these steps:
 * 1) CONFIGURACIÓN GPIO MATRIX:
 *  1.a) Asignar las funciones (y) 32/33 al GPIO correspondiente (x) GPIO15/18 [Tabla: 4.2 (p. 52)].Set the GPIO_FUNCx_OUT_SEL
 *       field in GPIO_FUNCx_OUT_SEL_CFG to the numeric index (Y) of desired peripheral output signal Y.
 *  1.b) Se configura la señal siempre como una salida. If the signal should always be enabled as an output, set the GPIO_FUNCx_OEN_SEL
 *       bit in the GPIO_FUN Cx_OUT_SEL_CFG register and the GPIO_ENABLE_DATA[x] field in the GPIO_ENABLE_REG register
 *       corresponding to GPIO pad X. To have the output enable signal decided by internal logic, clear the GPIO_FUNCx_OEN_SEL bit instead.
 *  1.c) The GPIO_ENABLE_DATA[x] field is a bit in either GPIO_ENABLE_REG (GPIOs 0-31) or GPIO_ENABLE1_REG (GPIOs 32-39). Clear this bit to
 *       disable the output driver for the GPIO pad.
 *
 * 2) CONFIGURACIÓN DRIVER SALIDA:2. For an open drain output: set the GPIO_PINx_PAD_DRIVER bit in the GPIO_PINx register corresponding to GPIO pad X. 
 *    For push/pull mode (default), clear this bit.
 *
 * 3) CONFIGURACIÓN IO_MUX:
 *  3.a) Selección de la funcion del PADx [Tabla: 4.3 (p. 57)]: Set the function field (MCU_SEL) to the IO_MUX function corresponding to GPIO X.
 *  3.b) Configuración de la potencia de salida del pad (0: min//3:max): Set the FUN_DRV field to the desired value for output strength (0-3).
 *       The higher the drive strength, the more current can be sourced/sunk from the pin.
 *  3.c) If using open drain mode, set/clear the FUN_WPU and FUN_WPD bits to enable/disable the internal
 *       pull-up/down resistors.
 */
void confMCPWMGPIO()
{
  // 1) CONFIGURACIÓN GPIO MATRIX
  //  1.a) Asignar las funciones (y) 32/33 al GPIO correspondiente (x) GPIO15/18 [Tabla: 4.2 (p. 52)].
  REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 32);
  REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 33);

  // 1.b) Se configura la señal siempre como una salida.
  REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OEN_SEL, 1);
  REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OEN_SEL, 1);
  REG_SET_FIELD(GPIO_ENABLE_REG, GPIO_ENABLE_DATA, BIT15 + BIT18);
  // Altnernativamente: si la habilitación de la señal de salida se decide por lógica interna, entonces:
  // REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OEN_SEL, 0);
  // REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OEN_SEL, 0);

  //1.c) (MÉTODO ALTERNATIVO) Si se desea iniciar con las salidas PWM desactivadas: GPIO como salida y apagada 
  //REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 256); // Devuelve el control de GPIO a los registros ENABLE y OUT
  //REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 256); // Devuelve el control de GPIO a los registros ENABLE y OUT
  //REG_WRITE(GPIO_OUT_W1TC_REG, BIT15 + BIT18);                          // Pone GPI18 + GPIO18 como salidas a nivel bajo = LOW (clear) [REVISAR]

  // 2) CONFIGURACIÓN DRIVER SALIDA: [bit GPIO_PINx_PAD_DRIVER del REGISTRO GPIO_PINx_REG.]: 0 Normal. 1: open-drain
  // Se deja el valor por defecto (0): Salida normal.
  // REG_SET_FIELD(GPIO_PIN15_REG, GPIO_PIN15_PAD_DRIVER, 1);
  // REG_SET_FIELD(GPIO_PIN18_REG, GPIO_PIN18_PAD_DRIVER, 1);

  // 3) CONFIGURACIÓN IO_MUX.
  // 3.a) Selección de la funcion del PADx [Tabla: 4.3 (p. 57)]: PAD15 = GPIO15 (Función 2) PAD15 = GPIO18 (Función 0)
  REG_SET_FIELD(IO_MUX_GPIO15_REG, MCU_SEL, 2);
  REG_SET_FIELD(IO_MUX_GPIO18_REG, MCU_SEL, 0);
  // 3.b) Configuración de la potencia de salida del pad (0: min//3:max). 2 Por defecto.
  REG_SET_FIELD(IO_MUX_GPIO15_REG, FUN_DRV, 2);
  REG_SET_FIELD(IO_MUX_GPIO15_REG, FUN_DRV, 2);
  //  3c) Configuración de las resistencias internas de pull-up y pull-down (Para modo OPEN DRAIN)
  // REG_SET_FIELD(IO_MUX_GPIO15_REG,FUN_WPU,0);
  // REG_SET_FIELD(IO_MUX_GPIO15_REG,FUN_WPD,0);
}

void confDtSM()
{
  // PWMxAoutput = PWMxAinput (Bypass A inptu --> *S1 = 1) (* Valor Por defecto--> No se modifica el registro)

  // PWMxAoutput = !PWMxAinput(FED+RED) (Bypass A inptu --> S0 = *S4 = *S7 = 0; S3 = S8 = 1)
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_B_OUTBYPASS, 0);   // S0
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_FED_OUTINVERT, 0); // S3
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_FED_OUTINVERT, 1); // S3
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_DEB_MODE, 1);      // S8

  // Update Methods (Actualizar con señal TEZ)
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_RED_UPMETHOD, 1); // bit0 is set to 1: TEZ;
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_FED_UPMETHOD, 1); // bit0 is set to 1: TEZ;

  // Tiempo muerto inicial (FED = RED): DTi = 800*6.25ns = 5us. Alternativamente usar: setPwmDT(800);
  REG_SET_FIELD(MCPWM_DT0_RED_CFG_REG(0), MCPWM_DT0_RED, 800);
  REG_SET_FIELD(MCPWM_DT0_FED_CFG_REG(0), MCPWM_DT0_FED, 800);
}

void confMCPWM()
{
  // 0) CONFIGURACION DEL MCPWMclk [REGISTRO MCPWM_CLK_CFG_REG]
  // Preescalado del PWM0clk = 0
  REG_SET_FIELD(MCPWM_CLK_CFG_REG(0), MCPWM_CLK_PRESCALE, 0);

  // 1) CONFIGURACION DEL TIMER0
  // Preescalado del TMR0clk = 0)
  REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PRESCALE, 0);

  // Periodo inicial del PWM: Tpwmi = 3200*6.25ns = 20us--> Fpwm = 50kHz  . Alternativamente usar: setTimer0Period(3200);
  REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD, 3200);
  // setTimer0Period(3200);

  REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD_UPMETHOD, 0);

  REG_SET_FIELD(MCPWM_TIMER0_CFG1_REG(0), MCPWM_TIMER0_START, 2); // PWM timer0 starts and runs on
  REG_SET_FIELD(MCPWM_TIMER0_CFG1_REG(0), MCPWM_TIMER0_MOD, 1);   // Metodo cuenta incremetal

  REG_SET_FIELD(MCPWM_TIMER0_SYNC_REG(0), MCPWM_TIMER1_SYNCO_SEL, 3); // Sync_out is always 0.

  REG_SET_FIELD(MCPWM_TIMER_SYNCI_CFG_REG(0), MCPWM_TIMER0_SYNCISEL, 7); // No sync input selected.

  // 2) CONFIGURACION DEL OPERADOR
  // Asignacion del timer0 al operador PWM0
  REG_SET_FIELD(MCPWM_OPERATOR_TIMERSEL_REG(0), MCPWM_OPERATOR0_TIMERSEL, 0); // Select the PWM timer for PWM operator0’s timing reference. 0:timer0

  // Método de actualización del time stamp de generador PWM0: (TEZ)
  REG_SET_FIELD(MCPWM_GEN0_STMP_CFG_REG(0), MCPWM_GEN0_A_UPMETHOD, BIT0); // All bits are set to 0: immediately; when bit0 is set to 1: TEZ

  // setDutyCicle(1000);
  // Ciclo de trabajo inicial del pwm: Dtpwmi = 1600*6.25ns = 10us --> Dtpwmi = 50%
  REG_SET_FIELD(MCPWM_GEN0_TSTMP_A_REG(0), MCPWM_GEN0_A, 1600);

  // Configuración del PMWA (Señales hardware de ON y OFF) [Ver imagen]
  REG_SET_FIELD(MCPWM_GEN0_A_REG(0), MCPWM_GEN0_A_UTEZ, 2); // PWM-0A HIGH at UTEZ signal
  REG_SET_FIELD(MCPWM_GEN0_A_REG(0), MCPWM_GEN0_A_UTEA, 1); // PWM-0A LOW at UTEA signal

  //// Configuración del PMWB (Señales hardware de ON y OFF) [Ver imagen]. En su lugar se utiliza el Submódulo Dead Time Generator
  // REG_SET_FIELD(MCPWM_GEN0_B_REG(0), MCPWM_GEN0_B_UTEZ, 1); // PWM-0B LOW at UTEZ signal
  // REG_SET_FIELD(MCPWM_GEN0_B_REG(0), MCPWM_GEN0_B_UTEA, 2); // PWM-0B HIGH at UTEA signal

  // Submodulo DeadTime. Para desactivar el Submódulo, descomentar las siguientes líneas
  // REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_A_OUTBYPASS, 1);   //Bypass PWMAx
  // REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_B_OUTBYPASS, 1);   //Bypass PWMAx

  // Llamada a la función de configuración del Submódulo Dead Time Generator
  confDtSM();
}

// SE PODRIA UTILIZAR SOLO LA FUNCION setGPIO y dentro de esta diferenciar entre GPIO de uso genera y PWM (con un if)
void enablePWM(bool state)
{
  Serial.print("Enable PWM ");
  Serial.print(": ");
  Serial.println(state);

  if (state)
  {
    // Si estado = ON entonces se habilitan las salidas PWM
    REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 32);
    REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 33);
  }
  else if (!state)
  {
    // Si el PWM esta apagado, entonces GPIO como salida y apagada
    // Devuelve el control de GPIO a los registros ENABLE y OUT
    REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 256);
    REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 256);

    // GPI18 + GPIO18 OUT = LOW (clear)
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT15 + BIT18);
  }
  else
  {
    Serial.println("Error enable PWM");
  }
}