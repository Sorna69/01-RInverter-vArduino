#define LED_BUILTIN 2
#define DIS 22


// Configuración modo AP
const char *ssid_AP = "ESP32-AP01";
const char *password_AP = "Manele-00";
const char* hostname = "ESP32-RInv";

IPAddress ip_AP(192, 168, 1, 10);
IPAddress gateway_AP(192, 168, 1, 1);
IPAddress subnet_AP(255, 255, 255, 0);

// Configuración moodo STA
const char *ssid_STA = "APET-2.4";
const char *password_STA = "power_transistor";

IPAddress ip_STA(192, 168, 0, 222);
IPAddress gateway_STA(192, 168, 0, 1);
IPAddress subnet_STA(255, 255, 255, 0);