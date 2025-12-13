#include <Servo.h>

Servo myservo;
int servoPin = 6;
int potPin = A0;
int value = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(servoPin);
}

void loop() {
  value = analogRead(potPin);
  value = map(value, 0, 1023, 0, 180);
  myservo.write(value);
  Serial.println(value);
}