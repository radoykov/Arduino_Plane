#include "wifi_state.h"

namespace Elem {
  const int mainEnable = 6;
  const int mainIn1    = 74;
  const int mainIn2    = 75;
  const int btnPin     = 53;

  bool          motorRunning  = false;
  bool          motorFwd      = true;
  unsigned long motorStartMs  = 0;
  unsigned long motorDuration = 0;

  bool gearUp   = false;
  bool rampOpen = false;
  bool cabinOpen = false;
}

static bool prevGear  = false;
static bool prevRamp  = false;
static bool prevCabin = false;

void elemStartMotor(bool forward, unsigned long durationMs) {
  if (Elem::motorRunning) return;
  Elem::motorRunning  = true;
  Elem::motorFwd      = forward;
  Elem::motorStartMs  = millis();
  Elem::motorDuration = durationMs;
  analogWrite (Elem::mainEnable, 255);
  digitalWrite(Elem::mainIn1, forward ? HIGH : LOW);
  digitalWrite(Elem::mainIn2, forward ? LOW  : HIGH);
}

void elemStopMotor() {
  Elem::motorRunning = false;
  analogWrite (Elem::mainEnable, 0);
  digitalWrite(Elem::mainIn1, LOW);
  digitalWrite(Elem::mainIn2, LOW);
}

void elemTickMotor() {
  if (Elem::motorRunning && millis() - Elem::motorStartMs >= Elem::motorDuration)
    elemStopMotor();
}

void gearsUp()    { if ( Elem::gearUp)    return; Elem::gearUp    = true;  elemStartMotor(false, 8000); }
void gearsDown()  { if (!Elem::gearUp)    return; Elem::gearUp    = false; elemStartMotor(true,  8000); }
void rampUp()     { if ( Elem::rampOpen)  return; Elem::rampOpen  = true;  elemStartMotor(true,  4000); }
void rampDown()   { if (!Elem::rampOpen)  return; Elem::rampOpen  = false; elemStartMotor(false, 4000); }
void cabinOpen()  { if ( Elem::cabinOpen) return; Elem::cabinOpen = true;  elemStartMotor(true,  4300); }
void cabinClose() { if (!Elem::cabinOpen) return; Elem::cabinOpen = false; elemStartMotor(false, 4200); }

bool setupElements() {
  pinMode(Elem::btnPin,     INPUT);
  pinMode(Elem::mainEnable, OUTPUT);
  pinMode(Elem::mainIn1,    OUTPUT);
  pinMode(Elem::mainIn2,    OUTPUT);
  return true;
}

void loopElements() {
  elemTickMotor();

  if (wElement.gear != prevGear) {
    prevGear = wElement.gear;
    wElement.gear ? gearsUp() : gearsDown();
  }
  if (wElement.ramp != prevRamp) {
    prevRamp = wElement.ramp;
    wElement.ramp ? rampUp() : rampDown();
  }
  if (wElement.cabin != prevCabin) {
    prevCabin = wElement.cabin;
    wElement.cabin ? cabinOpen() : cabinClose();
  }

  if (digitalRead(Elem::btnPin) == HIGH)
    Elem::gearUp ? gearsDown() : gearsUp();
}
