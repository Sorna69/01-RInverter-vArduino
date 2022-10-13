#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>
#include <soc/gpio_reg.h>
#include <soc/dport_reg.h>
#include <soc/dport_access.h>

// FUNCIONES
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

void enablePWM(bool state)
{
  // Reescribir para incluir ID del PWM (Funcion genérica para todos PWMs)
  Serial.print("Enable PWM ");
  Serial.print(": ");
  Serial.println(state);

  // Habría que añadir un SWITCH CASE en este punto para diferencia entre los distintos PWMs
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

// HABILITACIÓN DEL MÓDULO MCPWM
void enableMCPWMClck()
{
  // Habilitación del reloj del periférico PMW0 (Se tiene que utilizar las macros DPORT_REG_xxx)
  DPORT_REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_PWM0_CLK_EN);

  // Se resetea el periférico para que se guarde la configuración.REVISAR QUE HACE Y QU ENO HACE FALAT AAQJUI IMPORTANTE
  DPORT_REG_SET_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
  DPORT_REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
  // Revisar funcionalidad AHBLITE
  DPORT_REG_WRITE(DPORT_AHBLITE_MPU_TABLE_PWM0_REG, 1);
}

// 3) CONFIGURACION DEL SUBMÓDULO DEAD TIME GENERADOR
void confDtSM()
{
  // PWMxAoutput = PWMxAinput (Bypass A inptu --> *S1 = 1) (* Valor Por defecto--> No se modifica el registro)
  // REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_A_OUTBYPASS, 1); // S1

  // PWMxAoutput = !PWMxAinput(FED+RED) (Bypass A inptu --> S0 = *S4 = *S7 = 0; S3 = S8 = 1)
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_B_OUTBYPASS, 0);   // S0
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_FED_OUTINVERT, 1); // S3
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_DEB_MODE, 1);      // S8

  // Update Methods RED/FED (Actualizar con señal TEZ)
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_RED_UPMETHOD, 1); // bit0 is set to 1: TEZ;
  REG_SET_FIELD(MCPWM_DT0_CFG_REG(0), MCPWM_DT0_FED_UPMETHOD, 1); // bit0 is set to 1: TEZ;

  // // Desplazamiento inicial (FED = RED = 800). DTi = 800*6.25ns = 5us. Alternativamente usar: setPwmDT(800);
  REG_SET_FIELD(MCPWM_DT0_RED_CFG_REG(0), MCPWM_DT0_RED, 800);
  REG_SET_FIELD(MCPWM_DT0_FED_CFG_REG(0), MCPWM_DT0_FED, 800);
}

// CONFIGURACIÓN DEL MÓDULO MCPWM
void confMCPWM()
{
  // 1) CONFIGURACION DEL SUBMÓDULO DIVISOR DE RELOJ [CLOCK PREESCALER]
  // Preescalado del MCPWM0_CLK = 0
  REG_SET_FIELD(MCPWM_CLK_CFG_REG(0), MCPWM_CLK_PRESCALE, 0);

  // 2) CONFIGURACION DEL TIMER0
  // Metodo cuenta incremetal
  REG_SET_FIELD(MCPWM_TIMER0_CFG1_REG(0), MCPWM_TIMER0_MOD, 1);
  // Salida de sincronismo siempre 0.
  REG_SET_FIELD(MCPWM_TIMER0_SYNC_REG(0), MCPWM_TIMER1_SYNCO_SEL, 3);
  // Sin entrada de sincronismo.
  REG_SET_FIELD(MCPWM_TIMER_SYNCI_CFG_REG(0), MCPWM_TIMER0_SYNCISEL, 7);
  // Método de actualización TEZ
  REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD_UPMETHOD, 0);

  // Preescalado del TMR0clk = 0)
  REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PRESCALE, 0);
  // Periodo inicial del PWM: Tpwmi = 3200*6.25ns = 20us--> Fpwm = 50kHz  . Alternativamente usar: setTimer0Period(3200);
  REG_SET_FIELD(MCPWM_TIMER0_CFG0_REG(0), MCPWM_TIMER0_PERIOD, 3200);
  // setTimer0Period(3200);

  // 3) CONFIGURACION DEL GENERADOR (Bloque del Submódulo OPERATOR0)
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

  // PWM timer0 starts and runs on
  REG_SET_FIELD(MCPWM_TIMER0_CFG1_REG(0), MCPWM_TIMER0_START, 2);
}

