// Instanciazión de objetos AsynWebSocket y AsyncWebServer
AsyncWebSocket ws("/ws");
AsyncWebServer server(80);

// Funciones generadoras de respuesta a los comandos
void conectionOk(int id_cliente)
{
  String response;
  StaticJsonDocument<300> doc;
  doc["command"] = "conectionOk";
  doc["client-id"] = id_cliente;
  serializeJson(doc, response);

  ws.textAll(response);
  // Deberia ser posible enviar a un cliente concreto, pero no funciona
  // sin embargo, si activo los 2 el mensaje se envia 2 veces
  // ws.text(id_cliente, response);
  Serial.print("connected to id_cliente: ");
  Serial.println(id_cliente);
}

void initialStatus(const int output, bool value, int id_cliente)
{
  String response;
  StaticJsonDocument<300> doc;
  doc["command"] = "initialStatus";
  doc["id"] = output;
  doc["status"] = value ? String("ON") : String("OFF");
  serializeJson(doc, response);

  ws.textAll(response);
  // Deberia ser posible enviar a un cliente concreto, pero no funciona
  // sin embargo, si activo los 2 el mensaje se envia 2 veces
  // ws.text(id_cliente, response);
  Serial.print("initialStatus. id_cliente: ");
  Serial.println(id_cliente);
  Serial.print("initialStatus. id: ");
  Serial.print(output);
  Serial.println(value ? String(" ON") : String(" OFF"));
}

void updateData(char *Datos)
{
  // MEJORA: Utilizar un Char (Array) en lugar de String e ¿incluir Status de los LEDS?
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

void statusGPIO(const int output, bool value, int id_cliente)
{
  // Si quisiese una sola funcion que generase respuesta, en lugar de VOID la harai con retur JSon
  // y enviaria la respuesta al final del bucle de ProcesssRequest.
  // Esto permitiría hacer una sola funcion para gestionar la respuesta al server
  String response;
  StaticJsonDocument<300> doc;
  doc["command"] = "statusGPIO";
  doc["id"] = output;
  doc["status"] = value ? String("ON") : String("OFF");
  serializeJson(doc, response);

  ws.textAll(response);
  // Deberia ser posible enviar a un cliente concreto, pero no funciona
  // sin embargo, si activo los 2 el mensaje se envia 2 veces
  // ws.text(id_cliente, response);
  Serial.print("updateGPIO. id_cliente: ");
  Serial.println(id_cliente);
  Serial.print("updateGPIO. id: ");
  Serial.print(output);
  Serial.println(value ? String(" ON") : String(" OFF"));
}

// Funciones para la gestión de los comandos recibidos
void fSetGPIO(const int id, const bool state)
{
  // char idChar[id.length()];
  // id.toCharArray(idChar, 10);
  Serial.print("Set GPIO ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(state);
  digitalWrite(id, state);

  if (id == DIS_BUTTON)
  {
    digitalWrite(LED_BUILTIN, digitalRead(id));
  }
}

void setTpwm(const int id, const uint16_t pwm)
{
  Serial.print("Set PWM ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(pwm);

  setTimer0Period(pwm);
  setDutyCicle((uint16_t)pwm / 2);
  setPwmDT((uint16_t)pwm / 4);
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

  String command = doc["command"];

  if (command == "requestIntialStatus")
  {
    initialStatus(DIS_BUTTON, digitalRead(DIS_BUTTON), client->id());
  }

  else if (command == "setGPIO")
  {
    // char iden[10];
    // String id = doc["command"];
    // id.toCharArray(iden,10);
    const int id = doc["id"];
    // const String tipo = doc["tipo"]

    if (doc["tipo"] == "GPIO")
    {
      fSetGPIO(id, (bool)doc["status"]);
      // Mensaje Confirmacion/Respuesta
      // Para que estuviese mejor realmente harúia falta un IF (IF STATUS = digital Read entonces responder)
      statusGPIO(id, digitalRead(id), client->id());
    }
    else if (doc["tipo"] == "pwm")
    {
      enablePWM((bool)doc["status"]);
      // NECESITA MEJORA?
      statusGPIO(id, digitalRead(id), client->id());
    }
  }

  else if (command == "setPWM")
  {
    const int id = doc["id"];
    const uint16_t Tpwm = (uint16_t)doc["pwm"];
    setTpwm(id, Tpwm);
    // ws.textAll(request);
  }
  else
  {
    Serial.println("Comando no reconocido");
  }
  // echo
  // ws.textAll(request);
  // ws.text(client->id(),request);
}

void ProcessRequestMod(AsyncWebSocketClient *client, String request)
{
  enum ecommand
  {
    requestIntialStatus,
    setGPIO,
    setPWM
  };

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, request);

  // char iden[10];
  // String id = doc["command"];
  // id.toCharArray(iden,10);
  // const String tipo = doc["tipo"]
  const int id = doc["id"];
  ecommand command = doc["command"];

  if (error)
  {
    return;
  }

  Serial.println("1) Subrutina ProcessRequestMod");
  Serial.print("Request: ");
  Serial.println(request);

  switch (command)
  {
  case requestIntialStatus:
  {
    initialStatus(DIS_BUTTON, digitalRead(DIS_BUTTON), client->id());
  }
  break;

  case setGPIO:
  {
    if (doc["tipo"] == "GPIO")
    {
      fSetGPIO(id, (bool)doc["status"]);
      // Mensaje Confirmacion/Respuesta
      // Para que estuviese mejor realmente harúia falta un IF (IF STATUS = digital Read entonces responder)
      statusGPIO(id, digitalRead(id), client->id());
    }
    else if (doc["tipo"] == "pwm")
    {
      enablePWM((bool)doc["status"]);
      // NECESITA MEJORA?
      statusGPIO(id, digitalRead(id), client->id());
    }
    break;
  }

  case setPWM:
  {
    const uint16_t Tpwm = (uint16_t)doc["pwm"];
    setTpwm(id, Tpwm);
    // ws.textAll(request);
    break;
  }

  default:
    Serial.println("Comando no reconocido");
    break;
  }
  // echo
  // ws.textAll(request);
  // ws.text(client->id(),request);
}

// AsyncWebServer y AsyncWebSocketUtil
void initServer()
{
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(400, "text/plain", "Not found"); });

  server.begin();
  Serial.println("HTTP server started");
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->ping();
    // Al conectarse un cliente, se envía un mensaje de confirmación
    conectionOk(client->id());
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.printf("ws[%s][%u] disconnect: \n", server->url(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    // Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  }
  else if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len)
    {
      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < info->len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < info->len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }

      if (info->opcode == WS_TEXT)
        // ProcessRequest(client, msg);
        ProcessRequestMod(client, msg);
    }
    else
    {
      // message is comprised of multiple frames or the frame is split into multiple packets
      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len)
      {
        if (info->final)
        {
          if (info->message_opcode == WS_TEXT)
            // ProcessRequest(client, msg);
            ProcessRequestMod(client, msg);
        }
      }
    }
  }
}

void initWebSockets()
{
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  Serial.println("WebSocket server started");
}