#include <Servo.h>
#include "globals.h"

ModulinoMovement movement;
Servo myservo;

unsigned long lastPitchTime = 0;
unsigned long lastRollTime = 0;
unsigned long lastYawTime = 0;
float alpha = 0.95;

// filter constant (0.95–0.99 typical)
float sum = 0;
void setup() {
  Serial.begin(9600);
  Modulino.begin();
  movement.begin();
  lastPitchTime = lastRollTime = lastYawTime = micros();
  alpha = 0.95;
  calibrateYaw();

  myservo.attach(7);
}

void loop() {
  int d = getPitchAngle();
  if (d >= -90 && d <= 90) {
    myservo.write(getPitchAngle() + 90);
  }
  Serial.println();
  // Serial.print("| ");
  // Serial.print(getRollAngle());
  // Serial.print("| ");
  // Serial.print(getYawAngle());
  // Serial.print("| ");
  // Serial.println("");

  // delay(5);
}