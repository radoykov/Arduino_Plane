#include <Preferences.h>

// --- PIN DEFINITIONS ---
const int stepPin = 2; 
const int dirPin = 3;

// --- MEMORY & POSITIONING ---
Preferences storage;
long currentStepPos = 0;   
const int stepsPerRev = 800; 

void setupStepper() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // 1. Initialize Permanent Memory
  storage.begin("stepper-data", false);
  
  // 2. Load the position where we last unplugged
  currentStepPos = storage.getLong("lastPos", 0);
  
  Serial.print("Stepper Initialized. Starting at Step: ");
  Serial.println(currentStepPos);
}

void updateStepperLogic() {
  // 1. Get the raw Yaw from your existing controller variable
  // Ensure 'wServo.yaw' matches the variable name in your other files
  float incomingYaw = wServo.yaw; 

  // 2. Map Angle to Steps
  // If your controller sends -45 to 45 degrees, map that to steps.
  // At 800 steps/rev, 45 degrees is 100 steps ( (45/360)*800 )
  long targetStep = map(incomingYaw, -45, 45, -100, 100);

  // 3. Move toward target
  if (currentStepPos != targetStep) {
    bool movingForward = (targetStep > currentStepPos);
    digitalWrite(dirPin, movingForward ? HIGH : LOW);

    // Pulse
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(600); // Speed of the move
    digitalWrite(stepPin, LOW);
    delayMicroseconds(600);

    // Update position tracker
    if (movingForward) currentStepPos++;
    else currentStepPos--;
  }

  // 4.save
  // Only save to Flash if the motor has reached its target and stayed still for 3 seconds
  static unsigned long lastMoveTime = 0;
  static long lastSavedPos = -99999; // Dummy value

  if (currentStepPos != targetStep) {
    lastMoveTime = millis(); // Reset timer while moving
  }

  if (millis() - lastMoveTime > 3000 && currentStepPos != lastSavedPos) {
    storage.putLong("lastPos", currentStepPos);
    lastSavedPos = currentStepPos;
    Serial.println("Position saved to Giga Flash.");
  }
}