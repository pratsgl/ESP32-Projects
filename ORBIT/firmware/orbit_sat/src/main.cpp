
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <RadioLib.h>

// ============================================================
// PROJECT ORBIT — ORBIT-SAT
// Phase 3: SX1262 radio initialization validation
//
// RF transmission: DISABLED
// Packet reception: DISABLED
// Wi-Fi/Bluetooth: NOT INITIALIZED
//
// Hardware reference:
// Heltec WiFi LoRa 32 V3.2
// HTIT-WB32LAF_V3.2 reference schematic
// ============================================================

static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint32_t STATUS_INTERVAL_MS = 5000;

// Heltec board power and OLED pins
static constexpr uint8_t VEXT_PIN = 36;
static constexpr uint8_t OLED_RESET_PIN = 21;
static constexpr uint8_t OLED_SCL_PIN = 18;
static constexpr uint8_t OLED_SDA_PIN = 17;

// SX1262 SPI pins — verified against the schematic
static constexpr uint8_t LORA_NSS_PIN  = 8;
static constexpr uint8_t LORA_SCK_PIN  = 9;
static constexpr uint8_t LORA_MOSI_PIN = 10;
static constexpr uint8_t LORA_MISO_PIN = 11;

// SX1262 control pins — verified against the schematic
static constexpr uint8_t LORA_RESET_PIN = 12;
static constexpr uint8_t LORA_BUSY_PIN  = 13;
static constexpr uint8_t LORA_DIO1_PIN  = 14;

// Initialization frequency only; no TX or RX is started.
// Confirm the module's regional variant before future RF use.
static constexpr float RADIO_INIT_FREQUENCY_MHZ = 866.0;

// OLED: SSD1306 128x64
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
    U8G2_R0,
    OLED_RESET_PIN,
    OLED_SCL_PIN,
    OLED_SDA_PIN
);

// Explicitly use the Heltec LoRa SPI bus and control pins.
SX1262 radio = new Module(
    LORA_NSS_PIN,
    LORA_DIO1_PIN,
    LORA_RESET_PIN,
    LORA_BUSY_PIN,
    SPI
);

uint32_t lastStatusMs = 0;
bool oledReady = false;
bool radioReady = false;
int16_t radioInitResult = RADIOLIB_ERR_UNKNOWN;

void printBootBanner() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("          PROJECT ORBIT"));
  Serial.println(F("             ORBIT-SAT"));
  Serial.println(F("========================================"));
  Serial.println(F("Phase       : 3 - Radio initialization"));
  Serial.println(F("Firmware    : 0.2.0"));
  Serial.println(F("Board       : Heltec WiFi LoRa 32 V3.2"));
  Serial.println(F("Radio       : SX1262"));
  Serial.println(F("RF transmit : DISABLED"));
  Serial.println(F("Packet RX   : DISABLED"));
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
  oled.drawStr(0, 42, "FW: 0.2.0");

  if (radioReady) {
    oled.drawStr(0, 52, "RADIO: INITIALIZED");
    oled.drawStr(0, 62, "RF: TX/RX OFF");
  } else {
    oled.drawStr(0, 52, "RADIO: INIT FAILED");
    oled.drawStr(0, 62, "RF: TX/RX OFF");
  }

  oled.sendBuffer();
}

void initializeOled() {
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);
  delay(100);

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  delay(50);

  oledReady = oled.begin();

  if (oledReady) {
    Serial.println(F("OLED        : INITIALIZED"));
  } else {
    Serial.println(F("OLED        : INITIALIZATION FAILED"));
  }
}

void initializeRadio() {
  Serial.println();
  Serial.println(F("[RADIO] Starting SX1262 initialization"));

  // The schematic shows an externally powered 32 MHz
  // oscillator. Do not request DIO3 TCXO power based on
  // generic RadioLib examples.
  //
  // This setting must be confirmed against the exact board
  // revision before considering the radio operational.
  radio.tcxoVoltage = 1.8;

  // Configure the board's dedicated SPI pins.
  SPI.begin(
      LORA_SCK_PIN,
      LORA_MISO_PIN,
      LORA_MOSI_PIN,
      LORA_NSS_PIN
  );

  Serial.println(F("[RADIO] SPI pins configured"));
  Serial.println(F("[RADIO] Initializing at 866 MHz"));
  Serial.println(F("[RADIO] No packet TX/RX will be started"));

  // RadioLib's SX1262 LoRa begin() configures DIO2 as
  // RF-switch control in this library version.
  // Do not call setDio2AsRfSwitch() redundantly here.
  radioInitResult = radio.begin(RADIO_INIT_FREQUENCY_MHZ);

  if (radioInitResult == RADIOLIB_ERR_NONE) {
    radioReady = true;
    Serial.println(F("[RADIO] INITIALIZATION SUCCESS"));
  } else {
    radioReady = false;
    Serial.print(F("[RADIO] INITIALIZATION FAILED, code="));
    Serial.println(radioInitResult);
  }

  Serial.println(F("[RADIO] Transmit remains DISABLED"));
  Serial.println(F("[RADIO] Receive remains DISABLED"));
}

void setup() {
  Serial.begin(SERIAL_BAUD);

  const uint32_t waitStart = millis();
  while (!Serial && (millis() - waitStart < 3000)) {
    delay(10);
  }

  printBootBanner();

  initializeOled();
  initializeRadio();

  drawDisplay();

  Serial.println();
  Serial.println(F("[ORBIT-SAT] System initialization complete"));
  Serial.println(F("[ORBIT-SAT] state=SAFE_IDLE"));

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

    Serial.print(F(" | RADIO="));
    Serial.print(radioReady ? F("INITIALIZED") : F("ERROR"));

    Serial.print(F(" | radio_code="));
    Serial.print(radioInitResult);

    Serial.println(F(" | RF=TX_OFF,RX_OFF | state=SAFE_IDLE"));
  }

  delay(10);
}
