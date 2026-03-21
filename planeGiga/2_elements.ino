#include "wifi_state.h" 

namespace Elem
{
  int mainEnable = 6, mainIn1 = 74, mainIn2 = 75;
  int btnPin     = 53;

  // motor state machine
  bool          motorRunning  = false;
  bool          motorFwd      = true;
  unsigned long motorStartMs  = 0;
  unsigned long motorDuration = 0;

  // logical states
  int isCabinOpen = 0;
  int isRampOpen  = 0;
  int gearsDown   = 0;
}

// ── non-blocking motor start ──────────────────────────────
void elemStartMotor(bool forward, unsigned long duration)
{
  if (Elem::motorRunning) return;
  Elem::motorRunning  = true;
  Elem::motorFwd      = forward;
  Elem::motorStartMs  = millis();
  Elem::motorDuration = duration;
  analogWrite (Elem::mainEnable, 255);
  digitalWrite(Elem::mainIn1, forward ? HIGH : LOW);
  digitalWrite(Elem::mainIn2, forward ? LOW  : HIGH);
}

void elemStopMotor()
{
  Elem::motorRunning = false;
  analogWrite (Elem::mainEnable, 0);
  digitalWrite(Elem::mainIn1, LOW);
  digitalWrite(Elem::mainIn2, LOW);
}

void elemTickMotor()
{
  if (Elem::motorRunning && millis() - Elem::motorStartMs >= Elem::motorDuration)
    elemStopMotor();
}

// ── split cabin / gear / ramp into open/close ─────────────
void cabinOpen()
{
  if (Elem::isCabinOpen) return;
  Elem::isCabinOpen = 1;
  elemStartMotor(true, 4300);
}

void cabinClose()
{
  if (!Elem::isCabinOpen) return;
  Elem::isCabinOpen = 0;
  elemStartMotor(false, 4200);
}

void gearsUp()
{
  if (!Elem::gearsDown) return;
  Elem::gearsDown = 0;
  elemStartMotor(false, 8000);
}

void gearsDown()
{
  if (Elem::gearsDown) return;
  Elem::gearsDown = 1;
  elemStartMotor(true, 8000);
}

void rampUp()
{
  if (Elem::isRampOpen) return;
  Elem::isRampOpen = 1;
  elemStartMotor(true, 4000);
}

void rampDown()
{
  if (!Elem::isRampOpen) return;
  Elem::isRampOpen = 0;
  elemStartMotor(false, 4000);
}

bool setupElements()
{
  pinMode(Elem::btnPin,     INPUT);
  pinMode(Elem::mainEnable, OUTPUT);
  pinMode(Elem::mainIn1,    OUTPUT);
  pinMode(Elem::mainIn2,    OUTPUT);
  return true;
}

void loopElements()
{
  elemTickMotor();

  // physical button still triggers gears as before
  if (digitalRead(Elem::btnPin) == HIGH) {
    Elem::gearsDown ? gearsUp() : gearsDown();
  }
}
