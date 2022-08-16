#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#include "Config.h"

#include "Esp32Perip.hpp"
#include "AsyncWebServer.hpp"
#include "API.hpp"
#include "AsWebSockets.hpp"
#include "Funciones.hpp"

void setup()
{

  Serial.begin(115200);
  SPIFFS.begin();

  //Conectar_WiFi_AP();
  Conectar_WiFi_STA();
  InitServer();
  InitWebSockets();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIS, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(DIS, HIGH);
  digitalWrite(PWM_A, LOW);
  digitalWrite(PWM_B, LOW);

  enablePCOUNTERCLK();
  confGPIOPCNT();
  confPCOUNTER();
}

void loop()
{

}