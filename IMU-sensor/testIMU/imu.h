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

/* NO ME GUSTA QUE TENGO QUE VOLVER A DECLARAR LOS ENUM PERO BUENO */

enum AccelRange {
  ACC_RANGE_2G,
  ACC_RANGE_4G,
  ACC_RANGE_8G,
  ACC_RANGE_16G
};

enum AccelODR {
  ACC_ODR_1000Hz = 3,
  ACC_ODR_500Hz,
  ACC_ODR_250Hz,
  ACC_ODR_125Hz,
  ACC_ODR_62_5Hz,
  ACC_ODR_31_25Hz,
  ACC_ODR_LOWPOWER_128Hz  = 12,   
  ACC_ODR_LOWPOWER_21Hz,          
  ACC_ODR_LOWPOWER_11Hz,          
  ACC_ODR_LOWPOWER_3Hz            
};

 enum LpfMode {
  LPF_MODE_0,     
  LPF_MODE_1,     
  LPF_MODE_2,     
  LPF_MODE_3,     
  LPF_OFF,        
};

enum GyroRange {
  GYR_RANGE_16DPS,
  GYR_RANGE_32DPS,
  GYR_RANGE_64DPS,
  GYR_RANGE_128DPS,
  GYR_RANGE_256DPS,
  GYR_RANGE_512DPS,
  GYR_RANGE_1024DPS,
};

 enum GyroODR {
  GYR_ODR_7174_4Hz,
  GYR_ODR_3587_2Hz,
  GYR_ODR_1793_6Hz,
  GYR_ODR_896_8Hz,
  GYR_ODR_448_4Hz,
  GYR_ODR_224_2Hz,
  GYR_ODR_112_1Hz,
  GYR_ODR_56_05Hz,
  GYR_ODR_28_025Hz
};

class Imu {
  public:
    void init();
    void setup(
      bool calibrate = false, // No calibrar por default
      AccelRange accelRange = ACC_RANGE_4G, 
      AccelODR accelOdr = ACC_ODR_1000Hz, 
      LpfMode accelLpfOdr = LPF_MODE_0,
      GyroRange gyroRange = GYR_RANGE_64DPS, 
      GyroODR gyroOdr = GYR_ODR_896_8Hz, 
      LpfMode gyroLpfOdr = LPF_MODE_3
    );
    IMUdata getGyro();
    IMUdata getAcce();

  private:
    SensorQMI8658 qmi; // Variable para manejar el IMU
    Preferences preferences; // Para guardar los valores en la ESP32
    IMUdata acc;
    IMUdata gyr;
    // Variables para el sesgo del giroscopio
    float gyroBiasX = 0;
    float gyroBiasY = 0;
    float gyroBiasZ = 0;
    // Calibración automática interna (mantener el sensor quieto durante la calibración)
    uint16_t gX_gain = 0; 
    uint16_t gY_gain = 0; 
    uint16_t gZ_gain = 0;
};

#endif