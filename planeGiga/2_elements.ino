#include "wifi_state.h"

namespace Elem {
const int mainEnable = 4;
const int mainIn1 = 26;
const int mainIn2 = 27;

bool motorRunning = false;
unsigned long motorStartMs = 0;
unsigned long motorDuration = 0;
}

static bool prevGear = false;
static bool prevRamp = false;
static bool prevCabin = false;

static void setMotor(bool forward, unsigned long durationMs) {
  if (Elem::motorRunning) return;
  Elem::motorRunning = true;
  Elem::motorStartMs = millis();
  Elem::motorDuration = durationMs;
  analogWrite(Elem::mainEnable, 255);
  digitalWrite(Elem::mainIn1, forward ? HIGH : LOW);
  digitalWrite(Elem::mainIn2, forward ? LOW : HIGH);
}

static void stopMotor() {
  Elem::motorRunning = false;
  analogWrite(Elem::mainEnable, 0);
  digitalWrite(Elem::mainIn1, LOW);
  digitalWrite(Elem::mainIn2, LOW);
}

bool setupElements() {
  pinMode(Elem::mainEnable, OUTPUT);
  pinMode(Elem::mainIn1, OUTPUT);
  pinMode(Elem::mainIn2, OUTPUT);
  return true;
}

void loopElements() {
  if (Elem::motorRunning && millis() - Elem::motorStartMs >= Elem::motorDuration)
    stopMotor();

  if (wElement.gear != prevGear) {
    setGears();
    prevGear = wElement.gear;
    setMotor(wElement.gear, 8000);
  }
  if (wElement.ramp != prevRamp) {
    setRamp();
    prevRamp = wElement.ramp;
    setMotor(wElement.ramp, 4000);
  }
  if (wElement.cabin != prevCabin) {
    setCabin();
    prevCabin = wElement.cabin;
    setMotor(wElement.cabin, 4300);
  }
}
