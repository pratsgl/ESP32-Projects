# Project ORBIT — ORBIT-SAT firmware

## Current verified status

- PlatformIO build succeeded from this directory using `heltec_wifi_lora_32_V3`.
- Build output reported ESP32-S3 and 8 MB flash for the selected target definition.
- ORBIT firmware booted on the received Heltec-branded board.
- OLED was observed displaying ORBIT status including `RADIO: SAFE / OFF` and `STATUS: READY`.
- Serial output included `OLED=READY` and `state=SAFE_IDLE`.

These observations do not independently confirm LoRa initialization/communication, sensor integration, ground-station operation, or battery/solar operation.

## Board identification

Visible PCB marking: `HTIT-WB32LAF`; visible marking: `V3.2`. Packaging identifies SX1262 and advertises 863–928 MHz. Verify exact hardware revision, RF variant, antenna/connector compatibility, and board-specific interfaces against reliable manufacturer documentation before adding radio functionality.

## Build

From the repository root:

```bash
cd ORBIT/firmware/orbit_sat
pio run -e heltec_wifi_lora_32_V3
```

For upload, connect the board and confirm serial port and upload settings first:

```bash
pio run -e heltec_wifi_lora_32_V3 -t upload
pio device monitor -b 115200
```

Rerun the build after source or configuration changes.

## Safety

- Keep RF transmission disabled until board/radio, antenna, frequency, and applicable operating requirements are verified.
- Do not connect a battery or solar panel until the power path has been engineered and validated.
- Do not assume sensors or ground-station interfaces are integrated just because they appear in the architecture.
