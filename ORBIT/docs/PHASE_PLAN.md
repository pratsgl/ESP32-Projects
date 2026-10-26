# Project ORBIT Phased Plan

## Current status

**Phase 1 — board bring-up is in progress.** The official firmware path builds successfully for `heltec_wifi_lora_32_V3`. ORBIT firmware boot, OLED status, and serial `OLED=READY` / `state=SAFE_IDLE` output have been observed. LoRa communication, sensor acquisition, ground-station integration, Pi logging/dashboard, and solar/battery operation are not verified.

## Phases and exit criteria

1. **Board identification and bring-up — current.** Record markings; reproduce build/boot; document target and upload method.
2. **OLED validation.** Document the exact display driver/interface/pins used by the working firmware; verify repeated boots and sustained operation.
3. **Radio validation and packet link.** Verify radio variant, board pins, antenna path, and permitted operating conditions. Test initialization without transmission where supported.
4. **Sensor and telemetry integration.** Add sensors one at a time, validate readings, represent invalid data, and define a versioned packet schema.
5. **Ground-station and data logging.** Select a compatible radio from existing inventory; implement reception, validation, logging, and visualization.
6. **Commands and acknowledgements.** Add a strict allow-list, argument validation, acknowledgement tracking, bounded timeouts/retries, and safe error handling.
7. **Low-power and solar power.** Engineer and validate battery, protection, charging, regulation, and solar input before connection.
8. **Terrestrial mission scenarios and field tests.** Run controlled end-to-end tests and record behavior and limitations.
9. **Documentation and demonstration.** Consolidate architecture, decisions, build instructions, test evidence, limitations, and demo material.

Each phase depends on verified evidence from prior phases. This plan does not imply future functions are implemented.
