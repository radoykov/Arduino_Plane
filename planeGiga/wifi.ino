#include <WiFi.h>
#include <WiFiUdp.h>

namespace Wifi
{
  // ── Credentials ────────────────────────────────────────
  const char* SSID = "YOUR_SSID";      // <-- fill in (same network as UNO)
  const char* PASS = "YOUR_PASSWORD";  // <-- fill in

  const int PORT = 4210;

  WiFiUDP udp;
  char    packetBuf[64];

  // ── Received state ──────────────────────────────────────
  int  eng[2]   = { 0, 0 };
  bool gear     = false;
  bool flap[3]  = { false, false, false };
  bool ramp     = false;
  bool cabin    = false;

  // ── Previous state (edge detection) ────────────────────
  bool prevGear  = false;
  bool prevRamp  = false;
  bool prevCabin = false;

  // ── Non-blocking motor ──────────────────────────────────
  struct Motor {
    bool          running    = false;
    bool          dirFwd     = true;
    unsigned long startMs    = 0;
    unsigned long durationMs = 0;
    int pinEnable, pinIn1, pinIn2;
  };

  // !! Set correct pins for your GIGA wiring !!
  //               running  fwd   start  dur   EN   IN1  IN2
  Motor mGear  = { false,  true,   0,    0,    5,   30,  31 };
  Motor mRamp  = { false,  true,   0,    0,    8,   32,  33 };
  Motor mCabin = { false,  true,   0,    0,    9,   34,  35 };

  // ── Parse incoming packet ───────────────────────────────
  void parsePacket(char* data)
  {
    char* p;
    int   v;

    p = strstr(data, "ENG:");
    if (p) sscanf(p, "ENG:%d,%d", &eng[0], &eng[1]);

    p = strstr(data, "GEAR:");
    if (p) { sscanf(p, "GEAR:%d",  &v); gear  = (bool)v; }

    int f0, f1, f2;
    p = strstr(data, "FLAP:");
    if (p) {
      sscanf(p, "FLAP:%d,%d,%d", &f0, &f1, &f2);
      flap[0] = f0; flap[1] = f1; flap[2] = f2;
    }

    p = strstr(data, "RAMP:");
    if (p) { sscanf(p, "RAMP:%d",  &v); ramp  = (bool)v; }

    p = strstr(data, "CABIN:");
    if (p) { sscanf(p, "CABIN:%d", &v); cabin = (bool)v; }
  }

  // ── Motor helpers ───────────────────────────────────────
  void startMotor(Motor& m, bool forward, unsigned long dur)
  {
    if (m.running) return;
    m.running    = true;
    m.dirFwd     = forward;
    m.startMs    = millis();
    m.durationMs = dur;
    analogWrite (m.pinEnable, 255);
    digitalWrite(m.pinIn1, forward ? HIGH : LOW);
    digitalWrite(m.pinIn2, forward ? LOW  : HIGH);
  }

  void stopMotor(Motor& m)
  {
    m.running = false;
    analogWrite (m.pinEnable, 0);
    digitalWrite(m.pinIn1, LOW);
    digitalWrite(m.pinIn2, LOW);
  }

  void tickMotor(Motor& m)
  {
    if (m.running && (millis() - m.startMs >= m.durationMs))
      stopMotor(m);
  }

  // ── Apply engines ───────────────────────────────────────
  // Maps panel value 0-9 → PWM 0-255
  // Uses same enable pins as 3_engines.ino
  void applyEngines()
  {
    analogWrite(Eng::leftEnable,  map(eng[0], 0, 9, 0, 255));
    analogWrite(Eng::rightEnable, map(eng[1], 0, 9, 0, 255));
  }

  // ── Apply flaps ─────────────────────────────────────────
  // TODO: attach 3 servos and set their pins
  // Servo flapServos[3];
  void applyFlaps()
  {
    // flapServos[0].write(flap[0] ? 90 : 0);
    // flapServos[1].write(flap[1] ? 90 : 0);
    // flapServos[2].write(flap[2] ? 90 : 0);
  }

} // namespace Wifi

// ─────────────────────────────────────────────────────────
// TaskScheduler callbacks
// ─────────────────────────────────────────────────────────

bool setupWifi()
{
  // Motor pins
  pinMode(Wifi::mGear.pinEnable,  OUTPUT);
  pinMode(Wifi::mGear.pinIn1,     OUTPUT);
  pinMode(Wifi::mGear.pinIn2,     OUTPUT);
  pinMode(Wifi::mRamp.pinEnable,  OUTPUT);
  pinMode(Wifi::mRamp.pinIn1,     OUTPUT);
  pinMode(Wifi::mRamp.pinIn2,     OUTPUT);
  pinMode(Wifi::mCabin.pinEnable, OUTPUT);
  pinMode(Wifi::mCabin.pinIn1,    OUTPUT);
  pinMode(Wifi::mCabin.pinIn2,    OUTPUT);

  // Set static IP so UNO always knows where to send
  // Match this IP with GIGA_IP in controlPanel.ino
  IPAddress local  (192, 168, 1, 100);  // <-- GIGA static IP
  IPAddress gateway(192, 168, 1,   1);  // <-- your router (run ipconfig on PC)
  IPAddress subnet (255, 255, 255,  0);
  WiFi.config(local, gateway, subnet);

  WiFi.begin(Wifi::SSID, Wifi::PASS);

  // Blocking wait — fine here, scheduler not running yet
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
    delay(500);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("GIGA: WiFi failed");
    return false;
  }

  Wifi::udp.begin(Wifi::PORT);
  Serial.print("GIGA IP: ");
  Serial.println(WiFi.localIP());  // should print 192.168.1.100
  return true;
}

void loopWifi()
{
  using namespace Wifi;

  // 1. Tick motors (replaces delay())
  tickMotor(mGear);
  tickMotor(mRamp);
  tickMotor(mCabin);

  // 2. Read UDP
  int packetSize = udp.parsePacket();
  if (packetSize <= 0) return;

  int len = udp.read(packetBuf, sizeof(packetBuf) - 1);
  packetBuf[len] = '\0';
  parsePacket(packetBuf);

  // 3. React

  // Engines — continuous value, apply every packet
  applyEngines();

  // Flaps — apply every packet
  applyFlaps();

  // Gear — only trigger on state change
  if (gear != prevGear) {
    prevGear = gear;
    startMotor(mGear, gear, 8000);   // 8 s → adjust to your motor
  }

  // Ramp
  if (ramp != prevRamp) {
    prevRamp = ramp;
    startMotor(mRamp, ramp, 4000);   // 4 s → adjust
  }

  // Cabin
  if (cabin != prevCabin) {
    prevCabin = cabin;
    startMotor(mCabin, cabin, 4300); // 4.3 s → matches original delay
  }
}
