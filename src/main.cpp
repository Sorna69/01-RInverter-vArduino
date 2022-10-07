#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>

//#include "driver/mcpwm.h"

#include "Config.h"

#include "ESP32Perip.hpp"
#include "Inicializacion.hpp"
#include "AsWebSockets.hpp"



void setup()
{

  Serial.begin(115200);
  SPIFFS.begin();

  //initWiFiAP();
  initWiFiSTA(true);
  initServer();
  initWebSockets();

  enableMCPWMClck();
  confMCPWMGPIO();
  confMCPWM();
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DIS_BUTTON, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(DIS_BUTTON, HIGH);
}

void loop()
{
}