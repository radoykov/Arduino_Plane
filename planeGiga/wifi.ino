#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifi_state.h"

// forward declarations from 2_elements.ino
void gearsUp();
void gearsDown();
void cabinOpen();
void cabinClose();
void rampUp();
void rampDown();

const char *AP_SSID = "ArduinoGigaWifi";
const char *AP_PASS = "pesho123";
const int UDP_PORT = 4210;

WiFiUDP udp;
char packetBuf[128];

int wEng[2] = {0, 0};
bool wGear = false;
bool wFlap[3] = {false, false, false};
bool wRamp = false;
bool wCabin = false;
float wPitch = 0.0f;
float wRoll = 0.0f;
float wYaw = 0.0f;

bool prevGear = false;
bool prevRamp = false;
bool prevCabin = false;

void parsePacket(char *data)
{
  char *p;
  int v;

  p = strstr(data, "ENG:");
  if (p)
  {
    sscanf(p, "ENG:%d,%d", &wEng[0], &wEng[1]);
    Eng::leftValue = map(wEng[0], 0, 9, 0, 255);
    Eng::rightValue = map(wEng[1], 0, 9, 0, 255);
    analogWrite(Eng::leftEnable, Eng::leftValue);
    analogWrite(Eng::rightEnable, Eng::rightValue);
  }

  p = strstr(data, "GEAR:");
  if (p)
  {
    sscanf(p, "GEAR:%d", &v);
    wGear = (bool)v;
    if (wGear != prevGear)
    {
      prevGear = wGear;
      wGear ? gearsUp() : gearsDown();
    }
  }

  int f0, f1, f2;
  p = strstr(data, "FLAP:");
  if (p)
  {
    sscanf(p, "FLAP:%d,%d,%d", &f0, &f1, &f2);
    wFlap[0] = f0;
    wFlap[1] = f1;
    wFlap[2] = f2;
  }

  p = strstr(data, "RAMP:");
  if (p)
  {
    sscanf(p, "RAMP:%d", &v);
    wRamp = (bool)v;
    if (wRamp != prevRamp)
    {
      prevRamp = wRamp;
      wRamp ? rampUp() : rampDown();
    }
  }

  p = strstr(data, "CABIN:");
  if (p)
  {
    sscanf(p, "CABIN:%d", &v);
    wCabin = (bool)v;
    if (wCabin != prevCabin)
    {
      prevCabin = wCabin;
      wCabin ? cabinOpen() : cabinClose();
    }
  }

  p = strstr(data, "PITCH:");
  if (p)
    sscanf(p, "PITCH:%f", &wPitch);

  p = strstr(data, "ROLL:");
  if (p)
    sscanf(p, "ROLL:%f", &wRoll);

  p = strstr(data, "YAW:");
  if (p)
    sscanf(p, "YAW:%f", &wYaw);
}

bool setupWifi()
{
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting AP...");

  WiFi.beginAP(AP_SSID, AP_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_AP_LISTENING && millis() - start < 10000)
    delay(500);

  if (WiFi.status() != WL_AP_LISTENING)
  {
    Serial.println("AP failed");
    return false;
  }

  udp.begin(UDP_PORT);
  Serial.print("GIGA IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

void loopWifi()
{
  int packetSize = udp.parsePacket();
  if (packetSize <= 0)
    return;

  memset(packetBuf, 0, sizeof(packetBuf));
  int len = udp.read(packetBuf, sizeof(packetBuf) - 1);
  if (len <= 0)
    return;
  packetBuf[len] = '\0';

  // ← removed the ENG: filter that was killing everything else

  parsePacket(packetBuf);

  Serial.print("P:");
  Serial.print(wPitch, 2);
  Serial.print(" R:");
  Serial.print(wRoll, 2);
  Serial.print(" Y:");
  Serial.println(wYaw, 2);
}