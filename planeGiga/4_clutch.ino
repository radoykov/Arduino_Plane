#include <Servo.h>
#include "wifi_state.h"

namespace ClutchGearsCabin
{
  const int servoPin = 5;
  Servo servo;
  int value = 0;
}
namespace ClutchRampBlades
{
  const int servoPin = 6;
  Servo servo;
  int value = 0;
}

bool setupClutch()
{
  ClutchGearsCabin::servo.attach(ClutchGearsCabin::servoPin);
  ClutchRampBlades::servo.attach(ClutchRampBlades::servoPin);
  return true;
}

void setRamp()  {ClutchRampBlades::servo.write(110);}
void setBlades()  {ClutchRampBlades::servo.write(70);}
void setGears() {ClutchGearsCabin::servo.write(89);}
void setCabin() {ClutchGearsCabin::servo.write(99);}