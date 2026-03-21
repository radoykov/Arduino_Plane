
#include <WiFi.h>       // ← Giga uses WiFi.h  (NOT WiFiS3.h)
#include <WiFiUdp.h>
#include "wifi_state.h"

const char* AP_SSID = "ArduinoGigaWifi";
const char* AP_PASS = "pesho123";
const int   UDP_PORT = 4210;

WiFiUDP udp;
char    packetBuf[64];

// ── received state ──────────────────────────────────────────
int   wEng[2]  = {0, 0};
bool  wGear    = false;
bool  wFlap[3] = {false, false, false};
bool  wRamp    = false;
bool  wCabin   = false;
float wPitch   = 0.0f;
float wRoll    = 0.0f;
float wYaw     = 0.0f;

// ── previous state for edge detection ───────────────────────
bool prevGear  = false;
bool prevRamp  = false;
bool prevCabin = false;

// ────────────────────────────────────────────────────────────
//  PARSE — each packet has exactly one key, so one if-chain
// ────────────────────────────────────────────────────────────
void parsePacket(char* data) {
  int v;

  if (strncmp(data, "PITCH:", 6) == 0) {
    sscanf(data + 6, "%f", &wPitch);
    Serial.print("PITCH="); Serial.println(wPitch, 2);
    return;
  }
  if (strncmp(data, "ROLL:", 5) == 0) {
    sscanf(data + 5, "%f", &wRoll);
    Serial.print("ROLL="); Serial.println(wRoll, 2);
    return;
  }
  if (strncmp(data, "YAW:", 4) == 0) {
    sscanf(data + 4, "%f", &wYaw);
    Serial.print("YAW="); Serial.println(wYaw, 2);
    return;
  }
  if (strncmp(data, "ENG:", 4) == 0) {
    sscanf(data + 4, "%d,%d", &wEng[0], &wEng[1]);
    Serial.print("ENG L="); Serial.print(wEng[0]);
    Serial.print(" R=");    Serial.println(wEng[1]);
    // apply immediately
    Eng::leftValue  = map(wEng[0], 0, 9, 0, 255);
    Eng::rightValue = map(wEng[1], 0, 9, 0, 255);
    analogWrite(Eng::leftEnable,  Eng::leftValue);
    analogWrite(Eng::rightEnable, Eng::rightValue);
    return;
  }
  if (strncmp(data, "GEAR:", 5) == 0) {
    sscanf(data + 5, "%d", &v); wGear = (bool)v;
    Serial.print("GEAR="); Serial.println(wGear ? "UP" : "DN");
    if (wGear != prevGear) {
      prevGear = wGear;
      wGear ? gearsUp() : gearsDown();
    }
    return;
  }
  if (strncmp(data, "FLAP:", 5) == 0) {
    int f0, f1, f2;
    sscanf(data + 5, "%d,%d,%d", &f0, &f1, &f2);
    wFlap[0]=f0; wFlap[1]=f1; wFlap[2]=f2;
    Serial.print("FLAP=");
    Serial.print(wFlap[0]); Serial.print(",");
    Serial.print(wFlap[1]); Serial.print(",");
    Serial.println(wFlap[2]);
    return;
  }
  if (strncmp(data, "RAMP:", 5) == 0) {
    sscanf(data + 5, "%d", &v); wRamp = (bool)v;
    Serial.print("RAMP="); Serial.println(wRamp ? "UP" : "DN");
    if (wRamp != prevRamp) {
      prevRamp = wRamp;
      wRamp ? rampUp() : rampDown();
    }
    return;
  }
  if (strncmp(data, "CABIN:", 6) == 0) {
    sscanf(data + 6, "%d", &v); wCabin = (bool)v;
    Serial.print("CABIN="); Serial.println(wCabin ? "UP" : "DN");
    if (wCabin != prevCabin) {
      prevCabin = wCabin;
      wCabin ? cabinOpen() : cabinClose();
    }
    return;
  }

  // unknown packet — print raw for debugging
  Serial.print("[UNKNOWN] "); Serial.println(data);
}

// ────────────────────────────────────────────────────────────
//  SETUP / LOOP
// ────────────────────────────────────────────────────────────
bool setupWifi() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting AP...");

  WiFi.beginAP(AP_SSID, AP_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_AP_LISTENING && millis() - start < 10000)
    delay(500);

  if (WiFi.status() != WL_AP_LISTENING) {
    Serial.println("AP failed!");
    return false;
  }

  udp.begin(UDP_PORT);
  Serial.print("Giga IP: ");
  Serial.println(WiFi.localIP());  // confirm what IP to put in controlPanel.ino
  Serial.println("Waiting for packets...");
  return true;
}

void loopWifi() {
  int packetSize = udp.parsePacket();
  if (packetSize <= 0) return;

  int len = udp.read(packetBuf, sizeof(packetBuf) - 1);
  if (len <= 0) return;
  packetBuf[len] = '\0';

  parsePacket(packetBuf);
}
