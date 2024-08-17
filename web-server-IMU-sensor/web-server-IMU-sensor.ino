#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "SensorQMI8658.hpp"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "LittleFS.h"

#define SENSOR_SDA  6
#define SENSOR_SCL  7

SensorQMI8658 qmi;
IMUdata acc, gyr;

WiFiMulti wifiMulti;
AsyncWebServer server(80);  // Crear un servidor en el puerto 80
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("WebSocket client connected");
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.println("WebSocket client disconnected");
    }
}

void sendSensorData() {
    if (qmi.getDataReady()) {
        if (qmi.getAccelerometer(acc.x, acc.y, acc.z) && qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
            StaticJsonDocument<200> jsonDoc;
            jsonDoc["accel_x"] = acc.x;
            jsonDoc["accel_y"] = acc.y;
            jsonDoc["accel_z"] = acc.z;
            jsonDoc["gyro_x"] = gyr.x;
            jsonDoc["gyro_y"] = gyr.y;
            jsonDoc["gyro_z"] = gyr.z;
            jsonDoc["time"] = qmi.getTimestamp();
            jsonDoc["temp"] = qmi.getTemperature_C();
            
            String jsonString;
            serializeJson(jsonDoc, jsonString);
            ws.textAll(jsonString);
        }
    }
}

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

  if(!LittleFS.begin(true)){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
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

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  Serial.println("Read data now...");

  // Definir una ruta para obtener los datos del IMU
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {

    // Enviar la respuesta con los datos del sensor
    request->send(LittleFS, "/index.html", "text/html");
  });


  server.onNotFound(notFound);

  // Iniciar el servidor
  server.begin();
}

void loop() {
  hayWifi();
  sendSensorData(); // Enviar datos en tiempo real
  ws.cleanupClients(); // Limpia clientes inactivos
  delay(1000);
}
