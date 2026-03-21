#include <Servo.h>

namespace Clutch
{
  const int btnPin   = 53;
  const int servoPin = 4;
  const int potPin   = A6;  // A7 not available on GIGA, use A6
  Servo servo;
  int value = 0;
}

bool setupClutch()
{
  pinMode(Clutch::btnPin, INPUT);
  pinMode(Clutch::potPin, INPUT);
  Clutch::servo.attach(Clutch::servoPin);
  return true;
}

void loopClutch()
{
  Clutch::value = map(analogRead(Clutch::potPin), 0, 1023, 0, 180);
  Clutch::servo.write(Clutch::value);
}
