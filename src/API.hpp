//#include "pwm.hpp"

void setGPIO(const int id, bool state)
{
  // char idChar[id.length()];
  // id.toCharArray(idChar, 10);
  Serial.print("Set GPIO ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(state);
  digitalWrite(id, state);
  digitalWrite(22, state);
}

// SE PODRIA UTILIZAR SOLO LA FUNCION setGPIO y dentro de esta diferenciar entre GPIO de uso genera y PWM (con un if)
void enablePWM(const int id, bool state)
{
  // char idChar[id.length()];
  // id.toCharArray(idChar, 10);
  Serial.print("Set GPIO ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(state);
  switch (id)
  {
  case 15:
    if (state)
    {
      // REG_SET_FIELD(GPIO_FUNC"id"_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 32);
      REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 32);
    }
    else
    {
      // Si el PWM esta apagado, entonces GPIO como salida y apagada
      // Devuelve el control de GPIO a los registros ENABLE y OUT
      REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 256);
      REG_WRITE(GPIO_OUT_W1TC_REG, BIT15); // GPIO2 LOW (clear)
      // digitalWrite(id, state);
    }
    break;
    case 18:
    if (state)
    {
      REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 33);
    }
    else
    {

      REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 256);
      REG_WRITE(GPIO_OUT_W1TC_REG, BIT18);
    }
    break;

    default:
    Serial.println("Error, no se reconoce el GPIO-PWM");
    break;
  }
}

// MODIFICAR id Por const int (aunque solo hay 1) y añadir un int para la frecuencia
void setTpwm(int id, uint16_t pwm)
{
  Serial.print("Set PWM ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(pwm);
  set_TMR0_PERIOD(pwm);
  set_DutyCicle((uint16_t)pwm / 2);

  // Lo que tenga que hacer el PWM, cambiar frecuencia y/o Duty Cicle
  // digitalWrite(id, state);
}