#define LED_BUILTIN 2
#define PWM_A 15
#define PWM_B 18
#define DIS 22


// Configuración modo AP
const char *ssid_AP = "ESP32-RI";
const char *password_AP = "casa2030";

IPAddress ip_AP(192, 168, 1, 222);
IPAddress gateway_AP(192, 168, 1, 1);
IPAddress subnet_AP(255, 255, 255, 0);

// Configuración moodo STA
//const char *ssid_STA = "NETGEAR56";
//const char *password_STA = "casa2020";
const char *ssid_STA = "1B";
const char *password_STA = "Caseriumadorro_N36";
const char* hostname = "ESP32-RInv";

IPAddress ip_STA(192, 168, 0, 222);
IPAddress gateway_STA(192, 168, 0, 1);
IPAddress subnet_STA(255, 255, 255, 0);