//left engine
int leftEnable = 2;
int leftIn1 = 22;
int leftIn2 = 23;
int leftToggle = 24;
int leftLed = 25;
int leftThrottle = A0;

//right engine
int rightEnable = 3;
int rightIn1 = 26;
int rightIn2 = 27;
int rightToggle = 28;
int rightLed = 29;
int rightThrottle = A1;

int leftValue = 0;
int rightValue = 0;

void setup() {
  Serial.begin(9600);
  // Set motors to outputs
  pinMode(leftEnable, OUTPUT);
  pinMode(rightEnable, OUTPUT);

  pinMode(leftIn1, OUTPUT);
  pinMode(leftIn2, OUTPUT);
  pinMode(rightIn1, OUTPUT);
  pinMode(rightIn2, OUTPUT);

  //set states
  digitalWrite(leftIn1, HIGH);
  digitalWrite(leftIn2, LOW);
  digitalWrite(rightIn1, LOW);
  digitalWrite(rightIn2, HIGH);
}

void loop() {
  //left engine
  leftValue = analogRead(leftThrottle);
  leftValue = map(leftValue, 0, 1023, 80, 255);  //80 is minimum thrust

  if (digitalRead(leftToggle) == HIGH) {
    digitalWrite(leftLed, HIGH);
  } else {
    leftValue = 0;
    digitalWrite(leftLed, LOW);
  }
  analogWrite(leftEnable, leftValue);

  //right engine
  rightValue = analogRead(rightThrottle);
  rightValue = map(rightValue, 0, 1023, 80, 255);  //80 is minimum thrust

  if (digitalRead(rightToggle) == HIGH) {
    digitalWrite(rightLed, HIGH);
  } else {
    rightValue = 0;
    digitalWrite(rightLed, LOW);
  }
  analogWrite(rightEnable, rightValue);
  // Serial.println(rightValue);
}