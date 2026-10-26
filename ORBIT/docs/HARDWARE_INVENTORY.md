# Project ORBIT Hardware Inventory

“Available” means recorded as owned; it does not imply integration or successful testing.

## ORBIT-SAT board

- Received board: Heltec-branded ESP32-S3 board.
- Visible PCB marking: `HTIT-WB32LAF`.
- Visible revision marking: `V3.2`.
- Packaging identifies SX1262 and advertises 863–928 MHz.
- PlatformIO environment used for successful build: `heltec_wifi_lora_32_V3`.
- Build output reported ESP32-S3 and 8 MB flash for the selected target definition.
- Observed runtime: ORBIT firmware booted; OLED showed ORBIT status; serial included `OLED=READY` and `state=SAFE_IDLE`.
- Still to verify: exact hardware/RF variant against manufacturer documentation; antenna/connector compatibility; board-specific radio details.

Packaging and visible markings are recorded observations, not a substitute for manufacturer documentation.

## Other owned hardware

- Raspberry Pi 4B, 8 GB.
- ESP32-S3 Mini DevKit, 16 MB flash / 8 MB PSRAM.
- Hosyond ESP32-S3 touchscreen, 2.8-inch, 240×320.
- DHT22 and DHT11 temperature/humidity sensors.
- BMP280 pressure/temperature sensor.
- DS3231 RTC.
- NEO-6M GPS module.
- LoRa modules previously inventoried: Heltec SX1262, RA-02, SX1268, E22-400T22S. Exact variants, bands, interfaces, and current availability must be checked before assigning a role.

## Not yet confirmed/purchased for ORBIT

- A verified compatible antenna/pigtail combination for the onboard radio.
- Any additional ground-station radio, if existing inventory cannot meet the requirement.
- Battery, solar panel, and charging/protection components.

## Compatibility gates

1. Verify board model/revision and manufacturer pinout.
2. Verify OLED driver/interface and source-code pin definitions.
3. Verify SX1262 interface pins and RF variant.
4. Verify antenna/connector compatibility and applicable Indian requirements before RF transmission.
5. Check existing LoRa inventory before buying another module.
6. Verify sensor voltage levels and wiring before connecting.
7. Engineer battery charging/protection, regulation, and solar input before connection.
