#include "wifi_state.h"


namespace Flap {
  const int enable = 3;
  const int in1    = 24;   // C1  
  const int in2    = 25;   // C2

  const int PWM_VALUES[4] = { 0, 120, 182, 255 };
}

static int currentFlap = 0;

static void flapSet(int pos) {
  pos = constrain(pos, 0, 3);
  int pwm = Flap::PWM_VALUES[pos];

  if (pwm == 0) {
    // Neutral: both off → servo returns to center
    analogWrite(Flap::enable, 0);
    digitalWrite(Flap::in1, LOW);
    digitalWrite(Flap::in2, LOW);
  } else {
    // Backwards: C2 gets PWM, C1 low
    digitalWrite(Flap::in1, LOW);
    digitalWrite(Flap::in2, HIGH);
    analogWrite(Flap::enable, pwm);
  }

  currentFlap = pos;
}

void setupFlaps() {
  pinMode(Flap::enable, OUTPUT);
  pinMode(Flap::in1,    OUTPUT);
  pinMode(Flap::in2,    OUTPUT);
  flapSet(0);
}

void loopFlaps() {
  if (wElement.flap != currentFlap) {
    flapSet(wElement.flap);
  }
}