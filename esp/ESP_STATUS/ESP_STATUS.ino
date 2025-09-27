#include <Adafruit_NeoPixel.h>

#define LED_PIN    0
#define LED_COUNT  26
#define LED_TYPE   NEO_RGBW + NEO_KHZ800

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, LED_TYPE);

// Pinker-LEDs
const uint8_t LEFT_INDICATORS[]  = {0, 1};
const uint8_t RIGHT_INDICATORS[] = {24, 25};

// Statuszone
const uint8_t STATUS_START = 2;
const uint8_t STATUS_END   = 23;

// Huidige status
String mode = "IDLE";
String pinkers = "AUTO";
int speed_l = 0;
int speed_r = 0;
int voltage_mv = 0;
uint8_t brightness = 77; // max 30%

// Tijdsturing
unsigned long lastAnim = 0;
bool blinkState = false;

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show();
  strip.setBrightness(brightness);
}

void loop() {
  // --- AUTOTEST: cyclisch door alle modes ---
  static unsigned long lastChange = 0;
  static int state = 0;

  if (millis() - lastChange > 5000) {
    lastChange = millis();
    state = (state + 1) % 4;

    if (state == 0) {
      mode = "MOWING";
      pinkers = "AUTO";
      speed_l = 1200;
      speed_r = 800;   // links pinkt
      Serial.println("MODE: MOWING");
    } else if (state == 1) {
      mode = "IDLE";
      pinkers = "BOTH";
      Serial.println("MODE: IDLE");
    } else if (state == 2) {
      mode = "LOW_BATTERY";
      pinkers = "OFF";
      Serial.println("MODE: LOW_BATTERY");
    } else if (state == 3) {
      mode = "STATUS";
      pinkers = "OFF";
      Serial.println("MODE: STATUS");
    }
  }

  handlePinkerLogic();
  handleStatusEffect();
  strip.show();
  delay(20);
}

// --- Pinker-logica en aansturing ---
void handlePinkerLogic() {
  bool left = false, right = false;

  if (pinkerEnabled()) {
    if (pinkerModeAuto()) {
      int delta = abs(speed_l - speed_r);
      if (delta > 200) {
        left = speed_l > speed_r;
        right = speed_r > speed_l;
      }
    } else if (pinkerMode("LEFT")) {
      left = true;
    } else if (pinkerMode("RIGHT")) {
      right = true;
    } else if (pinkerMode("BOTH")) {
      left = right = true;
    }
  }

  unsigned long now = millis();
  if (now - lastAnim > 400) {
    blinkState = !blinkState;
    lastAnim = now;
  }

  for (uint8_t i : LEFT_INDICATORS) {
    strip.setPixelColor(i, left && blinkState ? strip.Color(255, 100, 0, 0) : 0);
  }
  for (uint8_t i : RIGHT_INDICATORS) {
    strip.setPixelColor(i, right && blinkState ? strip.Color(255, 100, 0, 0) : 0);
  }
}

bool pinkerMode(String val) {
  return pinkers.equalsIgnoreCase(val);
}

bool pinkerEnabled() {
  return !pinkerMode("OFF");
}

bool pinkerModeAuto() {
  return pinkerMode("AUTO");
}

// --- Statusleds 2–23 ---
void handleStatusEffect() {
  if (mode == "MOWING") {
    for (uint8_t i = STATUS_START; i <= STATUS_END; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0, 0));  // groen
    }
  } else if (mode == "LOW_BATTERY") {
    uint8_t level = blinkState ? 180 : 80;
    for (uint8_t i = STATUS_START; i <= STATUS_END; i++) {
      strip.setPixelColor(i, strip.Color(level, 0, 0, 0));  // rood pulserend
    }
  } else if (mode == "IDLE") {
    rainbowNoOrange();
  } else if (mode == "STATUS") {
    for (uint8_t i = STATUS_START; i <= STATUS_END; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0, 255));  // wit via W-kanaal
    }
  }
}

// Regenboog zonder oranje (gefilterd)
void rainbowNoOrange() {
  static uint16_t j = 0;
  for (uint8_t i = STATUS_START; i <= STATUS_END; i++) {
    uint32_t color = strip.gamma32(strip.ColorHSV((j + i * 65536L / (STATUS_END - STATUS_START)) % 65536));
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >>  8) & 0xFF;
    uint8_t b = (color >>  0) & 0xFF;

    // Filter oranje/gele tinten
    if (r > 180 && g > 100 && b < 50) {
      color = strip.Color(0, 0, 255);  // blauw als vervanger
    }

    strip.setPixelColor(i, color);
  }
  j += 256;
}
