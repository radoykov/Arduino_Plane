#include "globals.h"

float getRollAngle() {
  unsigned long currentTime = micros();
  float duration = (currentTime - lastRollTime) / 1e6;  // seconds
  lastRollTime = currentTime;

  movement.update();

  float roll = movement.getRoll();  // gyro pitch rate (°/s)

  // integrate gyro pitch rate
  static double degrees = 0;
  degrees += roll * duration;

  // get accelerometer-based pitch angle (gravity reference)
  float angle = atan2(movement.getY(), movement.getZ()) * RAD_TO_DEG;

  // complementary filter: blend gyro integration with accel reference
  degrees = alpha * degrees + (1 - alpha) * angle;
  return degrees;
}