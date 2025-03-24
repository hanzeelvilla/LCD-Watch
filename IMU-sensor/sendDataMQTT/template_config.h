// cambia el nombre de este archivo a config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <Wire.h>
#include <WiFiMulti.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <NTPClient.h>

#define SSID1 "TP-Link_AP_4822"
#define PSWD1 "93149075"
#define SSID2 "your_SSID12"
#define PSWD2 "your_PSWD2"

#define BROKER "test.mosquitto.org"
#define PORT 1883
#define TX_TOPIC "/TX_TOPIC"
#define MQTT_CLIENT "Client_test"

#define GMT_6 -21600 

#endif