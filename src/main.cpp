#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>

#include "Config.h"

#include "pwm.hpp"
#include "AsyncWebServer.hpp"
#include "API.hpp"
#include "AsWebSockets.hpp"
#include "Funciones.hpp"

void setup()
{

  Serial.begin(115200);
  SPIFFS.begin();

  Conectar_WiFi_AP();
  // Conectar_WiFi_STA();
  InitServer();
  InitWebSockets();

  conf_DPORT();
  conf_MCPWM();
  conf_GPIO();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(22, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(22, HIGH);
}

void loop()
{

  // test2_MCPWM();
  // delay(1000);
  // test_MCPWM_CLK_CFG_REG();
  // delay(1000);
  // test_MCPWM_TIMER0_CFG0_REG();
  // leer_TIMER0();
  // set_DutyCicle(10000);
  // delay(1000);
}