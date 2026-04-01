#include <Servo.h>
#include "wifi_state.h"

namespace Srv {
  Servo pitch;
  Servo roll;
  Servo yaw;

  const int pitchPin = 8;
  const int yawPin   = 9; 
  const int rollPin  = 10;
}

void setupServo() {
  Srv::pitch.attach(Srv::pitchPin);
  Srv::roll.attach(Srv::rollPin);
  Srv::yaw.attach(Srv::yawPin);
}

void loopServo() {
  Srv::pitch.write(wServo.pitch + 90);
  Srv::roll.write(wServo.roll   + 90);
  Srv::yaw.write(wServo.yaw    + 90);
}