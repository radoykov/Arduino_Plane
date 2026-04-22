#include "wifi_state.h"
#include <math.h>

// --- PIN DEFINITIONS ---
const int stepPin = 52;
const int dirPin  = 53;
const int enaPin  = 51;

// --- STEPPER CONFIG ---
const long MAX_STEP_POS = 400;
const long MIN_STEP_POS = -400;

// --- SPEED LIMITS ---
const unsigned long SPEED_MIN_US = 300;
const unsigned long SPEED_MAX_US = 25000;

// --- AIRCRAFT TURN MODEL ---
const float ROLL_DEADZONE_DEG = 4.0f;
const float ROLL_MAX_DEG      = 40.0f;
const float YAW_TRIM_FACTOR   = 0.25f;

// Exponential curve steepness.
// Higher = more dramatic difference between small and large angles.
// 3.0 = aggressive, 5.0 = very aggressive, try 4.0 first.
const float EXP_STEEPNESS = 4.0f;

// Wraparound suppression
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
  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin,  LOW);
  digitalWrite(enaPin,  HIGH);
}

void updateStepperLogic()
{
  if (!wifiConnected || wStand)
  {
    digitalWrite(enaPin, HIGH);
    stepPinHigh = false;
    digitalWrite(stepPin, LOW);
    return;
  }

  digitalWrite(enaPin, LOW);

  // --- Yaw wraparound suppression ---
  static int           lastYaw     = 0;
  static unsigned long wrapHoldoff = 0;
  int yaw = wServo.yaw;
  int yawDelta = yaw - lastYaw;
  lastYaw = yaw;
  if (abs(yawDelta) > WRAP_THRESHOLD)
    wrapHoldoff = millis() + WRAP_HOLDOFF_MS;
  int effectiveYaw = (millis() < wrapHoldoff) ? 0 : yaw;

  // --- Roll primary, yaw trim ---
  float roll = (float)wServo.roll;
  if (fabs(roll) < ROLL_DEADZONE_DEG && abs(effectiveYaw) < 5) return;

  float rollClamped = constrain(roll, -ROLL_MAX_DEG, ROLL_MAX_DEG);
  float turnDrive   = tanf(rollClamped * DEG_TO_RAD);
  float rudderTrim  = (float)effectiveYaw / 180.0f * YAW_TRIM_FACTOR;
  turnDrive += rudderTrim;

  if (currentStepPos >= MAX_STEP_POS && turnDrive > 0) return;
  if (currentStepPos <= MIN_STEP_POS && turnDrive < 0) return;

  float absDrive = constrain(fabs(turnDrive), 0.0f, 2.0f);
  if (absDrive < 0.05f) return;

  float t   = absDrive / 2.0f;
  float exp_t = (expf(EXP_STEEPNESS * t) - 1.0f) / (expf(EXP_STEEPNESS) - 1.0f);

  unsigned long interval = (unsigned long)(SPEED_MAX_US - exp_t * (SPEED_MAX_US - SPEED_MIN_US));

  unsigned long now = micros();
  if (now - lastStepTime < interval) return;
  lastStepTime = now;

  bool fwd = (turnDrive > 0);
  if (!stepPinHigh)
  {
    digitalWrite(dirPin,  fwd ? HIGH : LOW);
    digitalWrite(stepPin, HIGH);
    stepPinHigh = true;
  }
  else
  {
    digitalWrite(stepPin, LOW);
    stepPinHigh = false;
    currentStepPos += fwd ? 1 : -1;
  }
}
