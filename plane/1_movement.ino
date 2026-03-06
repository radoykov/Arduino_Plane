#include <Servo.h>
#include "1_globals.h"

ModulinoMovement movement;

namespace Mov
{
  Servo servo;
  unsigned long lastPitchTime = 0;
  unsigned long lastRollTime = 0;
  unsigned long lastYawTime = 0;
  float alpha = 0.95;
}

// expose globals required by pitch/roll/yaw .cpp files
unsigned long lastPitchTime;
unsigned long lastRollTime;
unsigned long lastYawTime;
float alpha;

bool setupMovement()
{
  Serial.begin(9600);
  Modulino.begin();
  movement.begin();
  lastPitchTime = lastRollTime = lastYawTime = micros();
  alpha = 0.95;
  calibrateYaw();
  Mov::servo.attach(7);
  return true;
}

void loopMovement()
{
  int d = getRollAngle();
  if (d >= -90 && d <= 90)
  {
    Mov::servo.write(d + 90);
  }
}
