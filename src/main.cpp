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

  // Conectar_WiFi_AP();
  Conectar_WiFi_STA();
  Conectar_WiFi_AP();
  //Conectar_WiFi_STA();
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

  Serial.print("Registro PCOUNTer antes: ");
  Serial.println(  REG_GET_FIELD(GPIO_FUNC39_IN_SEL_CFG_REG,GPIO_FUNC39_IN_SEL));
   confGPIOPCNT();
  Serial.print("Registro PCOUNTer despues: ");
  Serial.println(REG_GET_FIELD(GPIO_FUNC39_IN_SEL_CFG_REG,GPIO_FUNC39_IN_SEL));

  //Serial.print("Registro PCOUNTer antes: ");
  //Serial.println(REG_READ(PCNT_U0_CONF0_REG));
  confPCOUNTER();
  //Serial.print("Registro PCOUNTer despues: ");
  //Serial.println(REG_READ(PCNT_U0_CONF0_REG));

  //Serial.print("Registro PCOUNTer antes: ");
  //Serial.println(REG_READ(PCNT_CTRL_REG));
  startPCOUNTER();
  //Serial.print("Registro PCOUNTer despues: ");
  //Serial.println(REG_READ(PCNT_CTRL_REG));

}

void loop()
{
  Serial.print("Counter value: ");
  Serial.println(REG_READ(PCNT_U0_CNT_REG));
  delay(1000);
}