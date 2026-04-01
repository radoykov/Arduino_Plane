#pragma once
#include <Servo.h>

struct ServoState {
  int pitch = 0;
  int roll  = 0;
  int yaw   = 0;
};

struct EngineState {
  int left  = 0;
  int right = 0;
};

enum FunctionID {
  FN_NONE    = -1,
  RAMP_DOWN  =  0,
  RAMP_UP    =  1,
  GEAR_DOWN  =  2,
  GEAR_UP    =  3,
  CABIN_DOWN =  4,
  CABIN_UP   =  5
};

struct ElementState {
  bool gear   = false;
  bool ramp   = false;
  bool cabin  = false;
  FunctionID functionId = FN_NONE;
  int flap = 0;  // 0–4  (5 positions)
};

extern ServoState   wServo;
extern EngineState  wEngine;
extern ElementState wElement;

extern void setRamp();
extern void setBlades();
extern void setGears();
extern void setCabin();
extern void onMotorStopped();