# ORBIT Ground Station

**Status: planned; no end-to-end ground-station link is verified.**

## Available hardware

- Hosyond ESP32-S3 touchscreen, 2.8-inch 240×320.
- Raspberry Pi 4B, 8 GB.
- Several LoRa modules are listed in the inventory, but exact models, bands, and compatibility must be checked before selecting one.

## Intended responsibilities

- **Hosyond:** local status/telemetry interface and, after compatibility verification, radio interface.
- **Raspberry Pi:** telemetry ingestion, validated packet storage (CSV or SQLite), and a local browser dashboard.

## Integration gates

1. Identify an existing radio module compatible with ORBIT-SAT and permitted operating conditions before purchasing anything.
2. Define packet format and transport before implementing the UI or logger.
3. Validate incoming packets and mark missing/invalid data explicitly.
4. Add commands only after telemetry reception is reliable and command validation/acknowledgement rules are defined.

No radio compatibility, telemetry reception, command path, Pi service, or dashboard should be considered operational until tested and documented.
