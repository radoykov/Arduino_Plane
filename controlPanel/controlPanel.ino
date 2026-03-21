#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "1_globals.h"
#include <Modulino.h>
#include <WiFiS3.h>

const char* WIFI_SSID = "ArduinoGigaWifi";
const char* WIFI_PASS = "pesho123";
const int UDP_PORT = 4210;
const char* GIGA_IP = "192.168.3.1";  // default AP address for Giga

WiFiUDP udp;

ModulinoMovement movement;
unsigned long lastPitchTime, lastRollTime, lastYawTime;
float alpha;

bool setupMovement() {
  Modulino.begin();
  movement.begin();
  lastPitchTime = lastRollTime = lastYawTime = micros();
  alpha = 0.95;
  calibrateYaw();
  return true;
}

ModulinoKnob knob;
ArduinoLEDMatrix matrix;

const char* screenNames[] = { "Engines", "Gears", "Flaps", "Ramp", "Cabin" };
const int NUM_SCREENS = 5;
int currentScreen = 0;

int engineValues[2] = { 0, 0 };
int engineCursor = 0;
bool updownState[5] = { false, false, false, false, false };
int flapSelected = 0;
bool flapOpen[3] = { false, false, false };
bool flapChosen = false;

int prevEngineValues[2] = { -1, -1 };
bool prevUpdown[5] = { false, false, false, false, false };
bool prevFlapOpen[3] = { false, false, false };

bool lastPressed = false;
bool waitingSecondClick = false;
unsigned long firstClickTime = 0;
const unsigned long DOUBLE_CLICK_MS = 400;
int lastKnobPos = 0;

unsigned long lastImuSendTime = 0;
const unsigned long IMU_SEND_INTERVAL = 100;  // ms

void matrixScroll(const char* msg) {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(80);
  matrix.textFont(Font_4x6);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.println(msg);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

bool connectWiFi() {
  matrixScroll("WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    udp.begin(UDP_PORT);
    matrixScroll("WiFi OK!");
    delay(1500);
    return true;
  }
  matrixScroll("No WiFi!");
  delay(1500);
  return false;
}

void sendPacket(const char* buf) {
  udp.beginPacket(GIGA_IP, UDP_PORT);
  udp.write((const uint8_t*)buf, strlen(buf));
  udp.endPacket();
}

void sendPitch() {
  char buf[32];
  snprintf(buf, sizeof(buf), "PITCH:%.2f", getPitchAngle());
  sendPacket(buf);
}
void sendRoll() {
  char buf[32];
  snprintf(buf, sizeof(buf), "ROLL:%.2f", getRollAngle());
  sendPacket(buf);
}
void sendYaw() {
  char buf[32];
  snprintf(buf, sizeof(buf), "YAW:%.2f", getYawAngle());
  sendPacket(buf);
}

void sendEngines() {
  char buf[32];
  snprintf(buf, sizeof(buf), "ENG:%d,%d", engineValues[0], engineValues[1]);
  sendPacket(buf);
}
void sendGear() {
  char buf[16];
  snprintf(buf, sizeof(buf), "GEAR:%d", updownState[1] ? 1 : 0);
  sendPacket(buf);
}
void sendFlap() {
  char buf[24];
  snprintf(buf, sizeof(buf), "FLAP:%d,%d,%d",
           flapOpen[0] ? 1 : 0, flapOpen[1] ? 1 : 0, flapOpen[2] ? 1 : 0);
  sendPacket(buf);
}
void sendRamp() {
  char buf[16];
  snprintf(buf, sizeof(buf), "RAMP:%d", updownState[3] ? 1 : 0);
  sendPacket(buf);
}
void sendCabin() {
  char buf[16];
  snprintf(buf, sizeof(buf), "CABIN:%d", updownState[4] ? 1 : 0);
  sendPacket(buf);
}

void sendChangedEvents() {
  if (engineValues[0] != prevEngineValues[0] || engineValues[1] != prevEngineValues[1]) {
    prevEngineValues[0] = engineValues[0];
    prevEngineValues[1] = engineValues[1];
    sendEngines();
  }
  if (updownState[1] != prevUpdown[1]) {
    prevUpdown[1] = updownState[1];
    sendGear();
  }
  if (flapOpen[0] != prevFlapOpen[0] || flapOpen[1] != prevFlapOpen[1] || flapOpen[2] != prevFlapOpen[2]) {
    prevFlapOpen[0] = flapOpen[0];
    prevFlapOpen[1] = flapOpen[1];
    prevFlapOpen[2] = flapOpen[2];
    sendFlap();
  }
  if (updownState[3] != prevUpdown[3]) {
    prevUpdown[3] = updownState[3];
    sendRamp();
  }
  if (updownState[4] != prevUpdown[4]) {
    prevUpdown[4] = updownState[4];
    sendCabin();
  }
}

void drawScreenName(const char* name) {
  matrixScroll(name);
}

void drawEngines() {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  char d[2];
  d[1] = '\0';
  d[0] = '0' + engineValues[0];
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(d);
  matrix.endText();
  d[0] = '0' + engineValues[1];
  matrix.beginText(8, 1, 0xFFFFFF);
  matrix.print(d);
  matrix.endText();
  int cx = (engineCursor == 0) ? 1 : 9;
  matrix.point(cx, 7);
  matrix.point(cx + 1, 7);
  matrix.endDraw();
}
void drawUpDown(int idx) {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(updownState[idx] ? "UP" : "DN");
  matrix.endText();
  matrix.endDraw();
}
void drawFlaps() {
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  char d[2];
  d[0] = '0' + flapSelected + 1;
  d[1] = '\0';
  matrix.textFont(Font_4x6);
  matrix.beginText(4, 1, 0xFFFFFF);
  matrix.print(d);
  matrix.endText();
  matrix.point(4, 7);
  matrix.point(5, 7);
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

void onSingleClick() {
  switch (currentScreen) {
    case 0: engineCursor = 1 - engineCursor; break;
    case 1:
    case 3:
    case 4:
      updownState[currentScreen] = !updownState[currentScreen];
      break;
    case 2:
      if (flapChosen) {
        for (int i = 0; i < 3; i++)
          if (i != flapSelected) flapOpen[i] = false;
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
      flapSelected = constrain(flapSelected + delta, 0, 2);
      flapOpen[0] = flapOpen[1] = flapOpen[2] = false;
      flapOpen[flapSelected] = true;
      flapChosen = true;
      break;
  }
  drawCurrent();
}

void setup() {
  Serial.begin(115200);
  setupMovement();
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

  if (!pressed && lastPressed) {
    if (waitingSecondClick && (now - firstClickTime) < DOUBLE_CLICK_MS) {
      waitingSecondClick = false;
      currentScreen = (currentScreen + 1) % NUM_SCREENS;
      drawScreenName(screenNames[currentScreen]);
      drawCurrent();
    } else {
      waitingSecondClick = true;
      firstClickTime = now;
    }
  }
  if (waitingSecondClick && (now - firstClickTime) >= DOUBLE_CLICK_MS) {
    waitingSecondClick = false;
    onSingleClick();
  }
  lastPressed = pressed;

  if (knobPos != lastKnobPos) {
    int delta = knobPos - lastKnobPos;
    lastKnobPos = knobPos;
    onKnobChange(delta);
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (now - lastImuSendTime >= IMU_SEND_INTERVAL) {
      lastImuSendTime = now;
      sendPitch();
      sendRoll();
      sendYaw();
    }
    sendChangedEvents();
  }

  delay(20);
}
