namespace Eng
{
  int leftEnable   = 2,  leftIn1  = 22, leftIn2  = 23;
  int leftToggle   = 24, leftLed  = 25, leftThrottle  = A0;
  int rightEnable  = 3,  rightIn1 = 26, rightIn2 = 27;
  int rightToggle  = 28, rightLed = 29, rightThrottle = A1;
  int leftValue    = 0,  rightValue = 0;
}

bool setupEngines()
{
  pinMode(Eng::leftEnable,  OUTPUT);
  pinMode(Eng::rightEnable, OUTPUT);
  pinMode(Eng::leftIn1,     OUTPUT);
  pinMode(Eng::leftIn2,     OUTPUT);
  pinMode(Eng::rightIn1,    OUTPUT);
  pinMode(Eng::rightIn2,    OUTPUT);
  digitalWrite(Eng::leftIn1,  HIGH);
  digitalWrite(Eng::leftIn2,  LOW);
  digitalWrite(Eng::rightIn1, LOW);
  digitalWrite(Eng::rightIn2, HIGH);
  return true;
}

void loopEngines()
{
  if (wEng[0] == 0 && wEng[1] == 0)
  {
    // left
    Eng::leftValue = map(analogRead(Eng::leftThrottle), 0, 1023, 80, 255);
    if (digitalRead(Eng::leftToggle) == HIGH) {
      digitalWrite(Eng::leftLed, HIGH);
    } else {
      Eng::leftValue = 0;
      digitalWrite(Eng::leftLed, LOW);
    }
    analogWrite(Eng::leftEnable, Eng::leftValue);

    // right
    Eng::rightValue = map(analogRead(Eng::rightThrottle), 0, 1023, 80, 255);
    if (digitalRead(Eng::rightToggle) == HIGH) {
      digitalWrite(Eng::rightLed, HIGH);
    } else {
      Eng::rightValue = 0;
      digitalWrite(Eng::rightLed, LOW);
    }
    analogWrite(Eng::rightEnable, Eng::rightValue);
  }
}
