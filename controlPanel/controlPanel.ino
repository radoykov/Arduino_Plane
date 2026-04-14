#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "1_globals.h"
#include <Modulino.h>
#include <WiFiS3.h>

const char *WIFI_SSID = "ArduinoGigaWifi";
const char *WIFI_PASS = "pesho123";
const int UDP_PORT = 4210;
const char *GIGA_IP = "192.168.3.1";

WiFiUDP udp;
ModulinoMovement movement;
ModulinoKnob knob;
ArduinoLEDMatrix matrix;

const int PIN_SW_GEAR = 11;
const int PIN_SW_RAMP = 13;
const int PIN_SW_CABIN = 10;

float alpha;
unsigned long lastPitchTime, lastRollTime, lastYawTime;

const char *screenNames[] = {"Engines", "Gears", "Flaps", "Ramp", "Cabin"};
const int NUM_SCREENS = 5;
int currentScreen = 0;

int engineValues[2] = {0, 0};
int engineCursor = 0;
bool updownState[5] = {false, false, false, true, false};
int flapSelected = 0;

int prevEngineValues[2] = {-1, -1};
bool prevUpdown[5] = {false, false, false, true, false};
int prevFlapSelected = 0;

bool lastPressed = false;
int pendingClicks = 0;
unsigned long lastClickTime = 0;
const unsigned long CLICK_WIN = 400;

unsigned long lastImuSendTime = 0;
const unsigned long IMU_SEND_INTERVAL = 100;

int lastKnobPos = 0;

void readSwitches()
{
  updownState[1] = digitalRead(PIN_SW_GEAR)  == LOW;
  updownState[3] = digitalRead(PIN_SW_RAMP)  == HIGH;
  updownState[4] = digitalRead(PIN_SW_CABIN) == LOW;
}

void matrixScroll(const char *msg)
{
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

void drawEngines()
{
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_4x6);
  char d[2] = {0, '\0'};
  d[0] = '0' + engineValues[0];
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

void drawUpDown(int idx)
{
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(updownState[idx] ? "UP" : "DN");
  matrix.endText();
  matrix.endDraw();
}

void drawFlaps()
{
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_4x6);
  char d[2] = {(char)('0' + flapSelected + 1), '\0'};
  matrix.beginText(4, 1, 0xFFFFFF);
  matrix.print(d);
  matrix.endText();
  matrix.point(4, 7);
  matrix.point(5, 7);
  matrix.endDraw();
}

void drawCurrent()
{
  switch (currentScreen)
  {
  case 0: drawEngines();  break;
  case 1: drawUpDown(1);  break;
  case 2: drawFlaps();    break;
  case 3: drawUpDown(3);  break;
  case 4: drawUpDown(4);  break;
  }
}

bool connectWiFi()
{
  matrixScroll("WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    udp.begin(UDP_PORT);
    matrixScroll("OK!");
    delay(1000);
    return true;
  }
  matrixScroll("Fail!");
  delay(1000);
  return false;
}

void sendPacket(const char *buf)
{
  udp.beginPacket(GIGA_IP, UDP_PORT);
  udp.write((const uint8_t *)buf, strlen(buf));
  udp.endPacket();
}

void sendPitchRollYaw()
{
  movement.update();
  char buf[64];
  snprintf(buf, sizeof(buf), "PITCH:%d|ROLL:%d|YAW:%d",
           (int)getPitchAngle(), (int)getRollAngle(), (int)getYawAngle());
  sendPacket(buf);
}

void sendEngines()
{
  char buf[32];
  snprintf(buf, sizeof(buf), "ENG:%d,%d", engineValues[0], engineValues[1]);
  sendPacket(buf);
}

void sendGear()
{
  char buf[16];
  snprintf(buf, sizeof(buf), "GEAR:%d", updownState[1] ? 1 : 0);
  sendPacket(buf);
}

