#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifi_state.h"

ServoState wServo;
EngineState wEngine;
ElementState wElement;

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
    sscanf(data, "PITCH:%f|ROLL:%f|YAW:%f", &wServo.pitch, &wServo.roll, &wServo.yaw);
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
    return;
  }
  if (strncmp(data, "FLAP:", 5) == 0)
{
    sscanf(data + 5, "%d", &v);    
    wElement.flap = (bool)v;         
    return;
}
  if (strncmp(data, "RAMP:", 5) == 0)
  {
    sscanf(data + 5, "%d", &v);
    wElement.ramp = (bool)v;
    return;
  }
  if (strncmp(data, "CABIN:", 6) == 0)
  {
    sscanf(data + 6, "%d", &v);
    wElement.cabin = (bool)v;
    return;
  }
}

bool setupWifi()
{
  Serial.println("Starting AP...");
  WiFi.beginAP(AP_SSID, AP_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_AP_LISTENING)
  {
    if (millis() - start > 10000)
    {
      Serial.println("AP failed!");
      return false;
    }
  }

  udp.begin(UDP_PORT);
  Serial.print("Giga IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Ready.");
  return true;
}

void loopWifi()
{
  int packetSize = udp.parsePacket();
  if (packetSize <= 0)
    return;

  int len = udp.read(packetBuf, sizeof(packetBuf) - 1);
  if (len <= 0)
    return;
  packetBuf[len] = '\0';

  parsePacket(packetBuf);
}