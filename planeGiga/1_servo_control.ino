#include "wifi_state.h"
#include <math.h>

const int stepPin = 52;
const int dirPin  = 53;
const int enaPin  = 51;

const long MAX_STEP_POS = 400;
const long MIN_STEP_POS = -400;

const unsigned long SPEED_MIN_US = 4000;    // fastest
const unsigned long SPEED_MAX_US = 20000;  // slowest

const float ROLL_DEADZONE = 5.0f;
const float YAW_DEADZONE  = 8.0f;
const float ROLL_MAX      = 40.0f;
const float YAW_MAX       = 180.0f;
const float YAW_WEIGHT    = 0.3f;  // yaw adds 30% on top of roll

const int           WRAP_THRESHOLD  = 90;
const unsigned long WRAP_HOLDOFF_MS = 300;

long          currentStepPos = 0;
unsigned long lastStepTime   = 0;
bool          stepPinHigh    = false;

void setupStepper()
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin,  OUTPUT);
  pinMode(enaPin,  OUTPUT);
  digitalWrite(stepPin, HIGH);
  digitalWrite(dirPin,  HIGH);
  digitalWrite(enaPin,  LOW);
}

void updateStepperLogic()
{
  if (!wifiConnected || wStand)
  {
    digitalWrite(enaPin, LOW);
    digitalWrite(stepPin, HIGH);
    stepPinHigh = false;
    return;
  }

  digitalWrite(enaPin, HIGH);

  // Yaw wraparound suppression
  static int           lastYaw     = 0;
  static unsigned long wrapHoldoff = 0;
  int yaw      = wServo.yaw;
  int yawDelta = yaw - lastYaw;
  lastYaw      = yaw;
  if (abs(yawDelta) > WRAP_THRESHOLD)
    wrapHoldoff = millis() + WRAP_HOLDOFF_MS;
  float effectiveYaw = (millis() < wrapHoldoff) ? 0.0f : (float)yaw;

  float roll = (float)wServo.roll;

  // Deadzone
  float rollInput = (fabs(roll) < ROLL_DEADZONE) ? 0.0f : roll;
  float yawInput  = (fabs(effectiveYaw) < YAW_DEADZONE) ? 0.0f : effectiveYaw;

  // Linear 0..1 for each axis
  float rollT = constrain(fabs(rollInput) / ROLL_MAX, 0.0f, 1.0f);
  float yawT  = constrain(fabs(yawInput)  / YAW_MAX,  0.0f, 1.0f) * YAW_WEIGHT;
  float t     = constrain(rollT + yawT,  0.0f, 1.0f);

  if (t < 0.01f) return;

  // Drive direction
  float turnDrive = rollInput + yawInput * YAW_WEIGHT;
  if (currentStepPos >= MAX_STEP_POS && turnDrive > 0) return;
  if (currentStepPos <= MIN_STEP_POS && turnDrive < 0) return;

  // Linear interval: bigger angle = smaller interval = faster
  unsigned long interval = (unsigned long)(SPEED_MAX_US - t * (SPEED_MAX_US - SPEED_MIN_US));

  unsigned long now = micros();
  if (now - lastStepTime < interval) return;
  lastStepTime = now;

  bool fwd = (turnDrive > 0);

  if (!stepPinHigh)
  {
    digitalWrite(dirPin,  fwd ? LOW : HIGH);
    digitalWrite(stepPin, LOW);
    stepPinHigh = true;
  }
  else
  {
    digitalWrite(stepPin, HIGH);
    stepPinHigh = false;
    currentStepPos += fwd ? 1 : -1;
  }
}