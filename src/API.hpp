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
    digitalWrite(LED_BUILTIN, digitalRead(DIS));
  }
}