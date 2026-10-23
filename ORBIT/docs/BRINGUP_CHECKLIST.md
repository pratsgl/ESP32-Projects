# Board arrival checklist

## Before power
- [ ] Photograph both sides of PCB.
- [ ] Record exact product name, PCB revision and module markings.
- [ ] Inspect USB, OLED, RF connector and battery connector.
- [ ] Do not connect a battery or external antenna until specifications are confirmed.

## USB and serial
- [ ] Connect using a known-good USB data cable.
- [ ] On macOS, compare `ls /dev/cu.*` before and after connecting.
- [ ] Confirm the new serial device.
- [ ] Confirm board target and upload method from board documentation.
- [ ] Build before upload; do not assume the provisional target is final.
- [ ] Open serial monitor at 115200 baud.
- [ ] Confirm `ORBIT-SAT BOOT OK` and recurring status lines.

## OLED
- [ ] Confirm controller/interface and pins from exact board documentation.
- [ ] Confirm library compatibility.
- [ ] Add OLED initialization only after verification.

## SX1262
- [ ] Confirm SPI, RESET, BUSY and DIO pins from exact board documentation.
- [ ] Confirm RF connector and antenna specification.
- [ ] Test radio initialization without transmitting if supported.
- [ ] Keep transmit disabled until frequency and antenna checks are complete.
