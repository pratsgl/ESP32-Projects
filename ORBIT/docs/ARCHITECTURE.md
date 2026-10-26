# Project ORBIT Architecture

**Current milestone:** Phase 1 — board bring-up.
**Scope:** Terrestrial engineering platform; no orbital deployment is claimed.

## Verified implementation status

- Board: Heltec-branded ESP32-S3 board, visible PCB marking `HTIT-WB32LAF` and `V3.2`.
- Packaging identifies an SX1262 and advertises 863–928 MHz.
- PlatformIO environment: `heltec_wifi_lora_32_V3`.
- Build succeeded; build output reported ESP32-S3 and 8 MB flash for the selected target definition.
- ORBIT firmware booted. OLED status was observed, including `RADIO: SAFE / OFF` and `STATUS: READY`; serial output included `OLED=READY` and `state=SAFE_IDLE`.

These results do not prove radio initialization or RF communication. Sensor, GPS, ground-station, logging/dashboard, and power-system integrations are not confirmed.

## Intended system context

```text
Developer Mac — PlatformIO / USB serial
                  |
                  v
ORBIT-SAT — Heltec ESP32-S3 board
  ├── OLED status [observed with current firmware]
  ├── SX1262 LoRa [identified by packaging; operation unverified]
  ├── Environmental sensors [planned]
  └── GPS / RTC [optional, planned]
                  |
                  | LoRa telemetry/commands [planned; RF gates apply]
                  v
ORBIT-GS — Hosyond ESP32-S3 touchscreen [available; application planned]
                  |
                  | transport not selected/verified
                  v
Raspberry Pi 4B — logger/dashboard [planned]
```

The architecture image is a concept reference, not evidence that every depicted connection is implemented.

## Intended data flow

Future flow: sensor/GPS acquisition → validity checks → versioned telemetry packet → radio link → ground-station validation → local logging → dashboard. This flow is not yet confirmed operational. Missing sensor readings and invalid GPS fixes must be represented as unavailable, not fabricated.

## Commands and acknowledgements

Command handling is future work. Define an explicit allow-list, validate arguments/ranges, reject malformed or unknown commands safely, and specify acknowledgement, timeout, and retry behavior before enabling commands.

## Hardware and interfaces

| Component | Intended role | Current status |
|---|---|---|
| Heltec ESP32-S3/SX1262 board | ORBIT-SAT | Build and boot observed |
| Onboard OLED | Local status | ORBIT status observed |
| SX1262 radio | LoRa link | Packaging identifies radio; initialization/RF operation unverified |
| DHT22 / BMP280 | Environmental telemetry | Inventory only; integration not confirmed |
| DS3231 / NEO-6M GPS | Optional time/position | Inventory only; integration not confirmed |
| Hosyond touchscreen | ORBIT-GS UI | Available; application/link not verified |
| Raspberry Pi 4B, 8 GB | Logging/dashboard | Available; ORBIT service not confirmed |
| Battery/solar system | Independent power | Design and validation pending |

## RF and power safety

Do not transmit until the exact radio variant, antenna/connector compatibility, permitted operating conditions, and current Indian requirements are verified. Packaging frequency alone does not establish permission or antenna suitability.

Do not connect a battery or solar panel until cell chemistry, charging/protection, regulation, voltage/current limits, and wiring are verified. USB-powered bring-up is the current baseline.

## Acceptance evidence

Record build environment, command, result, serial output, display observations, and limitations. A successful compile does not prove runtime correctness; a successful boot does not prove RF or peripheral integration.
