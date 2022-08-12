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
}

// MODIFICAR id Por const int (aunque solo hay 1) y añadir un int para la frecuencia
void setPWM(int id, uint16_t pwm) {
  Serial.print("Set PWM ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(pwm);
  set_TMR0_PERIOD(pwm);
  set_DutyCicle((uint16_t)pwm/2);

  // Lo que tenga que hacer el PWM, cambiar frecuencia y/o Duty Cilce
  //digitalWrite(id, state);

}