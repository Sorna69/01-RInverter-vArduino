#include <soc/gpio_reg.h>
#include <soc/dport_reg.h>
#include <soc/dport_access.h>

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

/* CONFIGURACION GPIO I/O
To output peripheral signal Y to particular GPIO pad X, follow these steps:
1. Configure the GPIO_FUNCx_OUT_SEL_CFG register and GPIO_ENABLE_DATA[x] field corresponding to
GPIO X in the GPIO Matrix:
a) Set the GPIO_FUNCx_OUT_SEL field in GPIO_FUNCx_OUT_SEL_CFG to the numeric index (Y) of desired
peripheral output signal Y.
b) If the signal should always be enabled as an output, set the GPIO_FUNCx_OEN_SEL bit in the GPIO_FUN
Cx_OUT_SEL_CFG register and the GPIO_ENABLE_DATA[x] field in the GPIO_ENABLE_REG register
corresponding to GPIO pad X. To have the output enable signal decided by internal logic, clear the
GPIO_FUNCx_OEN_SEL bit instead.
c) The GPIO_ENABLE_DATA[x] field is a bit in either GPIO_ENABLE_REG (GPIOs 0-31) or GPIO_ENABLE1
_REG (GPIOs 32-39). Clear this bit to disable the output driver for the GPIO pad.

2. For an open drain output, set the GPIO_PINx_PAD_DRIVER bit in the GPIO_PINx register corresponding to
GPIO pad X. For push/pull mode (default), clear this bit.

3. Configure the IO_MUX to select the GPIO Matrix. Set the IO_MUX_x_REG register corresponding to GPIO
pad X as follows:
a) Set the function field (MCU_SEL) to the IO_MUX function corresponding to GPIO X (this is Function
2—numeric value 2—for all pins).
b) Set the FUN_DRV field to the desired value for output strength (0-3). The higher the drive strength, the
more current can be sourced/sunk from the pin.
c) If using open drain mode, set/clear the FUN_WPU and FUN_WPD bits to enable/disable the internal
pull-up/down resistors.
*/
void conf_GPIO()
{
  // Configuracion GPIO15/18 como Salida PWM0a (Func32/33)
  //  1a)
  REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OUT_SEL, 32);
  REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OUT_SEL, 33);

  // 1b)
  REG_SET_FIELD(GPIO_FUNC15_OUT_SEL_CFG_REG, GPIO_FUNC15_OEN_SEL, 1);
  REG_SET_FIELD(GPIO_FUNC18_OUT_SEL_CFG_REG, GPIO_FUNC18_OEN_SEL, 1);

  // 1c)
  REG_SET_FIELD(GPIO_ENABLE_REG, GPIO_ENABLE_DATA, BIT15 + BIT18);

  Serial.print("Valor de GPIO_FUNC15_OUT_SEL_CFG_REG: ");
  Serial.println(REG_READ(GPIO_FUNC15_OUT_SEL_CFG_REG));

  Serial.print("Valor de GPIO_FUNC18_OUT_SEL_CFG_REG: ");
  Serial.println(REG_READ(GPIO_FUNC18_OUT_SEL_CFG_REG));

  // 2 Se omite,no es salida open drain

  // 3a)
  // REG_SET_FIELD(PERIPHS_IO_MUX_MTDO_U, MCU_SEL, 2);
  REG_SET_FIELD(IO_MUX_GPIO15_REG, MCU_SEL, 2);
  // 3b)
  // REG_SET_FIELD(IO_MUX_GPIO15_REG,FUN_DRV,1);
  //  3c) Se omite

  // REG_SET_FIELD(GPIO_ENABLE_REG, GPIO_ENABLE_DATA, BIT15 + BIT18);

  Serial.print("Valor de GPIO_FUNC18_OUT_SEL_CFG_REG: ");
  Serial.println(REG_READ(GPIO_FUNC18_OUT_SEL_CFG_REG));
}

void enable_DPORT_CLK()
{
  // HABILITACIÓN DEL PERIFÉRICO MCPWM MEDIANTE DPORT REGISTERS
  Serial.print(" REGISTRO HABILITACION MCPWM: ");
  Serial.println(DPORT_REG_READ(DPORT_PERIP_CLK_EN_REG));
  DPORT_REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_PWM0_CLK_EN);
  Serial.print(" REGISTRO HABILITACION MCPWM: ");
  Serial.println(DPORT_REG_READ(DPORT_PERIP_CLK_EN_REG));

// No entiendo muy bien porque hay que restear y limpiar el reset para que funcione
// Set each bit to reset the corresponding module. Clear the bit to release the corresponding module. For the list of
// modules, please refer to register 5.17.
  DPORT_REG_SET_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
  DPORT_REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
  DPORT_REG_READ(DPORT_AHBLITE_MPU_TABLE_PWM0_REG);
  
  DPORT_REG_WRITE(DPORT_AHBLITE_MPU_TABLE_PWM0_REG, 1);
}

void enable_PCOUNTER_CLK()
{
  DPORT_REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_PCNT_CLK_EN);

  DPORT_REG_SET_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PCNT_RST);
  DPORT_REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_PCNT_RST);

// MPU/MMU registers are used for MPU/MMU configuration and operation control. They are listed in Section 5.4,
// categorized as ”MPU/MMU registers”. For a detailed description of these registers, please refer to Chapter Memory
// Management and Protection Units (MMU, MPU).
  DPORT_REG_WRITE(DPORT_AHBLITE_MPU_TABLE_PCNT_REG, 1);
}

void set_PCOUNTER_GPIO(){
  
}