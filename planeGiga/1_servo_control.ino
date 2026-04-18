#include "wifi_state.h"

// --- PIN DEFINITIONS ---
const int stepPin = 52;
const int dirPin  = 53;

long currentStepPos = 0;

void setupStepper()
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin,  OUTPUT);
  Serial.println("Stepper init.");
}

void updateStepperLogic()
{
  if (!wifiConnected) return;

  long targetStep = map(wServo.yaw, -45, 45, -100, 100);

  if (currentStepPos != targetStep)
  {
    bool fwd = (targetStep > currentStepPos);
    digitalWrite(dirPin,  fwd ? HIGH : LOW);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(600);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(600);

    currentStepPos += fwd ? 1 : -1;
  }
}