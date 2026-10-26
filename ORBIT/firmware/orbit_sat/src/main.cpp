
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// Project ORBIT — ORBIT-SAT
// Phase 2: OLED initialization
// LoRa radio remains uninitialized; no RF transmission.

static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint32_t STATUS_INTERVAL_MS = 5000;

// Verified from the installed Heltec WiFi LoRa 32 V3 variant.
static constexpr uint8_t VEXT_PIN = 36;
static constexpr uint8_t OLED_RESET_PIN = 21;
static constexpr uint8_t OLED_SCL_PIN = 18;
static constexpr uint8_t OLED_SDA_PIN = 17;

// SSD1306 128x64 OLED, hardware I2C.
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
    U8G2_R0,
    OLED_RESET_PIN,
    OLED_SCL_PIN,
    OLED_SDA_PIN
);

uint32_t lastStatusMs = 0;
bool oledReady = false;

void printBootBanner() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("          PROJECT ORBIT"));
  Serial.println(F("             ORBIT-SAT"));
  Serial.println(F("========================================"));
  Serial.println(F("Phase       : 2 - OLED initialization"));
  Serial.println(F("Firmware    : 0.1.1"));
  Serial.println(F("Radio       : NOT INITIALIZED"));
  Serial.println(F("RF transmit : DISABLED"));
  Serial.println(F("Wi-Fi/BT    : NOT INITIALIZED"));
  Serial.println(F("========================================"));
}

void drawDisplay() {
  if (!oledReady) {
    return;
  }

  oled.clearBuffer();

  oled.setFont(u8g2_font_6x10_tf);
  oled.drawStr(0, 10, "PROJECT ORBIT");
  oled.drawHLine(0, 13, 128);

  oled.setFont(u8g2_font_7x14B_tf);
  oled.drawStr(0, 29, "ORBIT-SAT");

  oled.setFont(u8g2_font_5x8_tf);
  oled.drawStr(0, 42, "FW: 0.1.1");
  oled.drawStr(0, 52, "RADIO: SAFE / OFF");
  oled.drawStr(0, 62, "STATUS: READY");

  oled.sendBuffer();
}

void initializeOled() {
  // Enable the board's external peripheral supply.
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);
  delay(100);

  // Initialize I2C on the verified Heltec pins.
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  delay(50);

  oledReady = oled.begin();

  if (oledReady) {
    Serial.println(F("OLED       : INITIALIZED"));
    drawDisplay();
  } else {
    Serial.println(F("OLED       : INITIALIZATION FAILED"));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);

  // Give the USB-to-UART serial monitor time to attach.
  const uint32_t waitStart = millis();
  while (!Serial && (millis() - waitStart < 3000)) {
    delay(10);
  }

  printBootBanner();
  initializeOled();

  Serial.println(F("RF transmit: DISABLED"));
  Serial.println(F("System     : SAFE_IDLE"));

  lastStatusMs = millis();
}

void loop() {
  const uint32_t now = millis();

  if (now - lastStatusMs >= STATUS_INTERVAL_MS) {
    lastStatusMs = now;

    Serial.print(F("[ORBIT-SAT] uptime_ms="));
    Serial.print(now);
    Serial.print(F(" | OLED="));
    Serial.print(oledReady ? F("READY") : F("ERROR"));
    Serial.println(F(" | state=SAFE_IDLE"));
  }

  delay(10);
}
