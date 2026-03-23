#include "wifi_state.h"

namespace Eng {
const int leftEnable = 2; 
const int rightEnable = 3;
const int leftIn1 = 22;
const int leftIn2 = 23;
const int rightIn1 = 24;
const int rightIn2 = 25;
}

bool setupEngines() {
  pinMode(Eng::leftEnable, OUTPUT);
  pinMode(Eng::rightEnable, OUTPUT);
  pinMode(Eng::leftIn1, OUTPUT);
  pinMode(Eng::leftIn2, OUTPUT);
  pinMode(Eng::rightIn1, OUTPUT);
  pinMode(Eng::rightIn2, OUTPUT);

  // Fixed forward direction
  setBlades();
  digitalWrite(Eng::leftIn1, LOW);
  digitalWrite(Eng::leftIn2, HIGH);
  digitalWrite(Eng::rightIn1, HIGH);
  digitalWrite(Eng::rightIn2, LOW);
  return true;
}

const int MIN_NUM = 56;  // MIN !

long val1 = 0, val2 = 0;

void loopEngines() {
  val1 = map(wEngine.left, 0, 9, MIN_NUM, 255);
  val2 = map(wEngine.right, 0, 9, MIN_NUM, 255);

  if (val1 <= MIN_NUM) {
    digitalWrite(Eng::leftIn1, LOW);
    digitalWrite(Eng::leftIn2, LOW);
  } else {
    setBlades();
    digitalWrite(Eng::leftIn1, LOW);
    digitalWrite(Eng::leftIn2, HIGH);
  }
  if(val2 <= MIN_NUM) {
    digitalWrite(Eng::rightIn1, LOW);
    digitalWrite(Eng::rightIn2, LOW);
  }else{
    setBlades();
    digitalWrite(Eng::rightIn1, HIGH);
    digitalWrite(Eng::rightIn2, LOW);
  }
  // Serial.println(val1, val2);
  analogWrite(Eng::leftEnable, val1);
  analogWrite(Eng::rightEnable, val2);
}
