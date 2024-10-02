#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"

#define SENSOR_SDA  6
#define SENSOR_SCL  7

SensorQMI8658 qmi;

IMUdata acc;
IMUdata gyr;

// Variables para el sesgo del giroscopio
float gyroBiasX = 0;
float gyroBiasY = 0;
float gyroBiasZ = 0;

void setup() {
  Serial.begin(115200);

  while (!Serial);

  delay(3000);

  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println("Failed to find QMI8658 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }

  // Calibración automática interna (mantén el sensor quieto durante la calibración)
  uint16_t gX_gain = 0, gY_gain = 0, gZ_gain = 0;

  if (!qmi.calibration(&gX_gain, &gY_gain, &gZ_gain)) {
    Serial.println("Calibration failed");
  } 
  else {
    Serial.println("All calibrations completed");
  }

  // Guardar los valores de calibración
  qmi.writeCalibration(gX_gain, gY_gain, gZ_gain);

  // Configuración del giroscopio y acelerómetro
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
  qmi.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896_8Hz, SensorQMI8658::LPF_MODE_3);

  qmi.enableGyroscope();
  qmi.enableAccelerometer();

  // Hacer la calibración manual para ajustar el sesgo
  calibrateGyroBias();
}

void loop() {
  if (qmi.getDataReady()) {
    // Lectura del acelerómetro
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
      Serial.print("ACCEL.x:");
      Serial.print(acc.x);
      Serial.print(",ACCEL.y:");
      Serial.print(acc.y);
      Serial.print(",ACCEL.z:");
      Serial.print(acc.z);
      Serial.println("");
    } 
    else {
      Serial.println("No jala");
    }
    
    // Lectura del giroscopio (restando el sesgo calculado)
    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
      float gyroX = gyr.x - gyroBiasX;
      float gyroY = gyr.y - gyroBiasY;
      float gyroZ = gyr.z - gyroBiasZ;

      Serial.print("GYRO.x:");
      Serial.print(gyroX);
      Serial.print(",GYRO.y:");
      Serial.print(gyroY);
      Serial.print(",GYRO.z:");
      Serial.print(gyroZ);
      Serial.println("");
    } 
    else {
      Serial.println("no jala el gyro");
    }
  }
    delay(1000);
}

// Función para calibrar manualmente el sesgo del giroscopio
void calibrateGyroBias() {
  float sumX = 0, sumY = 0, sumZ = 0;
  int samples = 100;  // Número de muestras para promediar el sesgo

  Serial.println("Calibrating gyroscope bias...");

  for (int i = 0; i < samples; i++) {
    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
      sumX += gyr.x;
      sumY += gyr.y;
      sumZ += gyr.z;
    }    
    delay(10);  // Pequeño retardo entre las muestras
  }

  // Calcular el promedio
  gyroBiasX = sumX / samples;
  gyroBiasY = sumY / samples;
  gyroBiasZ = sumZ / samples;

  Serial.println("Gyro bias calibration complete:");
  Serial.print("Bias X: "); Serial.println(gyroBiasX);
  Serial.print("Bias Y: "); Serial.println(gyroBiasY);
  Serial.print("Bias Z: "); Serial.println(gyroBiasZ);
}