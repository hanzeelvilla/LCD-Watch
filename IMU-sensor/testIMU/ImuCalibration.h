#ifndef IMU_CALIBRATION
#define IMU_CALIBRATION

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#include "Preferences.h"

// pines del IMU
#define SENSOR_SDA  6
#define SENSOR_SCL  7
#define IMU_CONF_NAME "imuConf"

// Declaraciones de variables globales
extern SensorQMI8658 qmi;
extern IMUdata acc;
extern IMUdata gyr;
extern float gyroBiasX;
extern float gyroBiasY;
extern float gyroBiasZ;
extern Preferences preferences;

void setupIMU();
void calibrateGyroBias();
void readIMU();

#endif