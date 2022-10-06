void setGPIO(const int id, bool state)
{
  // char idChar[id.length()];
  // id.toCharArray(idChar, 10);
  Serial.print("Set GPIO ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(state);
  digitalWrite(id, state);

  if (id == DIS_BUTTON)
  {
    digitalWrite(LED_BUILTIN, digitalRead(id));
  }
}

// MODIFICAR id Por const int (aunque solo hay 1) y añadir un int para la frecuencia
void setTpwm(int id, uint16_t pwm)
{
  Serial.print("Set PWM ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(pwm);
  
  setTimer0Period(pwm);
  setDutyCicle((uint16_t)pwm / 2);
  setPwmDT((uint16_t)pwm / 4);

  // Lo que tenga que hacer el PWM, cambiar frecuencia y/o Duty Cicle
  // digitalWrite(id, state);
}