#include <Servo.h>

namespace Clutch
{
  const int servoPin = 4;
  Servo servo;
  int value = 0;
}

bool setupClutch()
{
  pinMode(Clutch::potPin, INPUT);
  Clutch::servo.attach(Clutch::servoPin);
  return true;
}

void loopClutch()
{
  Clutch::servo.write(map(Clutch::value, 0, 1023, 80, 110));
}
