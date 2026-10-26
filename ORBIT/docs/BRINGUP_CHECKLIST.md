# ORBIT Bring-Up Checklist

## Board identification and build

- [x] ORBIT firmware boot observed.
- [x] Visible PCB marking recorded as `HTIT-WB32LAF`; visible marking `V3.2`.
- [x] Packaging identifies ESP32/SX1262 and advertises 863–928 MHz.
- [x] PlatformIO build succeeded with `heltec_wifi_lora_32_V3`.
- [x] Serial status observed with `OLED=READY` and `state=SAFE_IDLE`.
- [x] OLED observed showing ORBIT status including `RADIO: SAFE / OFF` and `STATUS: READY`.
- [ ] Confirm exact board revision and interfaces against reliable manufacturer documentation.
- [ ] Repeat a clean build and retain its log.
- [ ] Confirm upload settings and serial port before any future upload.

## OLED

- [x] ORBIT status observed on the onboard OLED.
- [ ] Document the display driver/library, interface, and pin definitions used by the current source.
- [ ] Verify repeated cold boots and sustained operation.

## Radio — no transmission

- [ ] Verify radio variant and board-specific pins.
- [ ] Verify RF connector, pigtail, and antenna compatibility.
- [ ] Verify applicable Indian radio requirements and permitted operating conditions.
- [ ] If supported, test radio initialization without transmitting.
- [ ] Keep transmission disabled until compatibility and compliance checks are complete.

## Sensors and ground station

- [ ] Verify electrical interfaces, voltage levels, pin mapping, and addresses before wiring.
- [ ] Integrate sensors one at a time and validate readings.
- [ ] Represent missing/failed measurements as invalid or unavailable.
- [ ] Identify a compatible ground-station radio from existing inventory before considering purchases.
- [ ] Define packet format and transport before implementing the logger/dashboard.
- [ ] Validate received packets before display or storage.

## Power and field testing

- [ ] Keep initial development USB-powered.
- [ ] Engineer battery charging/protection and regulation before connecting a battery.
- [ ] Validate solar input and charging before connecting a panel.
- [ ] Record test setup, results, limitations, and firmware revision.
