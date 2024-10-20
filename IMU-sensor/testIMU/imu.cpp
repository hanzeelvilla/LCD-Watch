#include "imu.h"

void Imu::init() {
  while (!Serial);
  delay(3000);

  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println("Failed to find QMI8658 - check your wiring!");
      while (1) {
        delay(1000);
      }
  } 
  else
    Serial.println("QMI8658 Initialized");
}

void Imu::setup(
    bool calibrate,
    AccelRange accelRange, 
    AccelODR accelOdr, 
    LpfMode accelLpfOdr,
    GyroRange gyroRange, 
    GyroODR gyroOdr, 
    LpfMode gyroLpfOdr
  ){

    if(calibrate) {
      // calibración automática interna (mantén el sensor quieto durante la calibración)
      uint16_t gX_gain = 0, gY_gain = 0, gZ_gain = 0;

      if (!qmi.calibration(&gX_gain, &gY_gain, &gZ_gain))
        Serial.println("Calibration failed");
      else {
        Serial.println("All calibrations completed");
        Serial.print("X gain: "); Serial.println(gX_gain);
        Serial.print("Y gain: "); Serial.println(gY_gain);
        Serial.print("Z gain: "); Serial.println(gZ_gain);
      }

      // guardar los valores de calibración TEMPORALMENTE
      qmi.writeCalibration(gX_gain, gY_gain, gZ_gain);

      // configuración del giroscopio y acelerómetro
      qmi.configAccelerometer(accelRange, accelOdr, accelLpfOdr);
      qmi.configGyroscope(gyroRange, gyroOdr, gyroLpfOdr);
      qmi.enableGyroscope();
      qmi.enableAccelerometer();
    }
}

void Imu::calibrateGyroBias() {
  float sumX = 0, sumY = 0, sumZ = 0;
  int samples = 100;
  
  Serial.println("Calibrating gyroscope bias...");

  for (int i = 0; i < samples; i++) {
    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
      sumX += gyr.x;
      sumY += gyr.y;
      sumZ += gyr.z;
    }
    delay(10); // pequeño retardo entre las muestras
  }

  // calcular el promedio
  gyroBiasX = sumX / samples;
  gyroBiasY = sumY / samples;
  gyroBiasZ = sumZ / samples;

  Serial.println("Gyro bias calibration complete:");
  Serial.print("Bias X: "); Serial.println(gyroBiasX);
  Serial.print("Bias Y: "); Serial.println(gyroBiasY);
  Serial.print("Bias Z: "); Serial.println(gyroBiasZ);
}

/*
  // calibración automática interna (mantén el sensor quieto durante la calibración)
  uint16_t gX_gain = 0, gY_gain = 0, gZ_gain = 0;

  if (!qmi.calibration(&gX_gain, &gY_gain, &gZ_gain))
    Serial.println("Calibration failed");
  else {
    Serial.println("All calibrations completed");
    Serial.print("X gain: "); Serial.println(gX_gain);
    Serial.print("Y gain: "); Serial.println(gY_gain);
    Serial.print("Z gain: "); Serial.println(gZ_gain);
  }

  // guardar los valores de calibración TEMPORALMENTE
  qmi.writeCalibration(gX_gain, gY_gain, gZ_gain);

  // configuración del giroscopio y acelerómetro
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
  qmi.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896_8Hz, SensorQMI8658::LPF_MODE_3);
  qmi.enableGyroscope();
  qmi.enableAccelerometer();

  // calibración manual
  calibrateGyroBias();

  Serial.println("Saving new configuration");

  // guardar en la ESP la configuración
  preferences.begin(IMU_CONF_NAME, false); //false -> read/write
  preferences.clear();
  Serial.println("Previous configuration cleared");

    LLAVE, VALOR
    gX_gain: int,
    gY_gain: int,
    gZ_gain: int,
    gyroBiasX: float,
    gyroBiasY: float,
    gyroBiasZ: float,
    
  preferences.putInt("gX_gain", gX_gain);
  preferences.putInt("gY_gain", gY_gain);
  preferences.putInt("gZ_gain", gZ_gain);
  preferences.putFloat("gyroBiasX", gyroBiasX);
  preferences.putFloat("gyroBiasY", gyroBiasY);
  preferences.putFloat("gyroBiasZ", gyroBiasZ);

  gX_gain = preferences.getInt("gX_gain", -1);
  gY_gain = preferences.getInt("gY_gain", -1);
  gZ_gain = preferences.getInt("gZ_gain", -1);
  gyroBiasX = preferences.getFloat("gyroBiasX", NAN);
  gyroBiasY = preferences.getFloat("gyroBiasY", NAN);
  gyroBiasZ = preferences.getFloat("gyroBiasZ", NAN);

  preferences.end();

  if (gX_gain != -1 && gY_gain != -1 && gZ_gain != -1 && gyroBiasX != NAN && gyroBiasY != NAN && gyroBiasZ != NAN)
    Serial.println("Configuration saved");
  else
    Serial.println("Error saving configuration");
}

// función para calibrar manualmente el sesgo del giroscopio
void calibrateGyroBias() {
  float sumX = 0, sumY = 0, sumZ = 0;
  int samples = 100; // num de muestras para promediar el sesgo

  Serial.println("Calibrating gyroscope bias...");

  for (int i = 0; i < samples; i++) {
    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
      sumX += gyr.x;
      sumY += gyr.y;
      sumZ += gyr.z;
    }
    delay(10); // pequeño retardo entre las muestras
  }

  // calcular el promedio
  gyroBiasX = sumX / samples;
  gyroBiasY = sumY / samples;
  gyroBiasZ = sumZ / samples;

  Serial.println("Gyro bias calibration complete:");
  Serial.print("Bias X: "); Serial.println(gyroBiasX);
  Serial.print("Bias Y: "); Serial.println(gyroBiasY);
  Serial.print("Bias Z: "); Serial.println(gyroBiasZ);
}

void readIMU() {
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
*/