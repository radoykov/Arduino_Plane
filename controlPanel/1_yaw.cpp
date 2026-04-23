// 1_yaw.cpp
#include "1_globals.h"

float gyroZBias = 0;

const float biasUpdateRate     = 0.99f;
const float stationaryThreshold = 0.5f;
const float YAW_ALPHA          = 0.995f;  // tune: higher = trust gyro more, slower drift correction

void calibrateYaw()
{
  float sum = 0;
  const int N = 200;
  for (int i = 0; i < N; i++)
  {
    movement.update();
    sum += movement.getYaw();
    delay(5);
  }
  gyroZBias = sum / N;
}

void updateBias(float gyroZ)
{
  if (fabs(gyroZ) < stationaryThreshold)
    gyroZBias = biasUpdateRate * gyroZBias + (1.0f - biasUpdateRate) * gyroZ;
}

float getYawAngle()
{
  unsigned long now = micros();
  float dt = (now - lastYawTime) / 1e6f;
  lastYawTime = now;

  float gyroZ = movement.getYaw();
  updateBias(gyroZ);
  float correctedZ = gyroZ - gyroZBias;

  static float yaw = 0;

  // Integrate gyro
  yaw += correctedZ * dt;

  // Complementary: slowly pull back toward 0 (initial state)
  yaw = YAW_ALPHA * yaw + (1.0f - YAW_ALPHA) * 0.0f;

  // Wrap
  if (yaw >  180.0f) yaw -= 360.0f;
  if (yaw < -180.0f) yaw += 360.0f;

  return yaw;
}