#include <Config.h>
#include <FirebaseClient.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "SensorQMI8658.hpp"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "LittleFS.h"
#include <WiFiClientSecure.h>

#define SENSOR_SDA  6
#define SENSOR_SCL  7
#define API_KEY "AIzaSyC6FzLGUEaX51nrIdpHvBBMDYDS0RFLtVs"
#define DATABASE_URL "https://abuelometro-database-default-rtdb.firebaseio.com/"

WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
NoAuth noAuth;

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

void sendDataToDB() {
  if (qmi.getDataReady()) {
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z) && qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {

      char jsonBuffer[512];

      snprintf(jsonBuffer, sizeof(jsonBuffer),
        "{\"sensorData\": {"
          "\"gyroscope\": {\"x\":%.5f, \"y\":%.5f, \"z\":%.5f},"
          "\"accelerometer\": {\"x\":%.5f, \"y\":%.5f, \"z\":%.5f},"
          "\"temperature\":%.2f,"
          "\"timestamp\":%lu"
        "}}",
        gyr.x, gyr.y, gyr.z,
        acc.x, acc.y, acc.z,
        qmi.getTemperature_C(),
        qmi.getTimestamp());

      Serial.print("Set JSON... ");
      bool status = Database.set<object_t>(client, "/data", object_t(jsonBuffer));
      if (status)
        Serial.println("ok");
      else
        printError(client.lastError().code(), client.lastError().message());
    }
  } else {
    Serial.println("Failed getting data");
  }
};

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

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
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

  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

  ssl.setInsecure();

  initializeApp(client, app, getAuth(noAuth));

  // Binding the authentication handler with your Database class object.
  app.getApp<RealtimeDatabase>(Database);

  // Set your database URL
  Database.url(DATABASE_URL);

  // In sync functions, we have to set the operating result for the client that works with the function.
  client.setAsyncResult(result);

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
  sendDataToDB();
  delay(1000);
}
