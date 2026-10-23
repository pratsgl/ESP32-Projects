# Project ORBIT hardware inventory

## Ordered
- Robocraze V3 ESP32-S3 + SX1262 LoRa board
- Onboard 0.96-inch OLED
- Delivery date not yet confirmed

## Already owned
- Raspberry Pi 4B, 8GB
- ESP32-S3 Mini DevKit, 16MB flash / 8MB PSRAM
- Hosyond ESP32-S3 2.8-inch 240×320 touchscreen
- DHT22, BMP280, DS3231 RTC, DHT11
- NEO-6M GPS module
- Various LoRa modules; exact models/frequencies to be inventoried

## Not yet purchased
- Antenna and pigtail
- Additional ground-station radio, if required
- Battery, solar panel and charging/protection components

## Compatibility gates
1. Record exact board name, PCB revision and module markings.
2. Confirm OLED controller, bus, address (if I²C) and pin mapping.
3. Confirm SX1262 SPI/BUSY/RESET/DIO pins and RF connector type.
4. Confirm radio variant, antenna specification and Indian operating requirements.
5. Confirm a compatible charging/protection circuit before connecting a battery.
6. Check existing LoRa inventory before buying another radio.
