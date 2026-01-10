//left and right engines
int leftEnable = 2;
int rightEnable = 3;
int leftIn1 = 22;
int leftIn2 = 23;
int rightIn1 = 24;
int rightIn2 = 25;

//potentiometer
int leftThrottle = A0;
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

  //set state
  digitalWrite(leftIn1, HIGH);
  digitalWrite(leftIn2, LOW);
  digitalWrite(rightIn1, LOW);
  digitalWrite(rightIn2, HIGH);
}

void loop() {
  leftValue = analogRead(leftThrottle);
  leftValue = map(leftValue, 0, 1023, 0, 255);
  analogWrite(leftEnable, leftValue);

  rightValue = analogRead(rightThrottle);
  rightValue = map(rightValue, 0, 1023, 70, 255);
  if (rightValue == 70) rightValue = 0;
  analogWrite(rightEnable, rightValue);

  Serial.print(leftValue);
  Serial.print(", ");
  Serial.println(rightValue);
}