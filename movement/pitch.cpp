#include "globals.h"

float getPitchAngle(){
  unsigned long currentTime = micros();
  float duration = (currentTime - lastPitchTime) / 1e6; // seconds
  lastPitchTime = currentTime;

  movement.update();

  float pitch = movement.getPitch(); // gyro pitch rate (°/s)

  // integrate gyro pitch rate
  static double degrees = 0;
  degrees += pitch * duration;

  // get accelerometer-based pitch angle (gravity reference)
  float angle = atan2(movement.getX(), movement.getZ()) * RAD_TO_DEG; 

  // complementary filter: blend gyro integration with accel reference
  degrees = alpha * degrees + (1 - alpha) * angle;
  return degrees;
}