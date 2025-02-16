void setGPIO(const int id, bool state)
{
  // char idChar[id.length()];
  // id.toCharArray(idChar, 10);
  Serial.print("Set GPIO ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(state);
  digitalWrite(id, state);

  // No entiendo este bucle de aqui
  if (id == MOTOR_SWITCH)
  {
    digitalWrite(LED_BUILTIN, digitalRead(id));
  }
}

// MODIFICAR id Por const int (aunque solo hay 1) y añadir un int para la frecuencia?
void setTpwm(int id, uint16_t Tpwm, uint16_t Duty)
{
  Serial.print("Set PWM-");
  Serial.print(id);
  Serial.print("//Tpwm: ");
  Serial.println(Tpwm);
  Serial.print("//Duty: ");
  Serial.println(Duty);
	  
  setTimer0Period(Tpwm);
  setDutyCicle((uint16_t)Duty);
  // digitalWrite(id, state);
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


