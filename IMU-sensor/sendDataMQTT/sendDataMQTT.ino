#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"

// Pines del IMU
#define SENSOR_SDA  6
#define SENSOR_SCL  7

SensorQMI8658 qmi;

IMUdata acc;
IMUdata gyr;

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
}

void loop() {
  if (qmi.getDataReady()) {
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
      Serial.print("ACCEL.x:"); Serial.print(acc.x); Serial.print(",");
      Serial.print("ACCEL.y:"); Serial.print(acc.y); Serial.print(",");
      Serial.print("ACCEL.z:"); Serial.print(acc.z); Serial.println();
    }

    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {  
      Serial.print("GYRO.x:"); Serial.print(gyr.x); Serial.print(",");
      Serial.print("GYRO.y:"); Serial.print(gyr.y); Serial.print(",");
      Serial.print("GYRO.z:"); Serial.print(gyr.z); Serial.println();
    }

    Serial.print("Temperature:");
    Serial.print(qmi.getTemperature_C());
    Serial.println(" degrees C");
  }
    
  delay(1000);
}
