bool setupEngines();   void loopEngines();
bool setupElements();  void loopElements();
bool setupServo();     void loopServo();
bool setupWifi();      void loopWifi();

struct Task {
  void         (*loop)();
  bool         (*setup)();
  unsigned long intervalMs;
  unsigned long lastRun;
};

// Tune intervals here
Task tasks[] = {
  { loopWifi,     setupWifi,     10,   0 },  // WiFi     — 10 ms  (maximum responsiveness)
  { loopServo,    setupServo,    20,  0 },  // Servo    — 20 ms (50 Hz smooth)
  { loopEngines,  setupEngines,  50,  0 },  // Engines  — 50 ms
  { loopElements, setupElements, 20,  0 },  // Elements — 20 ms (motor tick)
};
const int NUM_TASKS = sizeof(tasks) / sizeof(tasks[0]);

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_TASKS; i++) {
    if (tasks[i].setup) tasks[i].setup();
    tasks[i].lastRun = millis();
  }
}

void loop() {
  unsigned long now = millis();
  for (int i = 0; i < NUM_TASKS; i++) {
    if (now - tasks[i].lastRun >= tasks[i].intervalMs) {
      tasks[i].lastRun = now;
      tasks[i].loop();
    }
  }
}
