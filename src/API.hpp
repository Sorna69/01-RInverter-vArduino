void setGPIO(const int id, bool state)
{
  // char idChar[id.length()];
  // id.toCharArray(idChar, 10);
  Serial.print("Set GPIO ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(state);
  digitalWrite(id, state);

  if (id == DIS)
  {
    digitalWrite(LED_BUILTIN, digitalRead(id));
  }
}

// MODIFICAR id Por const int (aunque solo hay 1) y añadir un int para la frecuencia
// ACTUALMENTE CONFIGURADO PAR AQUE EL DUTY CYCLE SEA SIEMPRE DEL 50% HAY QUE SACAR FUERA LA FUNCION setDutyCicle
void setTpwm(int id, uint16_t pwm)
{
  Serial.print("Set PWM ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(pwm);
  
  setTimer0Period(pwm);
  //ESTI SALDRA FUERA DEK CODIGO
  setDutyCicle((uint16_t)pwm / 2);

  // Lo que tenga que hacer el PWM, cambiar frecuencia y/o Duty Cicle
  // digitalWrite(id, state);
}


// ACTUALMENTE CONFIGURADO PAR AQUE EL DUTY CYCLE SEA SIEMPRE DEL 50% HAY QUE SACAR FUERA LA FUNCION setDutyCicle
void setDuty(int id, uint16_t duty)
{
  Serial.print("Set Duty ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(duty);
  
  //ME HE QUEDADO AQUI
  //setDutyCicle((uint16_t)pwm / 2);
}

void setDAC(int id, uint16_t dac)
{
  Serial.print("Set DAC ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(dac);
  const int raw = dac * 255 / 3300;
// Inicialización del DAC
 dacWrite(DAC1, raw); 
}


