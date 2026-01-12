int mainEnable = 6;
int mainIn1 = 74;
int mainIn2 = 75;
int potPin = A0;

int value = 0;
const int btnPin = 53;

//cabin
int isCabinOpen = 0;
void cabin() {
  if (isCabinOpen == 0) {
    isCabinOpen = 1;
    //ignition
    analogWrite(mainEnable, 255);
    //up
    digitalWrite(mainIn1, HIGH);
    digitalWrite(mainIn2, LOW);

    delay(4300);  //time

    //turn off engine
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, LOW);
  } else if (isCabinOpen == 1) {

    isCabinOpen = 0;
    //ignition
    analogWrite(mainEnable, 255);
    //down
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, HIGH);

    delay(4200);  //time

    //turn off engine
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, LOW);
  }
}
//ramp
int isRampOpen = 0;
void ramp() {
  if (isRampOpen == 0) {
    isRampOpen = 1;
    //speed
    analogWrite(mainEnable, 255);
    //down
    digitalWrite(mainIn1, HIGH);
    digitalWrite(mainIn2, LOW);

    delay(4200);  //time

    //turn off engine
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, LOW);
  } else if () {
    isRampOpen = 0;
    //speed
    analogWrite(mainEnable, 255);

    //up
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, HIGH);

    delay(4200);  //time

    //turn off engine
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, LOW);
  }
}

//gears
int gearsDown = 0;
void gears() {
  if (gearsDown == 0) {
    gearsDown = 1;
    //speed
    analogWrite(mainEnable, 255);
    //down
    digitalWrite(mainIn1, HIGH);
    digitalWrite(mainIn2, LOW);

    delay(8000);  //time

    //turn off engine
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, LOW);
  } else if (gearsDown == 1) {
    gearsDown = 0;
    //speed
    analogWrite(mainEnable, 255);

    //up
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, HIGH);

    delay(8000);  //time

    //turn off engine
    digitalWrite(mainIn1, LOW);
    digitalWrite(mainIn2, LOW);
  }
}

void setup() {
  //button
  pinMode(btnPin, INPUT);
  //main
  pinMode(mainEnable, OUTPUT);

  pinMode(mainIn1, OUTPUT);
  pinMode(mainIn2, OUTPUT);
}

void loop() {
  if (digitalRead(btnPin) == HIGH) {
    gears();
  }
}