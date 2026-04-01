#include "1_globals.h"

float getRollAngle()
{
  unsigned long currentTime = micros();
  float duration = (currentTime - lastRollTime) / 1e6;
  lastRollTime = currentTime;

  float roll = movement.getRoll();

  static bool initialized = false;
  static double degrees = 0;
  if (!initialized) {
    degrees = atan2(movement.getY(), movement.getZ()) * RAD_TO_DEG;
    initialized = true;
  }

  degrees += roll * duration;

  float angle = atan2(movement.getY(), movement.getZ()) * RAD_TO_DEG;
  degrees = alpha * degrees + (1 - alpha) * angle;
  return degrees;
}