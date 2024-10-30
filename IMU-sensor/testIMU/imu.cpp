#include "imu.h"

void Imu::init() {
  while (!Serial);
  delay(3000);

  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println(" --------> Failed to find QMI8658 - check your wiring! <-------- ");
      while (1) {
        delay(1000);
      }
  } 
  else
    Serial.println("QMI8658 Initialized");
}

void Imu::setup(bool calibrate){
  if(calibrate) {
    // calibración automática interna (mantén el sensor quieto durante la calibración)
    Serial.println("Keep the sensor still, starting calibration in 3 seconds");
    delay(3000); // cambiar delay por millis despues

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

    // guardar los valores de la calibracion en la MEMORIA FLASH
    preferences.begin(IMU_CONF_NAME, false); //false -> read/write
    preferences.clear();
    Serial.println("Previous configuration cleared");

    /*
    LLAVE, VALOR
      gX_gain: int,
      gY_gain: int,
      gZ_gain: int,
    */

    preferences.putInt("gX_gain", gX_gain);
    preferences.putInt("gY_gain", gY_gain);
    preferences.putInt("gZ_gain", gZ_gain);

    gX_gain = preferences.getInt("gX_gain", -1);
    gY_gain = preferences.getInt("gY_gain", -1);
    gZ_gain = preferences.getInt("gZ_gain", -1);
    preferences.end();

    if (gX_gain != -1 || gY_gain != -1 || gZ_gain != -1)
      Serial.println("Configuration saved");
    else
      Serial.println(" --------> Error saving or reading configuration <-------- ");
  }
  else {
    preferences.begin(IMU_CONF_NAME, false);
    int gX_gain = preferences.getInt("gX_gain", -1);
    int gY_gain = preferences.getInt("gY_gain", -1);
    int gZ_gain = preferences.getInt("gZ_gain", -1);
    preferences.end();

    if (gX_gain != -1 || gY_gain != -1 || gZ_gain != -1) {
      Serial.println("IMU calibrated");
      qmi.writeCalibration(gX_gain, gY_gain, gZ_gain);

      Serial.print("gX_gain: ");Serial.println(gX_gain);
      Serial.print("gY_gain: ");Serial.println(gY_gain);
      Serial.print("gZ_gain: ");Serial.println(gZ_gain);
    }
    else
      Serial.println(" --------> Error reading configuration <-------- ");
  }

  // configuración del giroscopio y acelerómetro
  Serial.println("Applying acc and gyr configuration");
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0); // Buscar como pasar esta configuracion por parametros
  qmi.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896_8Hz, SensorQMI8658::LPF_MODE_3); // Buscar como pasar esta configuracion por parametros
  qmi.enableGyroscope();
  qmi.enableAccelerometer();
  Serial.println("acc and gyr configuration completed");

  Serial.println("===================================");
  Serial.println("   All configurations completed   ");
  Serial.println("===================================");
}


IMUdata Imu::getGyr() {
  if (qmi.getDataReady()) {
    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
      return gyr;
    }
  }
  else {
    Serial.println(" --------> Error reading Gyr DATA <-------- ");
    Serial.println(" --------> Trying again in 1 sec <--------"); 
    return error;
  }
}

IMUdata Imu::getAcc() {
  if (qmi.getDataReady()) {
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
      return acc;
    }
  }
  else {
    Serial.println(" --------> Error reading Acc DATA <-------- ");
    Serial.println(" --------> Trying again in 1 sec <--------"); 
    return error;
  }
}