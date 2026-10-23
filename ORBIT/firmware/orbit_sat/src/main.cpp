/*
 * Project ORBIT - Phase 1 safe bring-up firmware
 *
 * Serial diagnostics only. No LoRa transmission and no guessed OLED/radio pins.
 * Add board-specific OLED and SX1262 initialization after confirming the
 * exact PCB revision and pin mapping.
 */
#include <Arduino.h>

static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint32_t STATUS_INTERVAL_MS = 5000;
uint32_t lastStatusMs = 0;

void printBootBanner() {
  Serial.println();
  Serial.println(F("===================================="));
  Serial.println(F("       PROJECT ORBIT - ORBIT-SAT"));
  Serial.println(F("===================================="));
  Serial.println(F("ORBIT-SAT BOOT OK"));
  Serial.println(F("Phase: 1 - Hardware bring-up"));
  Serial.println(F("Mode: SAFE / NO LoRa TRANSMISSION"));
  Serial.println(F("OLED: pending exact controller/pinout"));
  Serial.println(F("SX1262: not initialized; pinout pending"));
  Serial.println(F("===================================="));
}

void printStatus() {
  Serial.print(F("[ORBIT] uptime_ms="));
  Serial.print(millis());
  Serial.print(F(" free_heap_bytes="));
  Serial.print(ESP.getFreeHeap());
  Serial.print(F(" chip_model="));
  Serial.print(ESP.getChipModel());
  Serial.print(F(" chip_revision="));
  Serial.print(ESP.getChipRevision());
  Serial.print(F(" cpu_mhz="));
  Serial.println(ESP.getCpuFreqMHz());
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  const uint32_t waitStart = millis();
  while (!Serial && (millis() - waitStart < 3000)) {
    delay(10);
  }
  printBootBanner();
  printStatus();
  lastStatusMs = millis();
}

void loop() {
  const uint32_t now = millis();
  if (now - lastStatusMs >= STATUS_INTERVAL_MS) {
    lastStatusMs = now;
    printStatus();
  }
  // Deliberately no radio transmission or unverified peripheral access.
  delay(10);
}
