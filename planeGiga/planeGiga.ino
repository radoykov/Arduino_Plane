#include <TaskScheduler.h>

bool setupEngines();
void loopEngines();
bool setupElements();
void loopElements();
bool setupClutch();
void loopClutch();
bool setupServo();
void loopServo();
bool setupWifi();
void loopWifi();

Scheduler runner;
Task tWifi(50, TASK_FOREVER, &loopWifi, &runner, false, &setupWifi);
Task tEngines(100, TASK_FOREVER, &loopEngines, &runner, false, &setupEngines);
Task tElements(100, TASK_FOREVER, &loopElements, &runner, false, &setupElements);
Task tClutch(20, TASK_FOREVER, &loopClutch, &runner, false, &setupClutch);
Task tServo(20, TASK_FOREVER, &loopServo, &runner, false, &setupServo);

void setup()
{
  tWifi.enable();
  tEngines.enable();
  tElements.enable();
  tClutch.enable();
  tServo.enable();
}

void loop()
{
  runner.execute();
}
