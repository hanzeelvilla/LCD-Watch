#include <Arduino.h>
#include <Wire.h>
#include "SensorQMI8658.hpp"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPAsyncWebServer.h>
#include "config.h"

#define SENSOR_SDA  6
#define SENSOR_SCL  7

SensorQMI8658 qmi;
IMUdata acc, gyr;

WiFiMulti wifiMulti;
AsyncWebServer server(80);  // Crear un servidor en el puerto 80

// Tiempo para conectarse a un access point
const uint32_t connectTimeoutMs = 10000;

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

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  
  // Conexión WiFi
  wifiMulti.addAP(SSID1, PSWD1);
  wifiMulti.addAP(SSID2, PSWD2);
  
  Serial.println("Connecting Wifi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // Inicializar sensor IMU
  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println("Failed to find QMI8658 - check your wiring!");
    while (1) { delay(1000); }
  }

  Serial.print("Device ID:");
  Serial.println(qmi.getChipID(), HEX);

  // Configurar el sensor
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0, false);
  qmi.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896_8Hz, SensorQMI8658::LPF_MODE_3, false);

  qmi.enableGyroscope();
  qmi.enableAccelerometer();

  Serial.println("Read data now...");

  // Definir una ruta para obtener los datos del IMU
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String data = "";
    
    if (qmi.getDataReady()) {
      if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
        data += "ACCEL: " + String(acc.x) + ", " + String(acc.y) + ", " + String(acc.z) + "\n";
      }
        
      if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
        data += "GYRO: " + String(gyr.x) + ", " + String(gyr.y) + ", " + String(gyr.z) + "\n";
      }
      
      data += "Timestamp: " + String(qmi.getTimestamp()) + ", Temperature: " + String(qmi.getTemperature_C()) + " *C\n";
    }

    // Enviar la respuesta con los datos del sensor
    request->send(200, "text/plain", data);
  });

  server.on("/accel", HTTP_GET, [](AsyncWebServerRequest *request) {
    String data = "";

    if (qmi.getDataReady()) {
      if(qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
        data += String(acc.x) + ", " + String(acc.y) + ", " + String(acc.z);
      }
    }

    request->send(200, "text/plain", data);
  });

  server.on("/gyro", HTTP_GET, [](AsyncWebServerRequest *request) {
    String data = "";

    if (qmi.getDataReady()) {
      if(qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
        data += String(gyr.x) + ", " + String(gyr.y) + ", " + String(gyr.z);
      }
    }

    request->send(200, "text/plain", data);
  });

  server.onNotFound(notFound);

  // Iniciar el servidor
  server.begin();
}

void loop() {
  hayWifi();
  delay(1000);
}
