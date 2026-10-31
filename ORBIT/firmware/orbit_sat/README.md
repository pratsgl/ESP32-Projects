
# Project ORBIT — ORBIT-SAT Firmware

## 1. Project Overview

ORBIT is an ESP32-S3-based educational satellite simulator.

This firmware runs on the Heltec WiFi LoRa 32 V3.2 board and provides:
- Environmental telemetry using a DHT11-compatible sensor.
- Local telemetry display using the onboard OLED.
- SX1262 radio initialization.
- Serial telemetry for development and ground-station integration.

**Current development branch:** `feature/lora-dht11-telemetry`

## 2. Hardware Platform

| Component | Details |
|---|---|
| Controller | Heltec WiFi LoRa 32 V3.2 |
| PCB marking | HTIT-WB32LAF |
| MCU | ESP32-S3 |
| Flash | 8 MB (selected PlatformIO target) |
| Temperature/Humidity | DHT11-compatible 3-pin module |
| Display | SSD1306 OLED, 128 × 64 |
| Radio | Semtech SX1262 |

## 3. Current Verified Status

- PlatformIO build succeeded.
- Firmware successfully flashed and executed on the Heltec board.
- DHT11 driver produced valid temperature and humidity readings.
- OLED initialization reported `READY`.
- SX1262 initialization returned RadioLib result code `0`.
- Serial telemetry sequence incremented correctly.
- Ten consecutive valid sensor readings were observed with zero failures.

### Observed sensor readings

| Measurement | Observed value |
|---|---:|
| Temperature | 28.0 °C |
| Relative humidity | 35.0% RH |
| Valid readings | 10 |
| Failed readings | 0 |

These are observed test readings, not calibrated measurements.

## 4. DHT11 Sensor Integration

### Wiring diagram

The DHT11-compatible module is connected to the Heltec board as follows:

```text
       DHT11 MODULE
      +------------+
      |            |
      | VCC  DATA  GND
      +--|----|----|--+
         |    |    |
         |    |    |
         |    |    |
      3V3   GPIO4  GND
       |     |      |
       +-----+------+
             |
    HELTEC WiFi LoRa 32
          V3.2
```

### Pin mapping

| DHT11 module | Heltec board |
|---|---|
| VCC | 3V3 |
| DATA | GPIO4 |
| GND | GND |

**Important:** Follow the labels printed on the sensor module. Connector pin order can vary between modules.

### Firmware configuration

```cpp
static constexpr uint8_t DHT_DATA_PIN = 4;
static constexpr uint8_t DHT_TYPE = DHT11;

DHT dht(DHT_DATA_PIN, DHT_TYPE);
```

Sensor sampling interval: 2500 ms.

### Sensor identification note

The module was initially tested using the DHT22 driver, which produced invalid readings. The DHT11 driver produced consistent readings of 28.0 °C and 37.0% RH on the standalone NodeMCU test.

On the Heltec board, the DHT11 driver subsequently produced valid readings of 28.0 °C and 35.0% RH.

The exact physical sensor model should still be confirmed against its product specification.

## 5. Heltec Onboard Hardware Pin Mapping

### OLED display

| Signal | GPIO |
|---|---:|
| SDA | 17 |
| SCL | 18 |
| RESET | 21 |

### SX1262 LoRa radio

| Signal | GPIO |
|---|---:|
| NSS | 8 |
| SCK | 9 |
| MOSI | 10 |
| MISO | 11 |
| RESET | 12 |
| BUSY | 13 |
| DIO1 | 14 |

### Other board signals

| Signal | GPIO |
|---|---:|
| Vext control | 36 |

These mappings are based on the hardware reference documents maintained in `ORBIT/docs/hardware/`.

## 6. Firmware Architecture

```text
             DHT11 SENSOR
          Temperature / Humidity
                    |
                    v
             ORBIT FIRMWARE
                    |
          +---------+---------+
          |                   |
          v                   v
       OLED DISPLAY       SERIAL TELEMETRY
          |                   |
          v                   v
     Local Display       Development /
                         Ground Station

             SX1262 RADIO
                    |
             Initialized only
                    |
              TX OFF / RX OFF
```

## 7. Software Dependencies

- PlatformIO
- Arduino framework for ESP32
- U8g2
- RadioLib
- Adafruit DHT sensor library
- Adafruit Unified Sensor

Dependencies are managed through `platformio.ini`.

## 8. Build Instructions

From the repository root:

```bash
cd ORBIT/firmware/orbit_sat
pio run -e heltec_wifi_lora_32_V3
```

## 9. Upload Instructions

Connect the Heltec board and confirm the serial port.

```bash
pio run -e heltec_wifi_lora_32_V3 -t upload
```

Open the serial monitor:

```bash
pio device monitor -b 115200
```

## 10. Expected Telemetry

Example observed output:

```text
--------------- ORBIT TELEMETRY ---------------
Sequence       : 10
Uptime         : 28562 ms
Temperature    : 28.0 C
Humidity       : 35.0 % RH
Sensor         : DHT11_VALID
Valid reads    : 10
Failed reads   : 0
OLED           : READY
RADIO          : INITIALIZED
Radio code     : 0
RF             : TX_OFF,RX_OFF
State          : SAFE_IDLE
------------------------------------------------
```

## 11. Radio Safety

- SX1262 initialization has been verified.
- No LoRa transmission has been performed.
- No LoRa reception has been performed.
- Keep RF TX and RX disabled until the radio configuration, antenna, frequency variant and applicable operating requirements are verified.

Radio initialization does not prove RF communication.

## 12. Known Limitations

- DHT11-compatible sensor model requires confirmation from its product specification.
- Temperature and humidity readings have not been independently calibrated.
- LoRa communication is not implemented or tested.
- Raspberry Pi ground-station integration is pending.
- Battery and solar power integration is pending.

## 13. Development Branch

Current branch:

```text
feature/lora-dht11-telemetry
```

Firmware commit:

```text
2367e34
Integrate DHT11 sensor with ORBIT telemetry
```

Documentation changes should be committed separately from firmware changes.

## 14. Hardware Reference Documents

Refer to:

```text
ORBIT/docs/hardware/
```

Important files:

- `HTIT-WB32LA_V3.2.pdf`
- `LoRa-32-v3-components.jpg`
- `Wi-Fi_LoRa32_V3.2_Pinmap.png`
- `WiFi_LoRa_32_V3.2_Schematic_Diagram.pdf`