void sendFlap()
{
  char buf[16];
  snprintf(buf, sizeof(buf), "FLAP:%d", flapSelected);
  sendPacket(buf);
}

void sendRamp()
{
  char buf[16];
  snprintf(buf, sizeof(buf), "RAMP:%d", updownState[3] ? 1 : 0);
  sendPacket(buf);
}

void sendCabin()
{
  char buf[16];
  snprintf(buf, sizeof(buf), "CABIN:%d", updownState[4] ? 1 : 0);
  sendPacket(buf);
}

void sendChangedEvents()
{
  if (engineValues[0] != prevEngineValues[0] || engineValues[1] != prevEngineValues[1])
  {
    prevEngineValues[0] = engineValues[0];
    prevEngineValues[1] = engineValues[1];
    sendEngines();
  }
  if (updownState[1] != prevUpdown[1])
  {
    prevUpdown[1] = updownState[1];
    sendGear();
  }
  if (updownState[3] != prevUpdown[3])
  {
    prevUpdown[3] = updownState[3];
    sendRamp();
  }
  if (updownState[4] != prevUpdown[4])
  {
    prevUpdown[4] = updownState[4];
    sendCabin();
  }
  if (flapSelected != prevFlapSelected)
  {
    prevFlapSelected = flapSelected;
    sendFlap();
  }
}

void onSingleClick()
{
  switch (currentScreen)
  {
  case 0: engineCursor = 1 - engineCursor;   break;
  case 1: updownState[1] = !updownState[1];  break;
  case 3: updownState[3] = !updownState[3];  break;
  case 4: updownState[4] = !updownState[4];  break;
  }
  drawCurrent();
}

void onDoubleClick()
{
  currentScreen = (currentScreen + 1) % NUM_SCREENS;
  matrixScroll(screenNames[currentScreen]);
  drawCurrent();
}

void onTripleClick()
{
  currentScreen = (currentScreen - 1 + NUM_SCREENS) % NUM_SCREENS;
  matrixScroll(screenNames[currentScreen]);
  drawCurrent();
}

void dispatchClicks(int count)
{
  if (count == 1)      onSingleClick();
  else if (count == 2) onDoubleClick();
  else if (count >= 3) onTripleClick();
}

bool setupMovement()
{
  Modulino.begin();
  movement.begin();
  lastPitchTime = lastRollTime = lastYawTime = micros();
  alpha = 0.90;
  calibrateYaw();
  return true;
}

void setup()
{
  Serial.begin(115200);
  setupMovement();
  knob.begin();
  matrix.begin();
  pinMode(PIN_SW_GEAR,  INPUT_PULLUP);
  pinMode(PIN_SW_RAMP,  INPUT_PULLUP);
  pinMode(PIN_SW_CABIN, INPUT_PULLUP);
  while (!connectWiFi());
  lastKnobPos = knob.get();
  matrixScroll(screenNames[currentScreen]);
  drawCurrent();
  sendFlap();
}

void loop()
{
  bool pressed = knob.isPressed();
  int knobPos = knob.get();
  unsigned long now = millis();

  readSwitches();

  if (!pressed && lastPressed)
  {
    pendingClicks++;
    lastClickTime = now;
  }
  if (pendingClicks > 0 && (now - lastClickTime) >= CLICK_WIN)
  {
    dispatchClicks(pendingClicks);
    pendingClicks = 0;
  }
  lastPressed = pressed;

  if (knobPos != lastKnobPos)
  {
    int delta = knobPos - lastKnobPos;
    lastKnobPos = knobPos;
    switch (currentScreen)
    {
    case 0: engineValues[engineCursor] = constrain(engineValues[engineCursor] + delta, 0, 9); break;
    case 2: flapSelected = constrain(flapSelected + delta, 0, 3); break;
    }
    drawCurrent();
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    if (now - lastImuSendTime >= IMU_SEND_INTERVAL)
    {
      lastImuSendTime = now;
      sendPitchRollYaw();
    }
    sendChangedEvents();
  }
}