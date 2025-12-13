#pragma once
#include "Modulino.h"

extern ModulinoMovement movement;   
extern unsigned long lastPitchTime;
extern unsigned long lastRollTime;
extern unsigned long lastYawTime;   
extern float alpha;   
float getPitchAngle();
float getRollAngle();
float getYawAngle();
void calibrateYaw();