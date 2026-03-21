#include <Servo.h>

namespace Srv
{
  Servo servo;
  int servoPin = 6;
  int potPin   = A0;
  int value    = 0;
}

bool setupServo()
{
  Srv::servo.attach(Srv::servoPin);
  return true;
}

void loopServo()
{
  Srv::value = map(analogRead(Srv::potPin), 0, 1023, 0, 180);
  Srv::servo.write(Srv::value);
}
