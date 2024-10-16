#include "ImuCalibration.h"

IMU imu;

void setup() {
  Serial.begin(115200);

  imu.init()
  setupIMU(true);

}

void loop() {
  readIMU();

  struct data = imu.read()
  print(data.xgisocopio)
}

/*
  TO DO

  [] cambiar de nombre los archivos
  [] clase IMU
  [] metodo init -> iniciar el imu y mostrar si inicio bien o no
  [] setupIMU -> configurar el IMU, calibrar(opcional), guardar/leer la calibracion
  [] read -> devolver data del gyro y acc
*/
