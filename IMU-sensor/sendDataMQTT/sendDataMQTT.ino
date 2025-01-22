#include <Wire.h>
#include <WiFiMulti.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "config.h"

// Pines del IMU
#define SENSOR_SDA  6
#define SENSOR_SCL  7

SensorQMI8658 qmi;

IMUdata acc;
IMUdata gyr;

WiFiMulti wifiMulti;
WiFiClient espClient;
PubSubClient client(espClient);

JsonDocument doc;

//FUNCIONES
bool wifiConnected();
void initWifi();
void reconnect();

void setup() {
  Serial.begin(115200);

  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println(" --------> Failed to find QMI8658 - check your wiring! <-------- ");
      while (1) {
        delay(1000);
      }
  } 
  else
    Serial.println("QMI8658 Initialized");

  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0); // Buscar como pasar esta configuracion por parametros
  qmi.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896_8Hz, SensorQMI8658::LPF_MODE_3); // Buscar como pasar esta configuracion por parametros
  qmi.enableGyroscope();
  qmi.enableAccelerometer();
  Serial.println("IMU configurado");

  initWifi();
  client.setServer(BROKER, PORT);
}

void loop() {
  if (qmi.getDataReady()) {
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
      Serial.print("ACCEL.x:"); Serial.print(acc.x); Serial.print(",");
      Serial.print("ACCEL.y:"); Serial.print(acc.y); Serial.print(",");
      Serial.print("ACCEL.z:"); Serial.print(acc.z); Serial.println();

      doc["acc"]["x"] = acc.x;
      doc["acc"]["y"] = acc.y;
      doc["acc"]["z"] = acc.z;
    }

    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {  
      Serial.print("GYRO.x:"); Serial.print(gyr.x); Serial.print(",");
      Serial.print("GYRO.y:"); Serial.print(gyr.y); Serial.print(",");
      Serial.print("GYRO.z:"); Serial.print(gyr.z); Serial.println();

      doc["gyr"]["x"] = acc.x;
      doc["gyr"]["y"] = acc.y;
      doc["gyr"]["z"] = acc.z;
    }

    Serial.print("Temperature:");
    Serial.print(qmi.getTemperature_C());
    Serial.println(" degrees C");
    Serial.println(qmi.getTimestamp());

    doc["temp"] = qmi.getTemperature_C();
    doc["time"] = qmi.getTimestamp();
  }

  if (wifiConnected()) {
    char jsonBuffer[200];
    serializeJson(doc, jsonBuffer);

    if (client.publish(TX_TOPIC, jsonBuffer)) {
        Serial.println("Publishing message...");
        Serial.println(jsonBuffer);
      }
      else 
        Serial.println("Failed to publish message.");
  }
  else
    Serial.println("Sin internet");
  
  reconnect();
  delay(1000);
}

bool wifiConnected() {
  return wifiMulti.run() == WL_CONNECTED;
}

void initWifi() {
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(SSID1, PSWD1);
  wifiMulti.addAP(SSID2, PSWD2); 

  Serial.println("Connecting to WiFi...");

  while (!wifiConnected()) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected");
  Serial.println("SSID: " + WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      if (client.connect(MQTT_CLIENT)) { // change the client ID to something unique
        Serial.print(" Connected to: ");
        Serial.print("test.mosquitto.org");
        /*
        client.subscribe(RX_TOPIC);
        Serial.print(" Subscribed to: ");
        Serial.println(RX_TOPIC);
        */
      } 
      else {
        Serial.print(" failed, rc= ");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }

  client.loop();
}
