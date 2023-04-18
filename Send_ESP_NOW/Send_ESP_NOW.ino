#include <esp_now.h>
#include <WiFi.h>

#define LED_BUILTIN 2

uint8_t broadcastAddress[] = {0x58,0xBF,0x25,0x81,0x3E,0x98};
// mac metalico 0x40,0x22,0xD8,0x5F,0xF7,0xFC

// Definir a estrutura para o pacote de dados
typedef struct struct_message {
  char data[32];
  int counter;
  long int millis;
  bool led;
} struct_message;

// Inicializar a estrutura do pacote de dados
struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus) { 
  Serial.println(sendStatus == 0 ? "Entregue":"Não entregue");
}//end OnDataSent

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  WiFi.mode(WIFI_STA);
  Serial.print("WiFi.macAddress: ");
  Serial.println(WiFi.macAddress());
  /*
      Available ESP32 RF power parameters:
      WIFI_POWER_19_5dBm    // 19.5dBm (For 19.5dBm of output, highest. Supply current ~150mA)
      WIFI_POWER_19dBm      // 19dBm
      WIFI_POWER_18_5dBm    // 18.5dBm
      WIFI_POWER_17dBm      // 17dBm
      WIFI_POWER_15dBm      // 15dBm
      WIFI_POWER_13dBm      // 13dBm
      WIFI_POWER_11dBm      // 11dBm
      WIFI_POWER_8_5dBm     //  8dBm
      WIFI_POWER_7dBm       //  7dBm
      WIFI_POWER_5dBm       //  5dBm
      WIFI_POWER_2dBm       //  2dBm
      WIFI_POWER_MINUS_1dBm // -1dBm( For -1dBm of output, lowest. Supply current ~120mA)
      Available ESP8266 RF power parameters:
      0    (for lowest RF power output, supply current ~ 70mA
      20.5 (for highest RF power output, supply current ~ 80mA
  */ 
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  for (int ii = 0; ii < 6; ++ii )
  {
    peerInfo.peer_addr[ii] = (uint8_t) broadcastAddress[ii];
  }
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
}//end setup

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  // Construir o pacote de dados
  strcpy(myData.data, "Hello world");
  myData.counter++;
  myData.millis = millis();
  myData.led = !myData.led;

  // Enviar o pacote de dados para o receptor
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  //Serial.println(result == ESP_OK?"PKG_OK":"PKG_ERROR");
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
}//end loop