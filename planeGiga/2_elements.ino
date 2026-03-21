namespace Elem
{
  int mainEnable = 6, mainIn1 = 74, mainIn2 = 75;
  int btnPin = 53;
  int isCabinOpen = 0, isRampOpen = 0, gearsDown = 0;
}

void cabin()
{
  analogWrite(Elem::mainEnable, 255);
  if (Elem::isCabinOpen == 0)
  {
    Elem::isCabinOpen = 1;
    digitalWrite(Elem::mainIn1, HIGH);
    digitalWrite(Elem::mainIn2, LOW);
    delay(4300);
  }
  else
  {
    Elem::isCabinOpen = 0;
    digitalWrite(Elem::mainIn1, LOW);
    digitalWrite(Elem::mainIn2, HIGH);
    delay(4200);
  }
  digitalWrite(Elem::mainIn1, LOW);
  digitalWrite(Elem::mainIn2, LOW);
}

void gears()
{
  analogWrite(Elem::mainEnable, 255);
  if (Elem::gearsDown == 0)
  {
    Elem::gearsDown = 1;
    digitalWrite(Elem::mainIn1, HIGH);
    digitalWrite(Elem::mainIn2, LOW);
    delay(8000);
  }
  else
  {
    Elem::gearsDown = 0;
    digitalWrite(Elem::mainIn1, LOW);
    digitalWrite(Elem::mainIn2, HIGH);
    delay(8000);
  }
  digitalWrite(Elem::mainIn1, LOW);
  digitalWrite(Elem::mainIn2, LOW);
}

bool setupElements()
{
  pinMode(Elem::btnPin, INPUT);
  pinMode(Elem::mainEnable, OUTPUT);
  pinMode(Elem::mainIn1, OUTPUT);
  pinMode(Elem::mainIn2, OUTPUT);
  return true;
}

void loopElements()
{
  if (digitalRead(Elem::btnPin) == HIGH)
    gears();
}
