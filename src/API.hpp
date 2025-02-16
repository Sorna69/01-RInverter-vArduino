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
// IMPORTANTE: CREO QUE ES MEJOR METER TODA LA ACUTALZIACIÓN DEL PWM EN UNA SOLA FUNCION. ES DECIR, AUNQUE
// SOLO CAMBIE UNO DE LSO PARÁMETROS, SE DEBEN ACTUALIZAR TODOS LOS REGISTROS DEL PWM (FRECUENCIA y DUTY CYCLE)
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

  // Lo que tenga que hacer el PWM, cambiar frecuencia y/o Duty Cicle
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


