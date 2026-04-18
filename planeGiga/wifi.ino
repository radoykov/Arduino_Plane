#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifi_state.h"

ServoState wServo;
EngineState wEngine;
ElementState wElement;
bool wifiConnected = false;
bool wStand = false;

const char *AP_SSID = "ArduinoGigaWifi";
const char *AP_PASS = "pesho123";
const int UDP_PORT = 4210;

WiFiUDP udp;
char packetBuf[64];

void parsePacket(char *data)
{
  int v;

  if (strncmp(data, "PITCH:", 6) == 0)
  {
    sscanf(data, "PITCH:%d|ROLL:%d|YAW:%d", &wServo.pitch, &wServo.roll, &wServo.yaw);
    return;
  }
  if (strncmp(data, "ENG:", 4) == 0)
  {
    sscanf(data + 4, "%d,%d", &wEngine.left, &wEngine.right);
    return;
  }
  if (strncmp(data, "GEAR:", 5) == 0)
  {
    sscanf(data + 5, "%d", &v);
    wElement.gear = (bool)v;
    wElement.functionId = v ? FunctionID::GEAR_UP : FunctionID::GEAR_DOWN;
    return;
  }
  if (strncmp(data, "FLAP:", 5) == 0)
  {
    sscanf(data + 5, "%d", &v);
    wElement.flap = v;
    return;
  }
  if (strncmp(data, "RAMP:", 5) == 0)
  {
    sscanf(data + 5, "%d", &v);
    wElement.ramp = (bool)v;
    wElement.functionId = v ? FunctionID::RAMP_UP : FunctionID::RAMP_DOWN;
    return;
  }
  if (strncmp(data, "CABIN:", 6) == 0)
  {
    sscanf(data + 6, "%d", &v);
    wElement.cabin = (bool)v;
    wElement.functionId = v ? FunctionID::CABIN_UP : FunctionID::CABIN_DOWN;
    return;
  }
  if (strncmp(data, "STAND:", 6) == 0)
  {
    sscanf(data + 6, "%d", &v);
    wStand = (bool)v;
    return;
  }
}

void setupWifi()
{
  Serial.println("Starting AP...");
  WiFi.beginAP(AP_SSID, AP_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_AP_LISTENING)
  {
    if (millis() - start > 10000)
    {
      Serial.println("AP failed!");
      return;
    }
  }

  udp.begin(UDP_PORT);
  wifiConnected = true;
  Serial.print("Giga IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Ready.");
}

void loopWifi()
{
  if (!wifiConnected)
    return;

  int packetSize = udp.parsePacket();
  if (packetSize <= 0)
    return;

  int len = udp.read(packetBuf, sizeof(packetBuf) - 1);
  if (len <= 0)
    return;
  packetBuf[len] = '\0';

  parsePacket(packetBuf);
}
