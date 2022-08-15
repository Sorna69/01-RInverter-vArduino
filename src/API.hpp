//#include "pwm.hpp"

void setGPIO(const int id, bool state) {
  //char idChar[id.length()];
  //id.toCharArray(idChar, 10);
  Serial.print("Set GPIO ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(state);
  digitalWrite(id, state);
  if(id == 22){
  digitalWrite(LED_BUILTIN, digitalRead(DIS));
  }

  // Original
  // digitalWrite(id, state);
  // digitalWrite(22, state);
}

// MODIFICAR id Por const int (aunque solo hay 1) y añadir un int para la frecuencia
void setPWM(const int id, uint16_t Tpwm) {
  Serial.print("Set PWM ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(Tpwm);

  set_TMR0_PERIOD(Tpwm);
  set_DutyCicle((uint16_t)Tpwm/2);

  // Lo que tenga que hacer el PWM, cambiar frecuencia y/o Duty Cilce
  //digitalWrite(id, state);

}