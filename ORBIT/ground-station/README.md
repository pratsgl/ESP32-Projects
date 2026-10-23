# ORBIT Ground Station (planned)

Available hardware: Hosyond ESP32-S3 with 2.8-inch touchscreen and Raspberry Pi 4B (8GB).

Planned split:
- Hosyond: local touchscreen UI and, after compatibility is confirmed, LoRa interface.
- Raspberry Pi: telemetry ingestion, CSV/SQLite logging and local web dashboard.

No radio compatibility is assumed. Verify whether an existing LoRa module can be safely interfaced before buying another one.
