#include "imu.h"

Imu imu;

// Configuración de thresholds
float impacto_threshold = 2.0;  // Aceleración brusca en g
float angle_threshold = 45.0;   // Cambio de orientación en grados
float low_activity_threshold = 0.5; // Aceleración baja en g
unsigned long low_activity_time = 1000; // Tiempo en ms de baja actividad

// Variables globales
float pitch = 0, roll = 0;
float accel_magnitude = 0;
unsigned long impact_time = 0;

  static float gyroPitch = 0, gyroRoll = 0;
  unsigned long previousTime = 0;

void setup() {
  Serial.begin(115200);
  imu.init();
  imu.setup();
}

void loop() {
  // Leer datos del sensor
  IMUdata acc = imu.getAcc();
  IMUdata gyr = imu.getGyr();

  // Calcular la magnitud de la aceleración
  accel_magnitude = sqrt(acc.x * acc.x + acc.y * acc.y + acc.z * acc.z);

  // Calcular ángulos con el filtro complementario
  float epsilon = 1e-6; // Pequeño valor para evitar divisiones por cero
  float accelPitch = atan2(-acc.x, sqrt(acc.y * acc.y + acc.z * acc.z + epsilon)) * 180.0 / PI;
  float accelRoll = atan2(acc.y, sqrt(acc.x * acc.x + acc.z * acc.z + epsilon)) * 180.0 / PI;
  Serial.print("AccelPitch:"); Serial.println(accelPitch);
  Serial.print("AccelRoll:"); Serial.println(accelRoll);

  unsigned long currentTime = millis();
  float deltaTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;

  gyroPitch += gyr.x * deltaTime;
  gyroRoll += gyr.y * deltaTime;

  Serial.print("gyroPitch: "); Serial.println(gyroPitch);
  Serial.print("gyroRoll: "); Serial.println(gyroRoll);

  // Filtro complementario
  pitch = 0.98 * (gyroPitch) + 0.02 * accelPitch;
  roll = 0.98 * (gyroRoll) + 0.02 * accelRoll;

  // Detectar impacto brusco
  if (accel_magnitude > impacto_threshold) {
    impact_time = millis(); // Registrar el tiempo del impacto
  }

  // Detectar cambio de orientación
  static float last_pitch = pitch, last_roll = roll;
  bool orientation_change = (abs(pitch - last_pitch) > angle_threshold || abs(roll - last_roll) > angle_threshold);
  last_pitch = pitch;
  last_roll = roll;

  // Detectar baja actividad
  if ((millis() - impact_time < 2000) && accel_magnitude < low_activity_threshold) {
    if (orientation_change) {
      Serial.println("Caída detectada!");
    }
  }

  // Mostrar valores en el monitor serial
  Serial.print("Pitch: "); Serial.print(pitch);
  Serial.print(", Roll: "); Serial.print(roll);
  Serial.print(", Magnitud Acel: "); Serial.println(accel_magnitude);

  delay(100);
}
