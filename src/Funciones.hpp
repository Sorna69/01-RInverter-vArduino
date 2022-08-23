void Conectar_WiFi_STA(bool useStaticIP = false)
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

void Conectar_WiFi_AP(bool useStaticIP = false)
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

// AsyncWebSocketUtil
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->ping();

    // al conectarse enviar los estados de los GPIO (Para mas estados incluir bucle)
    updateGPIO(LED_BUILTIN, digitalRead(LED_BUILTIN), client->id());
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
        ProcessRequest(client, msg);
      // ProcessRequestNano(client, msg);
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
            ProcessRequest(client, msg);

          // ProcessRequestNano(client, msg);
        }
      }
    }
  }
}

void InitWebSockets()
{
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  Serial.println("WebSocket server started");
}

