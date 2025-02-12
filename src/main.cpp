#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>

#include "Config.h"

#include "ESP32Perip.hpp"
#include "AsyncWebServer.hpp"
#include "API.hpp"
#include "AsWebSockets.hpp"
#include "Funciones.hpp"

void setup()
{

  Serial.begin(115200);
  SPIFFS.begin();

  Conectar_WiFi_AP();
  //Conectar_WiFi_STA();
  InitServer();
  InitWebSockets();

  enableMCPWMClck();
  confMCPWM();
  confMCPWMGPIO();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(22, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(22, HIGH);
}

void loop()
{

}