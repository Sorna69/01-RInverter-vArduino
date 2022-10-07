void initWiFiSTA(bool useStaticIP = false)
{
  Serial.println("");
  // WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_STA, password_STA);
  if (useStaticIP)
    WiFi.config(ip_STA, gateway_STA, subnet_STA);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print('.');
  }

  Serial.println("");
  Serial.print("Iniciado STA:\t");
  Serial.println(ssid_STA);
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void initWiFiAP(bool useStaticIP = false)
{
  Serial.println("");
  // WiFi.mode(WIFI_AP);
  while (!WiFi.softAP(ssid_AP, password_AP))
  {
    Serial.println(".");
    delay(100);
  }
  if (useStaticIP)
    WiFi.softAPConfig(ip_AP, gateway_AP, subnet_AP);

  Serial.println("");
  Serial.print("Iniciado AP:\t");
  Serial.println(ssid_AP);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
}