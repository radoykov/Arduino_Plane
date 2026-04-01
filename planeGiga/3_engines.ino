#include "wifi_state.h"

namespace Eng {
  const int leftEnable  = 5;
  const int rightEnable = 4;
  const int leftIn1     = 28;
  const int leftIn2     = 29;
  const int rightIn1    = 26;
  const int rightIn2    = 27;
}

void setupEngines() {
  pinMode(Eng::leftEnable,  OUTPUT);
  pinMode(Eng::rightEnable, OUTPUT);
  pinMode(Eng::leftIn1,     OUTPUT);
  pinMode(Eng::leftIn2,     OUTPUT);
  pinMode(Eng::rightIn1,    OUTPUT);
  pinMode(Eng::rightIn2,    OUTPUT);

  digitalWrite(Eng::leftIn1,  LOW);
  digitalWrite(Eng::leftIn2,  LOW);
  digitalWrite(Eng::rightIn1, LOW);
  digitalWrite(Eng::rightIn2, LOW);
}

const int MIN_NUM = 60;

long val1 = 0, val2 = 0;

void loopEngines() {
  val1 = map(wEngine.left,  0, 9, MIN_NUM, 255);
  val2 = map(wEngine.right, 0, 9, MIN_NUM, 255);

  if (val1 <= MIN_NUM) {
    digitalWrite(Eng::leftIn1, LOW);
    digitalWrite(Eng::leftIn2, LOW);
  } else {
    //setBlades();
    digitalWrite(Eng::leftIn1, LOW);
    digitalWrite(Eng::leftIn2, HIGH);
  }

  if (val2 <= MIN_NUM) {
    digitalWrite(Eng::rightIn1, LOW);
    digitalWrite(Eng::rightIn2, LOW);
  } else {
    //setBlades();
    digitalWrite(Eng::rightIn1, HIGH);
    digitalWrite(Eng::rightIn2, LOW);
  }

  analogWrite(Eng::leftEnable,  val1);
  analogWrite(Eng::rightEnable, val2);
}