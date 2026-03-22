#pragma once
#include <Servo.h>

// Servo state
struct ServoState {
  int pitch = 0;  
  int roll  = 0;
  int yaw   = 0;
};

// Engine state
struct EngineState {
  int  left  = 0;  // 0–255  PWM value
  int  right = 0;  // 0–255  PWM value
};

// Element (gear / cabin / ramp) state
struct ElementState {
  bool gear  = false;  // false = down,  true = up
  bool ramp  = false;  // false = closed, true = open
  bool cabin = false;  // false = closed, true = open
  int flap; // 1 , 2 , 3
};

// Single global instances
extern ServoState   wServo;
extern EngineState  wEngine;
extern ElementState wElement;

extern void setRamp();
extern void setBlades();
extern void setGears();
extern void setCabin();
