#include <TaskScheduler.h>

// Forward declare all module callbacks
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

Scheduler runner;
Task tEngines(100, TASK_FOREVER, &loopEngines, &runner, false, &setupEngines);
Task tElements(100, TASK_FOREVER, &loopElements, &runner, false, &setupElements);
Task tClutch(20, TASK_FOREVER, &loopClutch, &runner, false, &setupClutch);
Task tServo(20, TASK_FOREVER, &loopServo, &runner, false, &setupServo);
Task tMovement(20, TASK_FOREVER, &loopMovement, &runner, false, &setupMovement);

void setup()
{
  tEngines.enable();
  tElements.enable();
  tClutch.enable();
  tServo.enable();
  tMovement.enable();
}

void loop()
{
  runner.execute();
}
