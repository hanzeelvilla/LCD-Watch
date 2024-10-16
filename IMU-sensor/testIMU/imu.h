#ifdef IMU
#define IMU

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#include "Preferences.h"

// pines del IMU
#define SENSOR_SDA  6
#define SENSOR_SCL  7

// nombre del namespace
#define IMU_CONF_NAME "imuConf"

class Imu {
  public:
    void init();
    void setup(
      bool calibrate = false, // no calibrar por dafault
      AccelRange accelRange = SensorQMI8658::ACC_RANGE_4G, 
      AccelODR accelOdr = SensorQMI8658::ACC_ODR_1000Hz, 
      LpfMode accelLpfOdr = SensorQMI8658::LPF_MODE_0,
      GyroRange gyroRange = SensorQMI8658::GYR_RANGE_64DPS, 
      GyroODR gyroOdr = SensorQMI8658::GYR_ODR_896_8Hz, 
      LpfMode gryoLpfOdr = SensorQMI8658::LPF_MODE_3 
    )
    IMUdata getGyro();
    IMUdata getAcce();
  private:
    SensorQMI8658 qmi;
    IMUdata acc;
    IMUdata gyr;
    // variables para el sesgo del giroscopio
    float gyroBiasX = 0;
    float gyroBiasY = 0;
    float gyroBiasZ = 0;
    // calibración automática interna (mantén el sensor quieto durante la calibración)
    uint16_t gX_gain = 0, 
    uint_16_t gY_gain = 0, 
    uint_16_t gZ_gain = 0;
}

#endif 