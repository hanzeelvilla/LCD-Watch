#include "imu.h"

Imu imu;

void setup() {
  Serial.begin(115200);
  delay(100);
  imu.init();
  imu.setup();

  delay(1000); // delay para que se inicialize correctamente
}

void loop() {
  IMUdata gyr = imu.getGyr();
  IMUdata acc = imu.getAcc();

  Serial.print("Gyr X: "); Serial.print(gyr.x);
  Serial.print(" Gyr Y: "); Serial.print(gyr.y);
  Serial.print(" Gyr Z: "); Serial.println(gyr.z);

  Serial.print("Acc X: "); Serial.print(acc.x);
  Serial.print(" Acc Y: "); Serial.print(acc.y);
  Serial.print(" Acc Z: "); Serial.println(acc.z);

  delay(1000);
}