# Project ORBIT – Space Mission

Educational ESP32-S3 satellite simulator with SX1262 LoRa, sensors and a ground station. This is not an actual orbital satellite.

## Current phase
Phase 1 — hardware selection and bring-up.

## First milestone
When the Robocraze board arrives:
1. Inspect markings, connectors and exact model/revision.
2. Connect USB and confirm the serial port.
3. Build and flash the safe serial-only bring-up firmware.
4. Confirm `ORBIT-SAT BOOT OK` and diagnostics.
5. Identify the OLED controller/interface and board-specific pins from reliable documentation, then enable OLED output.
6. Identify SX1262 pins and verify initialization without transmitting.

No LoRa transmission is enabled in the starter firmware. Do not transmit until antenna compatibility, the exact board variant, permitted frequency and local operating requirements have been verified.

## Build
Open `firmware/orbit_sat/` in VS Code with PlatformIO IDE. The initial environment is a provisional generic ESP32-S3 target; it does not mean board-specific OLED/radio wiring is known.

```bash
cd firmware/orbit_sat
pio run
```

Do not upload until the exact board and upload method are confirmed.

## Intended local repository
`/Users/pradeepl/Documents/ESP32/ESP32-Projects-git`

Copy the `ORBIT` folder into that directory. This archive cannot write directly to your Mac filesystem.
