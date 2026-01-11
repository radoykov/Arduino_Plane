#include <Servo.h>

const int btnPin = 53;
const int servoPin = 4;
Servo myservo;


void setup() {
  Serial.begin(9600);
  pinMode(btnPin, INPUT);
  myservo.attach(servoPin);
  myservo.write(53);
}
int arr[] = { 53, 23, 53, 73};
int i = 0;
void loop() {
  if (digitalRead(btnPin) == HIGH) {
    myservo.write(arr[i]);
    delay(200);
    if (i < 3) {
      i++;
    } else {
      i = 0;
    }
  }
  delay(20);
   //35 delta; min-23 max-  mid = 53 ///MID TO POSITION 30
}