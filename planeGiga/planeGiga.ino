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
void setupStepper();
void updateStepperLogic();

void setup() {
  Serial.begin(115200);

  setupWifi();
  setupStepper();
  setupServo();
  setupEngines();
  setupElements();
  setupFlaps();
  setupLeds();
}

void loop() {
  loopWifi();
  updateStepperLogic();
  loopServo();
  loopEngines();
  loopElements();
  loopFlaps();
  loopLeds();
}
