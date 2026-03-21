#include <TaskScheduler.h>

bool setupEngines();
void loopEngines();
bool setupElements();
void loopElements();
bool setupClutch();
void loopClutch();
bool setupServo();
void loopServo();
bool setupMovement();
void loopMovement();
bool setupWifi();
void loopWifi();

Scheduler runner;
Task tEngines(100, TASK_FOREVER, &loopEngines, &runner, false, &setupEngines);
Task tElements(100, TASK_FOREVER, &loopElements, &runner, false, &setupElements);
Task tClutch(20, TASK_FOREVER, &loopClutch, &runner, false, &setupClutch);
Task tServo(20, TASK_FOREVER, &loopServo, &runner, false, &setupServo);
Task tMovement(20, TASK_FOREVER, &loopMovement, &runner, false, &setupMovement);
Task tWifi(50, TASK_FOREVER, &loopWifi, &runner, false, &setupWifi);

void setup() {
  Serial.begin(9600);
  tEngines.enable();
  tElements.enable();
  tClutch.enable();
  tServo.enable();
  tMovement.enable();
  tWifi.enable();
}

void loop() {
  runner.execute();
}
