#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include <Modulino.h>

#include <WiFiS3.h>
#include <WiFiUdp.h>
ModulinoKnob knob;
ArduinoLEDMatrix matrix;

// WiFi config
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";
const int UDP_PORT = 4210;
const char* GIGA_IP = "192.168.1.XXX";  // set GIGA's static IP here

WiFiUDP udp;
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL_MS = 100;  // send 10x per second

// Screens
const char* screenNames[] = { "Engines", "Gears", "Flaps", "Ramp", "Cabin" };
const int NUM_SCREENS = 5;
int currentScreen = 0;

int engineValues[2] = { 0, 0 };
int engineCursor = 0;

// UP/DOWN screens: Gears, Ramp, Cabin
// index maps: Gears=1, Ramp=3, Cabin=4
bool updownState[5] = { false, false, false, false, false };  // true=UP false=DOWN

// ── Flaps: 3 flap positions, each can be 0(small) or 1(big) ──
int flapSelected = 0;  // which flap is cursor on (0,1,2)
bool flapOpen[3] = { false, false, false };

// Click detection
bool lastPressed = false;
bool waitingSecondClick = false;
unsigned long firstClickTime = 0;
const unsigned long DOUBLE_CLICK_MS = 400;

int lastKnobPos = 0;

// WiFi connect


bool connectWiFi() {
  // Show "WiFi..." BEFORE blocking on connection
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(80);  // ← must be inside beginDraw/endDraw
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
    matrix.textScrollSpeed(80);  // ← same here
    matrix.textFont(Font_4x6);
    matrix.beginText(12, 1, 0xFFFFFF);
    matrix.println("WiFi OK!");
    matrix.endText(SCROLL_LEFT);
    matrix.endDraw();
    delay(1500);  // wait for scroll to finish
    return true;
  }

  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(80);  // ← and here
  matrix.textFont(Font_4x6);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println("No WiFi!");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
  delay(1500);  // wait for scroll to finish before retry
  return false;
}

void sendState() {
  char buf[64];
  snprintf(buf, sizeof(buf),
           "ENG:%d,%d|GEAR:%d|FLAP:%d,%d,%d|RAMP:%d|CABIN:%d",
           engineValues[0],
           engineValues[1],
           updownState[1] ? 1 : 0,  // Gears
           flapOpen[0] ? 1 : 0,
           flapOpen[1] ? 1 : 0,
           flapOpen[2] ? 1 : 0,
           updownState[3] ? 1 : 0,  // Ramp
           updownState[4] ? 1 : 0   // Cabin
  );

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

// ENGINES: left number       right number
void drawEngines() {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);

  // Left digit
  char d[2];
  d[1] = '\0';
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

  // Cursor dot under active engine
  int cx = (engineCursor == 0) ? 1 : 9;
  matrix.point(cx, 7);
  matrix.point(cx + 1, 7);

  matrix.endDraw();
}

// UP/DOWN: big UP or DOWN text
// UP   → arrow pointing up   (chevron shape)
// DOWN → arrow pointing down (chevron shape)
void drawUpDown(int screenIdx) {
  bool isUp = updownState[screenIdx];
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);

  if (isUp) {
    // Draw UP arrow chevron + "U" label
    // Arrow tip at top center (col 5-6, row 0)
    // matrix.point(5, 0);
    // matrix.point(6, 0);
    // // Wings spread
    // matrix.point(3, 2);
    // matrix.point(4, 1);
    // matrix.point(7, 1);
    // matrix.point(8, 2);
    // // Shaft
    // matrix.point(5, 1);
    // matrix.point(6, 1);
    // matrix.point(5, 2);
    // matrix.point(6, 2);
    // matrix.point(5, 3);
    // matrix.point(6, 3);
    // Label "UP" small
    matrix.textFont(Font_4x6);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.print("UP");
    matrix.endText();
  } else {
    // Draw DOWN arrow chevron + "DN" label
    // matrix.point(5, 6);
    // matrix.point(6, 6);
    // matrix.point(3, 4);
    // matrix.point(4, 5);
    // matrix.point(7, 5);
    // matrix.point(8, 4);
    // matrix.point(5, 5);
    // matrix.point(6, 5);
    // matrix.point(5, 4);
    // matrix.point(6, 4);
    // matrix.point(5, 3);
    // matrix.point(6, 3);
    matrix.textFont(Font_4x6);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.print("LOW");
    matrix.endText();
  }

  matrix.endDraw();
}

// FLAPS: 3 flap shapes, selected one is bigger
void drawFlaps() {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);

  int flapX[3] = { 1, 5, 9 };

  for (int i = 0; i < 3; i++) {
    int x = flapX[i];
    bool isSelected = (i == flapSelected);

    if (flapOpen[i]) {
      // Big flap: tall bar from row 1 to row 6
      for (int r = 1; r <= 6; r++) {
        matrix.point(x, r);
        matrix.point(x + 1, r);
      }
    } else {
      // Small flap: short bar rows 3-5
      for (int r = 3; r <= 5; r++) {
        matrix.point(x, r);
        matrix.point(x + 1, r);
      }
    }

    // Cursor indicator: dot at row 7 under selected flap
    if (isSelected) {
      matrix.point(x, 7);
      matrix.point(x + 1, 7);
    }
  }

  matrix.endDraw();
}

// Master draw dispatcher
void drawCurrent() {
  switch (currentScreen) {
    case 0: drawEngines(); break;  // Engines
    case 1: drawUpDown(1); break;  // Gears
    case 2: drawFlaps(); break;    // Flaps
    case 3: drawUpDown(3); break;  // Ramp
    case 4: drawUpDown(4); break;  // Cabin
  }
}

void onSingleClick() {
  switch (currentScreen) {
    case 0:
      // Engines: move cursor between left and right engine
      engineCursor = 1 - engineCursor;
      break;
    case 1:
    case 3:
    case 4:
      // Gears / Ramp / Cabin: toggle UP/DOWN
      updownState[currentScreen] = !updownState[currentScreen];
      break;
    case 2:
      // Flaps: move cursor to next flap
      flapSelected = (flapSelected + 1) % 3;
      break;
  }
  drawCurrent();
}

void onKnobChange(int delta) {
  switch (currentScreen) {
    case 0:
      // Engines: adjust active engine value
      engineValues[engineCursor] = constrain(engineValues[engineCursor] + delta, 0, 9);
      break;
    case 2:
      // Flaps: knob opens/closes selected flap
      flapOpen[flapSelected] = (delta > 0);
      break;
  }
  drawCurrent();
}

void setup() {
  Serial.begin(9600);
  Modulino.begin();
  knob.begin();
  matrix.begin();

  while (!connectWiFi())
    ;


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
      // DOUBLE CLICK → next screen
      waitingSecondClick = false;
      currentScreen = (currentScreen + 1) % NUM_SCREENS;
      drawScreenName(screenNames[currentScreen]);
      drawCurrent();
    } else {
      waitingSecondClick = true;
      firstClickTime = now;
    }
  }

  // Single click timeout confirmed
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

  if (WiFi.status() == WL_CONNECTED && now - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = now;
    sendState();
  }

  delay(50);
}