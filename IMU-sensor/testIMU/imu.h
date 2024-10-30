#ifndef IMU_H // Cambio de 'ifdef' a 'ifndef' para proteger la definición
#define IMU_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#include "Preferences.h"

// Pines del IMU
#define SENSOR_SDA  6
#define SENSOR_SCL  7

// Nombre del namespace
#define IMU_CONF_NAME "imuConf"

class Imu {
  public:
    void init();
    void setup(bool calibrate = false); // No calibrar por default);
    IMUdata getGyr();
    IMUdata getAcc();

  private:
    SensorQMI8658 qmi; // Variable para manejar el IMU
    Preferences preferences; // Para guardar los valores en la ESP32
    IMUdata acc;
    IMUdata gyr;
    IMUdata error = {NAN, NAN, NAN};
  };

#endif