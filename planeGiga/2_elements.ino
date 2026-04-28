#include <Servo.h>
#include "wifi_state.h"

const int SERVO1_PIN  = 6;
const int SERVO2_PIN  = 7;
const int MOTOR_PWM   = 2;
const int MOTOR_DIR_A = 22;
const int MOTOR_DIR_B = 23;

struct FnConfig {
  Servo*        servo;
  int           servoAngle;
  bool          motorForward;
  unsigned long motorRunMs;
};

const unsigned long SERVO_SETTLE_MS  = 300;
const int           SERVO_HOME_ANGLE = 90;
const int           MOTOR_SPEED      = 200;

Servo servo1;
Servo servo2;

FnConfig fnTable[] = {
  { &servo1, 115, false,  3000 },  // RAMP_DOWN  (0)
  { &servo1, 115, true, 3000 },  // RAMP_UP    (1)
  { &servo2,  68, true,  5900 },  // GEAR_DOWN  (2)
  { &servo2,  68, false, 5900 },  // GEAR_UP    (3)
  { &servo2, 110, true,  2950 },  // CABIN_DOWN (4)
  { &servo2, 110, false, 3050 },  // CABIN_UP   (5)
};

enum State { IDLE, SERVO_MOVING, MOTOR_RUNNING, SERVO_RETURNING };
State      state          = IDLE;
FunctionID activeFn       = FN_NONE;
FunctionID lastExecutedFn = FN_NONE;
unsigned long stateStart  = 0;

void motorStart(bool forward) {
  digitalWrite(MOTOR_DIR_A, forward ? HIGH : LOW);
  digitalWrite(MOTOR_DIR_B, forward ? LOW  : HIGH);
  analogWrite(MOTOR_PWM, MOTOR_SPEED);
}

void motorStop() {
  digitalWrite(MOTOR_DIR_A, LOW);
  digitalWrite(MOTOR_DIR_B, LOW);
  analogWrite(MOTOR_PWM, 0);
}

void setupElements() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(SERVO_HOME_ANGLE);
  servo2.write(SERVO_HOME_ANGLE);
  pinMode(MOTOR_PWM,   OUTPUT);
  pinMode(MOTOR_DIR_A, OUTPUT);
  pinMode(MOTOR_DIR_B, OUTPUT);
  motorStop();
}

void loopElements() {
  unsigned long now = millis();

  switch (state) {

    case IDLE:
      // Ignore no-command and anything we already ran
      if (wElement.functionId == FN_NONE)            return;
      if (wElement.functionId == lastExecutedFn)     return;  // ← KEY FIX

      activeFn   = wElement.functionId;
      stateStart = now;
      fnTable[activeFn].servo->write(fnTable[activeFn].servoAngle);
      state = SERVO_MOVING;
      Serial.print("Starting function: ");
      Serial.println(activeFn);
      break;

    case SERVO_MOVING:
      if (now - stateStart >= SERVO_SETTLE_MS) {
        motorStart(fnTable[activeFn].motorForward);
        stateStart = now;
        state = MOTOR_RUNNING;
      }
      break;

    case MOTOR_RUNNING:
      if (now - stateStart >= fnTable[activeFn].motorRunMs) {
        motorStop();
        fnTable[activeFn].servo->write(SERVO_HOME_ANGLE);
        stateStart = now;
        state = SERVO_RETURNING;
      }
      break;

    case SERVO_RETURNING:
      if (now - stateStart >= SERVO_SETTLE_MS) {
        lastExecutedFn = activeFn;  // ← remember what we just did
        activeFn       = FN_NONE;
        state          = IDLE;
      }
      break;

    default: break;
  }
}