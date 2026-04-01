void setupEngines();
void loopEngines();
void setupElements();
void loopElements();
void setupServo();
void loopServo();
void setupWifi();
void loopWifi();
void setupLeds();
void loopLeds();
void setupFlaps();
void loopFlaps();
//void setupClutch();   

void setup() {
  Serial.begin(115200);

  setupWifi();
  setupServo();
  //setupClutch();    
  setupEngines();
  setupElements();
  setupFlaps();
  setupLeds();
}

void loop() {
  loopWifi();
  loopServo();
  loopEngines();
  loopElements();  
  loopFlaps();
  loopLeds();
}