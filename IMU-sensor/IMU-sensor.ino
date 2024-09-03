#include <Arduino.h>
#include <Wire.h>
#include "SensorQMI8658.hpp"

#define SENSOR_SDA  6
#define SENSOR_SCL  7

SensorQMI8658 qmi;
IMUdata acc, gyr;

void setup() {
    Serial.begin(115200);

    if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
        Serial.println("Failed to find QMI8658 - check your wiring!");
        while (1) { delay(1000); }
    }

    Serial.print("Device ID:");
    Serial.println(qmi.getChipID(), HEX);

    qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0, false);
    qmi.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896_8Hz, SensorQMI8658::LPF_MODE_3, false);

    qmi.enableGyroscope();
    qmi.enableAccelerometer();

    // Print register configuration information
    //qmi.dumpCtrlRegister();

    Serial.println("Read data now...");
}

void loop() {
    if (qmi.getDataReady()) {
        
        if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
            Serial.printf("{ACCEL: %f, %f, %f}\n", acc.x, acc.y, acc.z);
        }
        
        if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
            Serial.printf("{GYRO: %f, %f, %f}\n", gyr.x, gyr.y, gyr.z);
        }
        Serial.printf("Timestamp: %lu, Temperature: %.2f *C\n", qmi.getTimestamp(), qmi.getTemperature_C());
    }
    delay(1000);
}
