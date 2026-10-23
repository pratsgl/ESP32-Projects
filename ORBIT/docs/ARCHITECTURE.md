# Project ORBIT architecture

## ORBIT-SAT
Robocraze ESP32-S3 + SX1262 board. Intended functions: sensors/RTC, optional GPS, OLED status, LoRa telemetry after RF checks, and later ground commands/acknowledgements.

## ORBIT-GS
Hosyond ESP32-S3 touchscreen. Intended functions: telemetry display and command UI. A compatible LoRa radio is required; built-in radio availability is not assumed.

## Mission data service
Raspberry Pi 4B: telemetry ingestion through a verified interface, CSV/SQLite logging, and a local dashboard with charts and packet statistics.

## Planned telemetry fields
`schema_version, mission_id, sequence, uptime_ms, timestamp_utc, temperature_c, humidity_pct, pressure_hpa, battery_v, latitude, longitude, gps_fix, status_flags`

Mark unavailable sensor/GPS fields as invalid or unavailable; do not fabricate values.

## Link safety
No transmit code in Phase 1. Verify antenna and permitted frequency before transmission. Add packet validation, sequence numbers, acknowledgements, timeout handling and command allow-list before remote control.
