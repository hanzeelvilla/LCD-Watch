#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"

#define SENSOR_SDA 6
#define SENSOR_SCL 7

SensorQMI8658 qmi;

IMUdata acc;
IMUdata gyr;

void setup() {
  Serial.begin(115200);

  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println("Failed to find QMI8658 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }
  else {
    Serial.println("QMI8658 initialized");
  }

  // Get chip id
  Serial.print("Device ID:");
  Serial.println(qmi.getChipID(), HEX);

  uint16_t gX_gain = 0,  gY_gain = 0,  gZ_gain = 0;

  // Call internal calibration to calibrate the sensor
  bool result = false;

  while (!result) {

    // Calibrate only once, do not obtain calibration value
    // result = qmi.calibration();

    // Get the calibration value after calibration
    result = qmi.calibration(&gX_gain, &gY_gain, &gZ_gain);
    if (result) {
      Serial.println("All calibrations are completed");
      break;
    }
    int i = 5;
    while (i--) {
      Serial.printf("Calibration failed, please leave the sensor alone and keep quiet! , Will try again in %d seconds", i);
    }
  }

  // Print gain
  Serial.println("Calibration data ->");
  Serial.print("Gyro-X gain : "); Serial.println(gX_gain);
  Serial.print("Gyro-Y gain : "); Serial.println(gY_gain);
  Serial.print("Gyro-Z gain : "); Serial.println(gZ_gain);

  // The example only provides a method for writing calibration values
  result = qmi.writeCalibration(gX_gain, gY_gain, gZ_gain);
  if (result) {
    Serial.println("Write calibrations successfully");
  } else {
    Serial.println("Write calibrations failed!");
  }

  qmi.configAccelerometer(
    /*
    * ACC_RANGE_2G
    * ACC_RANGE_4G
    * ACC_RANGE_8G
    * ACC_RANGE_16G
    */
    SensorQMI8658::ACC_RANGE_4G,
    /*
    * ACC_ODR_1000H
    * ACC_ODR_500Hz
    * ACC_ODR_250Hz
    * ACC_ODR_125Hz
    * ACC_ODR_62_5Hz
    * ACC_ODR_31_25Hz
    * ACC_ODR_LOWPOWER_128Hz
    * ACC_ODR_LOWPOWER_21Hz
    * ACC_ODR_LOWPOWER_11Hz
    * ACC_ODR_LOWPOWER_3H
    */
    SensorQMI8658::ACC_ODR_1000Hz,
    /*
    *  LPF_MODE_0     //2.66% of ODR
    *  LPF_MODE_1     //3.63% of ODR
    *  LPF_MODE_2     //5.39% of ODR
    *  LPF_MODE_3     //13.37% of ODR
    *  LPF_OFF        // OFF Low-Pass Fitter
    */
    SensorQMI8658::LPF_MODE_0);

  qmi.configGyroscope(
    /*
    * GYR_RANGE_16DPS
    * GYR_RANGE_32DPS
    * GYR_RANGE_64DPS
    * GYR_RANGE_128DPS
    * GYR_RANGE_256DPS
    * GYR_RANGE_512DPS
    * GYR_RANGE_1024DPS
    */
    SensorQMI8658::GYR_RANGE_64DPS,
    /*
    * GYR_ODR_7174_4Hz
    * GYR_ODR_3587_2Hz
    * GYR_ODR_1793_6Hz
    * GYR_ODR_896_8Hz
    * GYR_ODR_448_4Hz
    * GYR_ODR_224_2Hz
    * GYR_ODR_112_1Hz
    * GYR_ODR_56_05Hz
    * GYR_ODR_28_025H
    */
    SensorQMI8658::GYR_ODR_896_8Hz,
    /*
    *  LPF_MODE_0     //2.66% of ODR
    *  LPF_MODE_1     //3.63% of ODR
    *  LPF_MODE_2     //5.39% of ODR
    *  LPF_MODE_3     //13.37% of ODR
    *  LPF_OFF        // OFF Low-Pass Fitter
    */
    SensorQMI8658::LPF_MODE_3
  );

  /*
    * If both the accelerometer and gyroscope sensors are turned on at the same time,
    * the output frequency will be based on the gyroscope output frequency.
    * The example configuration is 896.8HZ output frequency,
    * so the acceleration output frequency is also limited to 896.8HZ
  */

  qmi.enableGyroscope();
  qmi.enableAccelerometer();

  // Print register configuration information
  qmi.dumpCtrlRegister();

  Serial.println("Read data now...");
}

void loop() {
  if (qmi.getDataReady()) {
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
      Serial.print("ACCEL.x:");
      Serial.print(acc.x);
      Serial.print(",ACCEL.y:");
      Serial.print(acc.y);
      Serial.print(",ACCEL.z:");
      Serial.print(acc.z);
      Serial.println("");
    }
    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
      Serial.print("GYRO.x:");
      Serial.print(gyr.x);
      Serial.print(",GYRO.y:");
      Serial.print(gyr.y);
      Serial.print(",GYRO.z:");
      Serial.print(gyr.z);
      Serial.println("");
    }
  }
  delay(1000);
}
