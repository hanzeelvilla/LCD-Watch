#include <WiFi.h>
#include <WiFiMulti.h>
#include "config.h"

WiFiMulti wifiMulti;

// Tiempo para conectarse a un access point
const uint32_t connectTimeoutMs = 5000;

void hayWifi() {
  // Verificar que siga conectado a internet, con las redes de la lista
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not connected!");
  }
}

void setup() {
  Serial.begin(115200);

  wifiMulti.addAP(SSID1, PSWD1);
  wifiMulti.addAP(SSID2, PSWD2);

  Serial.println("Connecting Wifi...");
  Serial.println(WiFi.macAddress());
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  hayWifi();
  delay(1000);
}
