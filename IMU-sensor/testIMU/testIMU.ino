#include "imu.h"

Imu imu;

void setup() {
  Serial.begin(115200);
  imu.init();
  /*
  setupIMU();
  */

}

void loop() {
  /*
  readIMU();
  struct data = imu.read()
  print(data.xgisocopio)
  */
}

/*
  TO DO

  [x] cambiar de nombre los archivos
  [x] clase IMU
  [] metodo init -> iniciar el imu y mostrar si inicio bien o no
  [] setupIMU -> configurar el IMU, calibrar(opcional), guardar/leer la calibracion
  [] read -> devolver data del gyro y acc
*/
