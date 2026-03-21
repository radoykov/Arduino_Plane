#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include <Modulino.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>

ModulinoKnob knob;
ArduinoLEDMatrix matrix;

// ── WiFi config ───────────────────────────────────────────
const char* WIFI_SSID = "YOUR_SSID";      // <-- fill in
const char* WIFI_PASS = "YOUR_PASSWORD";  // <-- fill in
const int UDP_PORT = 4210;
const char* GIGA_IP = "192.168.1.100";  // <-- static IP you set on GIGA

WiFiUDP udp;
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL_MS = 100;

// ── Screens ───────────────────────────────────────────────
const char* screenNames[] = { "Engines", "Gears", "Flaps", "Ramp", "Cabin" };
const int NUM_SCREENS = 5;
int currentScreen = 0;

// ── Engines ───────────────────────────────────────────────
int engineValues[2] = { 0, 0 };
int engineCursor = 0;

// ── UP/DOWN screens: index 1=Gears, 3=Ramp, 4=Cabin ─────
bool updownState[5] = { false, false, false, false, false };

// ── Flaps ─────────────────────────────────────────────────
int flapSelected = 0;
bool flapOpen[3] = { false, false, false };

// ── Click detection ───────────────────────────────────────
bool lastPressed = false;
bool waitingSecondClick = false;
unsigned long firstClickTime = 0;
const unsigned long DOUBLE_CLICK_MS = 400;

int lastKnobPos = 0;

bool connectWiFi() {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(80);
  matrix.textFont(Font_4x6);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println("WiFi...");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    udp.begin(UDP_PORT);
    matrix.beginDraw();
    matrix.clear();
    matrix.stroke(0xFFFFFFFF);
    matrix.textScrollSpeed(80);
    matrix.textFont(Font_4x6);
    matrix.beginText(12, 1, 0xFFFFFF);
    matrix.println("WiFi OK!");
    matrix.endText(SCROLL_LEFT);
    matrix.endDraw();
    delay(1500);
    return true;
  }

  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(80);
  matrix.textFont(Font_4x6);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println("No WiFi!");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
  delay(1500);
  return false;
}

void sendState() {
  char buf[64];
  snprintf(buf, sizeof(buf),
           "ENG:%d,%d|GEAR:%d|FLAP:%d,%d,%d|RAMP:%d|CABIN:%d",
           engineValues[0],
           engineValues[1],
           updownState[1] ? 1 : 0,
           flapOpen[0] ? 1 : 0,
           flapOpen[1] ? 1 : 0,
           flapOpen[2] ? 1 : 0,
           updownState[3] ? 1 : 0,
           updownState[4] ? 1 : 0);
  udp.beginPacket(GIGA_IP, UDP_PORT);
  udp.write((uint8_t*)buf, strlen(buf));
  udp.endPacket();
}

void drawScreenName(const char* name) {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(80);
  matrix.textFont(Font_4x6);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println(name);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

void drawEngines() {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);

  char d[2];
  d[1] = '\0';

  // Left digit
  d[0] = '0' + engineValues[0];
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(d);
  matrix.endText();

  // Right digit
  d[0] = '0' + engineValues[1];
  matrix.beginText(8, 1, 0xFFFFFF);
  matrix.print(d);
  matrix.endText();

  // Cursor dot
  int cx = (engineCursor == 0) ? 1 : 9;
  matrix.point(cx, 7);
  matrix.point(cx + 1, 7);

  matrix.endDraw();
}

void drawUpDown(int screenIdx) {
  bool isUp = updownState[screenIdx];
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(isUp ? "UP" : "LOW");
  matrix.endText();
  matrix.endDraw();
}

void drawFlaps() {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);

  int flapX[3] = { 1, 5, 9 };

  for (int i = 0; i < 3; i++) {
    int x = flapX[i];

    if (flapOpen[i]) {
      for (int r = 1; r <= 6; r++) {
        matrix.point(x, r);
        matrix.point(x + 1, r);
      }
    } else {
      for (int r = 3; r <= 5; r++) {
        matrix.point(x, r);
        matrix.point(x + 1, r);
      }
    }

    if (i == flapSelected) {
      matrix.point(x, 7);
      matrix.point(x + 1, 7);
    }
  }

  matrix.endDraw();
}

void drawCurrent() {
  switch (currentScreen) {
    case 0: drawEngines(); break;
    case 1: drawUpDown(1); break;
    case 2: drawFlaps(); break;
    case 3: drawUpDown(3); break;
    case 4: drawUpDown(4); break;
  }
}

bool flapChosen = false;
void onSingleClick() {
  switch (currentScreen) {
    case 0:
      engineCursor = 1 - engineCursor;
      break;
    case 1:
    case 3:
    case 4:
      updownState[currentScreen] = !updownState[currentScreen];
      break;
    case 2:
      // If a flap was opened on current position, close all others before moving
      if (flapChosen) {
        for (int i = 0; i < 3; i++) {
          if (i != flapSelected) flapOpen[i] = false;
        }
        flapChosen = false;
      }
      flapSelected = (flapSelected + 1) % 3;
      break;
  }

  drawCurrent();
}

void onKnobChange(int delta) {
  switch (currentScreen) {
    case 0:
      engineValues[engineCursor] = constrain(engineValues[engineCursor] + delta, 0, 9);
      break;
    case 2:
      flapOpen[flapSelected] = (delta > 0);
      flapChosen = (delta > 0);
      break;
  }
  drawCurrent();
}

void setup() {
  Serial.begin(9600);
  Modulino.begin();
  knob.begin();
  matrix.begin();

  // while (!connectWiFi());

  lastKnobPos = knob.get();
  drawScreenName(screenNames[currentScreen]);
  drawCurrent();
}

void loop() {
  bool pressed = knob.isPressed();
  int knobPos = knob.get();
  unsigned long now = millis();

  // Click detection
  if (!pressed && lastPressed) {
    if (waitingSecondClick && (now - firstClickTime) < DOUBLE_CLICK_MS) {
      // Double click → next screen
      waitingSecondClick = false;
      currentScreen = (currentScreen + 1) % NUM_SCREENS;
      drawScreenName(screenNames[currentScreen]);
      drawCurrent();
    } else {
      waitingSecondClick = true;
      firstClickTime = now;
    }
  }

  // Single click timeout
  if (waitingSecondClick && (now - firstClickTime) >= DOUBLE_CLICK_MS) {
    waitingSecondClick = false;
    onSingleClick();
  }

  lastPressed = pressed;

  // Knob
  if (knobPos != lastKnobPos) {
    int delta = knobPos - lastKnobPos;
    lastKnobPos = knobPos;
    onKnobChange(delta);
  }

  // Send WiFi state
  if (WiFi.status() == WL_CONNECTED && now - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = now;
    sendState();
  }

  delay(50);
}
