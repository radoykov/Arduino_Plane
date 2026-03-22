#include <Servo.h>
#include "wifi_state.h"

namespace Srv {
Servo pitch;
Servo roll;
Servo yaw;

const int pitchPin = 2;
const int rollPin = 3;
const int yawPin = 5;
}

bool setupServo() {
  Srv::pitch.attach(Srv::pitchPin);
  Srv::roll.attach(Srv::rollPin);
  Srv::yaw.attach(Srv::yawPin);
  return true;
}

void loopServo() {
  // wServo values are –90…+90; add 90 to map to 0…180 servo range.
  Srv::pitch.write(constrain(wServo.roll + 90, 0, 180));
  Srv::roll.write(constrain(wServo.roll + 90, 0, 180));
  Srv::yaw.write(constrain(wServo.yaw + 90, 0, 180));

  Serial.println(constrain(wServo.pitch + 90, 0, 180));
}
