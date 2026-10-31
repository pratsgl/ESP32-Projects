
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include <U8g2lib.h>
#include <RadioLib.h>
#include <DHT.h>

// ============================================================
// PROJECT ORBIT - Phase 4.1
// Heltec WiFi LoRa 32 V3.2
// DHT11 temperature and humidity integration
//
// SAFETY: LoRa TX and RX remain disabled.
// ============================================================

// -------------------- Configuration -------------------------

static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint32_t STATUS_INTERVAL_MS = 5000;
static constexpr uint32_t SENSOR_INTERVAL_MS = 2500;

static constexpr uint8_t VEXT_PIN = 36;

static constexpr uint8_t OLED_RESET_PIN = 21;
static constexpr uint8_t OLED_SCL_PIN = 18;
static constexpr uint8_t OLED_SDA_PIN = 17;

static constexpr uint8_t DHT_DATA_PIN = 4;
static constexpr uint8_t DHT_TYPE = DHT11;

// SX1262 pins
static constexpr uint8_t LORA_NSS_PIN  = 8;
static constexpr uint8_t LORA_SCK_PIN  = 9;
static constexpr uint8_t LORA_MOSI_PIN = 10;
static constexpr uint8_t LORA_MISO_PIN = 11;
static constexpr uint8_t LORA_RESET_PIN = 12;
static constexpr uint8_t LORA_BUSY_PIN  = 13;
static constexpr uint8_t LORA_DIO1_PIN  = 14;

static constexpr float RADIO_INIT_FREQUENCY_MHZ = 866.0;

// -------------------- Hardware objects ---------------------

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
    U8G2_R0,
    OLED_RESET_PIN,
    OLED_SCL_PIN,
    OLED_SDA_PIN
);

DHT dht(DHT_DATA_PIN, DHT_TYPE);

SX1262 radio = new Module(
    LORA_NSS_PIN,
    LORA_DIO1_PIN,
    LORA_RESET_PIN,
    LORA_BUSY_PIN,
    SPI
);

// -------------------- Runtime state ------------------------

uint32_t lastStatusMs = 0;
uint32_t lastSensorReadMs = 0;
uint32_t telemetrySequence = 0;

bool oledReady = false;
bool radioReady = false;
bool sensorValid = false;

float temperatureC = NAN;
float humidityRH = NAN;

uint32_t successfulReads = 0;
uint32_t failedReads = 0;

int16_t radioInitResult = RADIOLIB_ERR_UNKNOWN;

// -------------------- Boot banner --------------------------

void printBootBanner() {
    Serial.println();
    Serial.println("========================================");
    Serial.println("       PROJECT ORBIT - SATELLITE");
    Serial.println("========================================");
    Serial.println("Phase          : 4.1 - Sensor Integration");
    Serial.println("Controller     : Heltec WiFi LoRa 32 V3.2");
    Serial.println("Sensor         : DHT11 driver");
    Serial.println("Sensor GPIO    : GPIO4");
    Serial.println("OLED           : SSD1306 128x64");
    Serial.println("Radio          : SX1262");
    Serial.println("RF TX          : DISABLED");
    Serial.println("RF RX          : DISABLED");
    Serial.println("========================================");
}

// -------------------- OLED initialization ------------------

void initializeOled() {
    pinMode(VEXT_PIN, OUTPUT);
    digitalWrite(VEXT_PIN, LOW);
    delay(100);

    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

    oled.begin();
    oled.setFont(u8g2_font_6x10_tf);
    oled.clearBuffer();

    oled.drawStr(0, 12, "PROJECT ORBIT");
    oled.drawStr(0, 27, "Satellite Boot");
    oled.drawStr(0, 42, "Phase 4.1");
    oled.drawStr(0, 57, "Initializing...");

    oled.sendBuffer();

    oledReady = true;
    Serial.println("[OLED] Initialized");
}

// -------------------- DHT11 initialization -----------------

void initializeDhtSensor() {
    dht.begin();
    delay(1500);

    Serial.println("[DHT11] Driver initialized");
    Serial.println("[DHT11] Data GPIO: GPIO4");
}

// -------------------- Sensor reading -----------------------

void readDhtSensor() {
    const float humidity = dht.readHumidity();
    const float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature) ||
        humidity < 0.0 || humidity > 100.0 ||
        temperature < -40.0 || temperature > 80.0) {

        sensorValid = false;
        failedReads++;

        Serial.println("[DHT11] ERROR: Invalid sensor reading");
        return;
    }

    temperatureC = temperature;
    humidityRH = humidity;
    sensorValid = true;

    successfulReads++;
    telemetrySequence++;

    Serial.printf(
        "[DHT11] Temperature: %.1f C | Humidity: %.1f %% RH\n",
        temperatureC,
        humidityRH
    );
}

