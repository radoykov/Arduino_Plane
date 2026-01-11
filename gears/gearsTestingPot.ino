#include <Servo.h>

const int btnPin = 53;
const int servoPin = 4;
const int potPin = A7;
Servo myservo;

int value = 0;

void setup(){
  Serial.begin(9600);
  pinMode(btnPin, INPUT);
  pinMode(potPin, INPUT);
  myservo.attach(servoPin);
}

void loop(){
  value = analogRead(potPin);
  value = map(value, 0, 1023, 0, 180); 
  myservo.write(value);
  Serial.println(value);  //35 delta; min-23 max-  mid = 53 ///MID TO POSITION 30
  delay(5);

}