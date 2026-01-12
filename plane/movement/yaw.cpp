#include "globals.h"

float gyroZBias = 0;

const float biasUpdateRate = 0.99;
const float stationaryThreshold = 0.5;

void calibrateYaw() {
  float sum = 0;
  const int N = 200;  // number of samples

  for (int i = 0; i < N; i++) {
    movement.update();
    sum += movement.getYaw();
    delay(5);
  }
  gyroZBias = sum / N;
}

void updateBias(float gyroZ) {
  if (fabs(gyroZ) < stationaryThreshold) {
    gyroZBias = biasUpdateRate * gyroZBias + (1.0 - biasUpdateRate) * gyroZ;
  }
}

float getYawAngle() {
  unsigned long now = micros();
  float dt = (now - lastYawTime) / 1e6;
  lastYawTime = now;

  movement.update();

  float gyroZ = movement.getYaw();

  updateBias(gyroZ);

  float correctedZ = gyroZ - gyroZBias;

  static float yaw = 0;
  yaw += correctedZ * dt;

  if (yaw > 180) yaw -= 360;
  if (yaw < -180) yaw += 360;

  return yaw;
}