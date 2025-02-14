AsyncWebSocket ws("/ws");


// PROBAR SI EL FALLO EN ENABLE Y DISABLE TIENE QUE VER CON TRUE/FALSE VS ON/OFF

// Comando enviados al fronte
// Deserializa el JSON a un String mas sencillo para enviar a arduino
// MEJORAR: PASAR A CHAR ARRAY E INCLUIR STATUS DE LOS LEDS

void updateDataMod(char *Datos)
{
  String response;
  char *token;
  StaticJsonDocument<300> doc;

  doc["command"] = "updateDATA";

  // get the first token
  token = strtok(Datos, "-");
  if (token != NULL)
  {
    doc["pHData"] = atoi(token);
    token = strtok(NULL, "-");
    if (token != NULL)
    {
      doc["tempData"] = atoi(token);
      token = strtok(NULL, "-");
      if (token != NULL)
      {
        doc["modeData"] = atoi(token);
      }
    }
  }

  serializeJson(doc, response);
  // Serial.println(response);
  ws.textAll(response);
}

void updateGPIO(int input, bool value, int id_cliente)
{
  // Si quisiese una sola funcion que generase respuesta, en lugar de VOID la escribiría con retur JSon
  // y enviaria la respuesta al final del bucle de ProcesssRequest.
  // Esto permitiría hacer una sola funcion para gestionar la respuesta al server
  String response;
  StaticJsonDocument<300> doc;
  doc["command"] = "updateGPIO";
  doc["id"] = input;
  doc["status"] = value ? String("ON") : String("OFF");
  serializeJson(doc, response);

  ws.textAll(response);
  // Deberia ser posible enviar a un cliente concreto, pero no funciona
  // sin embargo, si activo los 2 el mensaje se envia 2 veces
  // ws.text(id_cliente, response);
  Serial.print("updateGPIO. id_cliente: ");
  Serial.println(id_cliente);
  Serial.print("updateGPIO. id: ");
  Serial.print(input);
  Serial.println(value ? String(" ON") : String(" OFF"));
}

void ProcessRequest(AsyncWebSocketClient *client, String request)
{
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, request);
  if (error)
  {
    return;
  }

  Serial.println("1) Subrutina ProcessRequest");
  Serial.print("Request: ");
  Serial.println(request);

  String device = doc["device"];

  if (device == "ESP32")
  {
    String command = doc["command"];

    if (command == "setGPIO")
    {
      // char iden[10];
      // String id = doc["command"];
      // id.toCharArray(iden,10);
      const int id = doc["id"];
      setGPIO(id, (bool)doc["status"]);
      // Mensaje Confirmacion
      updateGPIO(id, digitalRead(id), client->id());
    }
     else   if (command == "enablePWM")
    {
      const int id = doc["id"];
      enablePWM(id, (bool)doc["status"]);
      // NECESITA SER MODIFICADO
      //updateGPIO(id, digitalRead(id), client->id());
    }
    else if (command == "setPWM")
    {
      const int id = doc["id"];
      const uint16_t Tpwm = (uint16_t)doc["Tpwm"];
      const uint8_t Duty = (uint8_t)doc["Duty"];
      setTpwm(id, Tpwm, Duty);

      // setPWM(doc["id"], (uint16_t)doc["pwm"]);
      // ws.textAll(request);
    }


    //COMANDOS NUEVOS, REVISAR
    else if (command == "setDuty")
    {
      const int id = doc["id"];
      const uint16_t duty = (uint16_t)doc["Duty"];
      setDuty(id, duty);

      // setPWM(doc["id"], (uint16_t)doc["pwm"]);
      // ws.textAll(request);
    }

    else if (command == "setPWM")
    {
      const int id = doc["id"];
      const uint16_t dac = (uint16_t)doc["DAC"];
      setDAC(id, dac);
    }

    //FIN COMANDOS NUEVOS

    // echo
    // ws.textAll(request);
    // ws.text(client->id(),request);
  }

  else
  {
    Serial.println("Dispositivo no reconocido");
  }
}