#include "1_globals.h"

float getPitchAngle()
{
  unsigned long currentTime = micros();
  float duration = (currentTime - lastPitchTime) / 1e6;
  lastPitchTime = currentTime;

  float pitch = movement.getPitch();

  static bool initialized = false;
  static double degrees = 0;
  if (!initialized) {
    degrees = atan2(movement.getX(), movement.getZ()) * RAD_TO_DEG; 
    initialized = true;
  }

  degrees += -pitch * duration;  // negated gyro

  float angle = atan2(movement.getX(), movement.getZ()) * RAD_TO_DEG;  
  degrees = alpha * degrees + (1 - alpha) * angle;
  return degrees;
}