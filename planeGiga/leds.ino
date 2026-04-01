const int NUM_LEDS = 3;
const int LED_PINS[NUM_LEDS] = { 54, 55, 56 };

const int PATTERN_A[][2] = {
  {  50, HIGH },
  { 100, LOW  },
  {  50, HIGH },
  {2500, LOW  },
};

const int PATTERN_B[][2] = {
  {  80, HIGH },
  { 150, LOW  },
  {  80, HIGH },
  { 150, LOW  },
  {  80, HIGH },
  {3000, LOW  },
};

const int PATTERN_C[][2] = {
  { 200, HIGH },
  {1000, LOW  },
};

const int *PATTERNS[NUM_LEDS]      = { (int*)PATTERN_A, (int*)PATTERN_B, (int*)PATTERN_C };
const int  PATTERN_STEPS[NUM_LEDS] = { 4, 6, 2 };
const int  PATTERN_COLS            = 2;

unsigned long previousMillis[NUM_LEDS] = { 0, 0, 0 };
int stepIdx[NUM_LEDS] = { 0, 0, 0 };

void setupLeds() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    // Apply the initial state of step 0 immediately
    digitalWrite(LED_PINS[i], PATTERNS[i][1]);
  }
}

void loopLeds() {
  unsigned long now = millis();

  for (int i = 0; i < NUM_LEDS; i++) {
    const int* pat      = PATTERNS[i];
    int        maxSteps = PATTERN_STEPS[i];

    // Duration of the CURRENT step
    int duration = pat[stepIdx[i] * PATTERN_COLS + 0];

    if (now - previousMillis[i] >= (unsigned long)duration) {
      previousMillis[i] = now;

      // Advance to next step
      stepIdx[i] = (stepIdx[i] + 1) % maxSteps;

      // Apply state of the NEW current step  ← was applying NEXT step before
      int ledState = pat[stepIdx[i] * PATTERN_COLS + 1];
      digitalWrite(LED_PINS[i], ledState);
    }
  }
}