// -------------------- Radio initialization -----------------

void initializeRadio() {
    SPI.begin(
        LORA_SCK_PIN,
        LORA_MISO_PIN,
        LORA_MOSI_PIN,
        LORA_NSS_PIN
    );

    radio.tcxoVoltage = 1.8;

    radioInitResult = radio.begin(RADIO_INIT_FREQUENCY_MHZ);

    if (radioInitResult == RADIOLIB_ERR_NONE) {
        radioReady = true;
        Serial.println("[RADIO] SX1262 initialized successfully");
    } else {
        radioReady = false;

        Serial.printf(
            "[RADIO] Initialization failed: %d\n",
            radioInitResult
        );
    }

    Serial.println("[RADIO] TX DISABLED");
    Serial.println("[RADIO] RX DISABLED");
}

// -------------------- OLED telemetry ----------------------

void drawDisplay() {
    if (!oledReady) {
        return;
    }

    oled.clearBuffer();

    oled.setFont(u8g2_font_6x10_tf);

    oled.drawStr(0, 10, "ORBIT SATELLITE");
    oled.drawHLine(0, 13, 128);

    if (sensorValid) {
        char temperatureText[24];
        char humidityText[24];

        snprintf(
            temperatureText,
            sizeof(temperatureText),
            "Temp: %.1f C",
            temperatureC
        );

        snprintf(
            humidityText,
            sizeof(humidityText),
            "Humidity: %.1f%%",
            humidityRH
        );

        oled.drawStr(0, 27, temperatureText);
        oled.drawStr(0, 39, humidityText);
    } else {
        oled.drawStr(0, 27, "Sensor: ERROR");
        oled.drawStr(0, 39, "Check DHT11");
    }

    oled.drawHLine(0, 43, 128);

    oled.drawStr(
        0,
        55,
        radioReady ? "RADIO: READY" : "RADIO: ERROR"
    );

    oled.drawStr(0, 64, "RF: TX OFF / RX OFF");

    oled.sendBuffer();
}

// -------------------- Serial telemetry --------------------

void printTelemetry() {
    Serial.println();
    Serial.println("--------------- ORBIT TELEMETRY ---------------");

    Serial.printf("Sequence       : %lu\n",
                  static_cast<unsigned long>(telemetrySequence));

    Serial.printf("Uptime         : %lu ms\n",
                  static_cast<unsigned long>(millis()));

    if (sensorValid) {
        Serial.printf("Temperature    : %.1f C\n", temperatureC);
        Serial.printf("Humidity       : %.1f %% RH\n", humidityRH);
        Serial.println("Sensor         : DHT11_VALID");
    } else {
        Serial.println("Sensor         : DHT11_ERROR");
    }

    Serial.printf("Valid reads    : %lu\n",
                  static_cast<unsigned long>(successfulReads));

    Serial.printf("Failed reads   : %lu\n",
                  static_cast<unsigned long>(failedReads));

    Serial.printf("OLED           : %s\n",
                  oledReady ? "READY" : "ERROR");

    Serial.printf("RADIO          : %s\n",
                  radioReady ? "INITIALIZED" : "ERROR");

    Serial.printf("Radio code     : %d\n", radioInitResult);

    Serial.println("RF             : TX_OFF,RX_OFF");
    Serial.println("State          : SAFE_IDLE");

    Serial.println("------------------------------------------------");
}

// -------------------- Setup --------------------------------

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1500);

    printBootBanner();

    initializeOled();
    initializeDhtSensor();
    initializeRadio();

    // Take the first sensor reading after initialization.
    lastSensorReadMs = millis() - SENSOR_INTERVAL_MS;
    lastStatusMs = millis();

    Serial.println();
    Serial.println("[ORBIT] Initialization sequence complete");
}

// -------------------- Main loop ----------------------------

void loop() {
    const uint32_t now = millis();

    if (now - lastSensorReadMs >= SENSOR_INTERVAL_MS) {
        lastSensorReadMs = now;

        readDhtSensor();
        drawDisplay();
    }

    if (now - lastStatusMs >= STATUS_INTERVAL_MS) {
        lastStatusMs = now;
        printTelemetry();
    }

    delay(10);
}

