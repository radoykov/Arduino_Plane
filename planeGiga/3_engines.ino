#include "wifi_state.h"

namespace Eng
{
  const int leftEnable = 2;
  const int leftIn1 = 22;
  const int leftIn2 = 23;
  const int leftToggle = 24;
  const int leftThrottle = A0;
  const int rightEnable = 3;
  const int rightIn1 = 26;
  const int rightIn2 = 27;
  const int rightToggle = 28;
  const int rightThrottle = A1;
}

bool setupEngines()
{
  pinMode(Eng::leftEnable, OUTPUT);
  pinMode(Eng::rightEnable, OUTPUT);
  pinMode(Eng::leftIn1, OUTPUT);
  pinMode(Eng::leftIn2, OUTPUT);
  pinMode(Eng::rightIn1, OUTPUT);
  pinMode(Eng::rightIn2, OUTPUT);
  pinMode(Eng::leftToggle, INPUT);
  pinMode(Eng::rightToggle, INPUT);
  // Fixed forward direction
  digitalWrite(Eng::leftIn1, HIGH);
  digitalWrite(Eng::leftIn2, LOW);
  digitalWrite(Eng::rightIn1, HIGH);
  digitalWrite(Eng::rightIn2, LOW);
  return true;
}

const MIN_NUM = 80; // MIN !

long val1, val2;

void loopEngines()
{
  val1 = map(wEngine.left, 0, 9, MIN_NUM, 255);
  val2 = map(wEngine.right, 0, 9, MIN_NUM, 255);

  if (val1 <= MIN_NUM)
  {
    digitalWrite(Eng::leftIn1, LOW);
    digitalWrite(Eng::leftIn2, LOW);
  }
  else if (val2 <= MIN_NUM)
  {
    digitalWrite(Eng::rightIn1, LOW);
    digitalWrite(Eng::rightIn2, LOW);
  }

  if (Eng::leftIn1 == LOW && Eng::leftIn2 == LOW)
  {
    digitalWrite(Eng::leftIn1, HIGH);
    digitalWrite(Eng::leftIn2, LOW);
  }
  else if (Eng::rightIn1 == LOW && Eng::rightIn2 == LOW)
  {
    digitalWrite(Eng::rightIn1, HIGH);
    digitalWrite(Eng::rightIn2, LOW);
  }

  analogWrite(Eng::leftEnable, val1);
  analogWrite(Eng::rightEnable, val2);
}