/** ASIGNACIÓN PAD/PINES A SEÑALES DEL PERIFÉRICO MCPWM [GPIO+IO-MUX]**
 * @brief
 * Para asignar un pin/pad (x) del ESP32 a una señal de salida (y) de uno de sus periféricos se deben seguir los siguientes pasos:
 * 1) CONFIGURACIÓN GPIO MATRIX:
 *    1.a) Asignación de la salida del periferico [signaly_out] al GPIO correspondiente [GPIOx_out] mediante la edición de GPIO_FUNCX_OUT_SEL (registro GPIO_FUNCX_OUT_SEL_CFG). Ver [Tabla 4.2 p. 52].
 *    1.b) Configuración del modo de salida del periférico mediante la edición de GPIO_FUNCX_OEN_SEL (registro GPIO_FUNCX_OUT_SEL_CFG).
 *         GPIO_FUNCx_OEN_SEL= 1 --> Señal de salida siempre activa.
 *         GPIO_FUNCx_OEN_SEL= 0 -->Señal de salida activa/inactiva según lógica interna.
 *    1.c) Configuración del driver de salida del GPIO correspondiente mediante la edición de GPIO_PINX_PAD_DRIVER (registro GPIO_PINX).
 *         GPIO_PINx_PAD_DRIVER = 1 --> Modo de salida drenador abierto.
 *         NOTA: Si el driver está funcionando en modo drenador abierto se puede activar/desactivar las resistencias internas de pull-up/down mediante la edición de FUN_WPU y FUN_WPD (registro IO_MUX_GPIOX) del GPIO correspondiente.
 *          FUN_WPX = 1 --> Resistencia Pull-down/up habilitada.
 *          FUN_WPX = 0 --> Resistencia Pull-down/up deshabilitada.
 *         GPIO_PINx_PAD_DRIVER = 0 --> Modo push/pull mode (por defecto).
 *    1.d) Hablitación del driver de salida mediante la edición de GPIO_ENABLE_DATA[X] (registro GPIO_ENABLE_REG [GPIOs 0-31] o GPIO_ENABLE1_REG [GPIOs 32-39])
 *         GPIO_ENABLE_DATA[x] =1 --> Driver de salida habilitado.
 *         GPIO_ENABLE_DATA[x] =0 --> Driver de salida deshabilitado.
 *
 * 2) CONFIGURACIÓN MULTIPLEXOR E/S [IO MUX]
 *    2.a) Asignar al Pad x la función GPIO mediante la edición de MCU_SEL (registro IO_MUX_GPIOX_REG). Ver [**Tabla: 4.3 (p. 57)**]
 *    2.b) Configuración de la potencia de salida del del pin/pad correspondiente  mediante la edición de FUN_DRV (registro IO_MUX_GPIOX_REG$. Cuanto mayor es la potencia de salida del driver más corriente puede manejar el pin.
 *         FUN_DRV = 0 --> Potencia de salida mínima.
 *         FUN_DRV = 3 --> Potencia de salida máxima.
 */
void confMCPWMGPIO()
{
  // 1) CONFIGURACIÓN MATRIZ GPIO
  // 1.a) Asignación de la funcion 32 al al GPIO-15 [Tabla: 4.2 p. 52].
  REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 32);
  // 1.a) Asignación la funcion 33 al al GPIO-18 [Tabla: 4.2 p. 52].
  REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 33);
  // NOTA: Si se desea iniciar con las salidas PWM desactivadas: GPIO como salida y apagada
  // 1.a) Devuelve el control de GPIO-16 a los registros ENABLE y OUT
  // REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 256);
  // 1.a) Devuelve el control de GPIO-18 a los registros ENABLE y OUT
  // REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 256);
  // 1.x) GPIO-15 y GPIO-18 como a nivel bajo = LOW (clear)
  // REG_WRITE(GPIO_OUT_W1TC_REG, BIT15 + BIT18);

  // 1.b) Señal del GPIO-15 siempre como una salida.
  REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OEN_SEL, 1);
  // 1.b) Señal del GPIO-18 siempre como una salida.
  REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OEN_SEL, 1);
  // Altnernativamente: Señal del GPIO-15 se decide por lógica interna
  // REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OEN_SEL, 0);
  // Altnernativamente: Señal del GPIO-18 se decide por lógica interna
  // REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OEN_SEL, 0);

  // 1.c) Driver GPIO-15 Modo push/pull (valor por defecto).
  // REG_SET_FIELD(GPIO_PIN15_REG, GPIO_PIN15_PAD_DRIVER, 1);
  // 1.c) Driver GPIO-15 Modo push/pull (valor por defecto).
  // REG_SET_FIELD(GPIO_PIN18_REG, GPIO_PIN18_PAD_DRIVER, 1);
  // NOTA: Si driver de salida en modo OPEN DRAIN se habilitan/deshabilitan las resistencias internas de pull-up/down
  // REG_SET_FIELD(IO_MUX_GPIO15_REG,FUN_WPU,0);
  // REG_SET_FIELD(IO_MUX_GPIO15_REG,FUN_WPD,0);

  // 1.d) Habilitación del driver de salida del GPIO-15 y GPIO-18
  REG_SET_FIELD(GPIO_ENABLE_REG, GPIO_ENABLE_DATA, BIT15 + BIT18);

  // 2) CONFIGURACIÓN IO_MUX.
  // 2.a) MCU_SEL (PAD15) = 2 --> PAD15 = GPIO15 [Tabla: 4.3 (p. 57)]
  REG_SET_FIELD(IO_MUX_GPIO15_REG, MCU_SEL, 2);
  // 2.a) MCU_SEL (PAD18) = 0 --> PAD18 = GPIO18 [Tabla: 4.3 (p. 57)]
  REG_SET_FIELD(IO_MUX_GPIO18_REG, MCU_SEL, 0);
  // 2.b) Configuración de la potencia de salida del pad.
  REG_SET_FIELD(IO_MUX_GPIO15_REG, FUN_DRV, 2);
  REG_SET_FIELD(IO_MUX_GPIO15_REG, FUN_DRV, 2);
}