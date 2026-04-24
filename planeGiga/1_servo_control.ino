#include "wifi_state.h"
#include <math.h>

const int enaPin  = 51;//orange
const int stepPin = 52;//grey
const int dirPin  = 53;//green

const long MAX_STEP_POS = 1600;
const long MIN_STEP_POS = -1600;

const unsigned long SPEED_MIN_US = 2000;
const unsigned long SPEED_MAX_US = 5000;

const float ROLL_DEADZONE        = 5.0f;
const float YAW_DEADZONE         = 8.0f;
const float ROLL_MAX             = 40.0f;
const float YAW_MAX              = 180.0f;
const float YAW_WEIGHT           = 0.9f;
const float ENGINE_DIFF_WEIGHT   = 0.5f;
const int   ENGINE_DEADZONE      = 5;

const int           WRAP_THRESHOLD  = 90;
const unsigned long WRAP_HOLDOFF_MS = 300;

long          currentStepPos = 0;
unsigned long lastStepTime   = 0;
bool          stepPinHigh    = false;

void setupStepper() {
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
  float rollInput   = (fabs(roll)         < ROLL_DEADZONE) ? 0.0f : roll;
  float yawInput    = (fabs(effectiveYaw) < YAW_DEADZONE)  ? 0.0f : effectiveYaw;

  // Engine differential: left fails → rotates left → positive engineInput = turn right
  int   engineDiff  = wEngine.right - wEngine.left;
  float engineInput = (abs(engineDiff) < ENGINE_DEADZONE) ? 0.0f : (float)engineDiff;

  // Normalize all axes linearly to 0..1
  float rollT   = constrain(fabs(rollInput)   / ROLL_MAX,  0.0f, 1.0f);
  float yawT    = constrain(fabs(yawInput)    / YAW_MAX,   0.0f, 1.0f);
  float engineT = constrain(fabs(engineInput) / 100.0f,    0.0f, 1.0f) * ENGINE_DIFF_WEIGHT;
  float t       = constrain(rollT + yawT * YAW_WEIGHT + engineT, 0.0f, 1.0f);

  if (t < 0.01f) return;

  // Weighted sum — all normalized to [-1..1] before weighting
  float turnDrive = -(rollInput   / ROLL_MAX)
                  + (yawInput    / YAW_MAX)  * YAW_WEIGHT
                  + (engineInput / 100.0f)   * ENGINE_DIFF_WEIGHT;

  if (currentStepPos >= MAX_STEP_POS && turnDrive > 0) return;
  if (currentStepPos <= MIN_STEP_POS && turnDrive < 0) return;

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