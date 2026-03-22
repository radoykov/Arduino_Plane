#include <Servo.h>
#include "wifi_state.h"

namespace Srv {
Servo pitch;
Servo roll;
Servo yaw;

const int pitchPin = 7;
const int rollPin = 8;
const int yawPin = 9;
}

bool setupServo() {
  Srv::pitch.attach(Srv::pitchPin);
  Srv::roll.attach(Srv::rollPin);
  Srv::yaw.attach(Srv::yawPin);
  return true;
}

void loopServo() {
  // wServo values are –90…+90; add 90 to map to 0…180 servo range.
  Srv::pitch.write(wServo.pitch + 90);
  Srv::roll.write(wServo.roll + 90);
  Srv::yaw.write(wServo.yaw + 90);

  Serial.println(wServo.pitch + 90);
}
