# Project ORBIT Architecture

**Status:** Phase 1 — hardware selection and bring-up

**Document scope:** Intended terrestrial system architecture and current implementation status

Project ORBIT is an educational satellite mission simulator built around ESP32-S3
hardware. It models selected spacecraft telemetry and ground operations on a
terrestrial test setup. It is not an actual satellite and has no orbital
deployment.

## 1. Purpose and scope

ORBIT is intended to teach embedded firmware, sensor integration, telemetry
design, low-power concepts, radio links, ground-station interfaces, data logging,
and mission-style fault handling. The planned satellite simulator is an ESP32-S3
board with an SX1262 LoRa radio. A separate ESP32-S3 touchscreen is intended to
act as the ground-station user interface, with a Raspberry Pi providing storage
and a local dashboard.

The current scope includes safe board bring-up first, followed by verified
peripheral integration and staged telemetry experiments. Planned functions in
this document are architectural goals; they are not claims that the hardware is
connected or the software exists. As of Phase 1, firmware is serial-diagnostics
only. OLED and SX1262 initialization, sensors, GPS, telemetry, ground software,
and command handling are not implemented.

Non-goals include launching or operating a real spacecraft, claiming orbital
readiness, assuming undocumented board wiring, or transmitting until the radio
variant, antenna path, permitted operating conditions, and applicable Indian
requirements have been verified.

The PlatformIO environment is provisional: it selects the generic
`esp32-s3-devkitc-1` board definition. It is useful as a starter configuration,
not confirmation that it matches the ordered Robocraze board or its upload
method.

## 2. System context

Legend: `[E]` existing/owned, `[O]` ordered, `[P]` planned software or function,
`[U]` unverified interface or compatibility. Arrow labels show intended data
direction. Components marked `[P]` are not operational.

```text
                         Developer Mac [E]
                  edit, build, serial monitoring
                              |
                              | USB serial [U]
                              v
  Environmental sensors [E]    ORBIT-SAT [O]       Optional GPS [E]
  BMP280, DHT22, DS3231 ------> ESP32-S3 + SX1262 <--- NEO-6M
  (DS3231 optional)             onboard OLED [O]      [P integration]
         [P integration]                |
                                         | LoRa [P, U]
                                         v
                              ORBIT-GS [E]
                    Hosyond ESP32-S3 touchscreen
                    2.8-inch, 240 x 320 display
                                         |
                                         | ground radio interface
                                         | [U: radio/compatibility]
                                         v
                                  LoRa radio [P]
                                         |
                                         | telemetry/commands [P, U]
                                         v
                               Raspberry Pi 4B, 8GB [E]
                           ingestion, logs, dashboard [P]

  GitHub repository [E] <---- version-control workflow ----> Developer Mac
```

The Robocraze ESP32-S3/SX1262 board and its 0.96-inch OLED are ordered; the
delivery date is not confirmed. The environmental sensors, GPS, Hosyond
touchscreen, and Raspberry Pi are recorded as already owned. A compatible
ground-station LoRa radio has not been established: the inventory includes
unidentified LoRa modules, but their exact models and frequencies still need to
be recorded and checked. No sensor wiring or board pin mapping is established.

## 3. Detailed component architecture

The diagram separates the current serial-only firmware from planned firmware
services. The arrows between planned components describe a target design, not
working code.

```text
 ORBIT-SAT firmware (current target: Arduino on ESP32-S3)
 +--------------------------------------------------------------+
 | [E: implemented]                                             |
 |   setup/loop -> USB serial boot banner and periodic status   |
 |                  |                                           |
 |                  +--> heap, chip model/revision, CPU MHz     |
 |                                                              |
 | [P: not implemented]                                         |
 |   Sensor abstraction <--- BMP280, DHT22, optional DS3231     |
 |          |                                                   |
 |          +-------------- Optional NEO-6M GPS                 |
 |          v                                                   |
 |   validation and validity flags                              |
 |          v                                                   |
 |   telemetry packet builder                                   |
 |          v                                                   |
 |   radio interface -> SX1262 -> LoRa link                     |
 |                                                              |
 |   command parser/allow-list, acknowledgements and recovery    |
 +--------------------------------------------------------------+
                               ^             |
                    commands  |             | telemetry
                               |             v
 +----------------------------+-------------------------------+
 | ORBIT-GS [E: touchscreen board; application P]                |
 | Ground-station interface [P]: compatible LoRa radio [U]       |
 | Display/command UI [P] -> radio interface [P]                 |
 +----------------------------+-------------------------------+
                              |
                   data transport to Pi [P, U]
                              v
 +--------------------------------------------------------------+
 | Raspberry Pi 4B [E: hardware; software P]                     |
 | Ingestion/packet validation -> data logger -> dashboard       |
 |                              CSV or SQLite [P]                 |
 +--------------------------------------------------------------+
```

The current source in `firmware/orbit_sat/src/main.cpp` implements only
`setup()` and `loop()` for serial diagnostics. It intentionally has no OLED,
sensor, GPS, SX1262, packet, receiver, logger, dashboard, or command code.

## 4. Telemetry data flow

This is the intended flow once each link and component has been implemented and
verified. The current firmware does not perform any of these telemetry steps.

```text
Sensors/GPS [P]
     |
     v
Acquisition [P] -> validation and validity marking [P]
     -> packet creation [P] -> LoRa transmission [P, U]
     -> ground radio reception [P, U] -> packet validation [P]
     -> logging on Raspberry Pi [P] -> dashboard [P]
```

Values should be marked unavailable or invalid when a sensor is absent, a
reading fails validation, or GPS has no valid fix. Do not substitute fabricated
measurements. Reception must reject malformed packets before logging or display.

## 5. Command and acknowledgement flow

Commands and acknowledgements are later-phase functionality. No command path is
implemented, and no remote control should be treated as available.

```text
Ground UI [P] -> ground radio [P, U] -> LoRa command [P, U]
     -> satellite command parser [P]
     -> allow-list and argument/range validation [P]
     -> accepted/rejected acknowledgement [P]
     -> ground acknowledgement handling [P]
     -> timeout/retry policy and error reporting [P]

Invalid or unknown command -> reject safely, record/report error [P]
No acknowledgement before deadline -> timeout and report link failure [P]
```

Command identifiers, authorization model, retry limits, and timeout durations
remain design decisions. Commands that can affect power, radio behavior, or
hardware should be constrained to an explicit allow-list with validated
arguments.

## 6. Hardware inventory and responsibility

Availability reflects the inventory in `HARDWARE_INVENTORY.md`; integration
status reflects the current repository state. “Existing” means recorded as
owned, not necessarily tested or connected.

| Component | Purpose | Availability | Current integration status |
|---|---|---|---|
| Robocraze V3 ESP32-S3 + SX1262 board | Satellite simulator MCU and planned LoRa radio | Ordered; delivery unconfirmed | Firmware target not confirmed; not integrated |
| Onboard 0.96-inch OLED | Local status display | Ordered with board | Controller, interface and pins unknown; not integrated |
| BMP280 | Pressure and temperature sensing | Existing | Not wired or read by firmware |
| DHT22 | Temperature and humidity sensing | Existing | Not wired or read by firmware |
| DS3231 RTC | Optional real-time clock | Existing | Not wired or read by firmware |
| NEO-6M GPS | Optional position and fix data | Existing | Not wired or read by firmware |
| DHT11 | Alternate sensor in inventory | Existing | Not selected for current proposed schema; not integrated |
| Hosyond ESP32-S3 touchscreen, 2.8-inch 240 x 320 | Ground-station display and planned command UI | Existing | No ground-station application implemented |
| Ground-station LoRa radio | Receive telemetry and later send commands | Compatibility unverified; possible modules on hand | Exact model/frequency and compatibility not established |
| Raspberry Pi 4B, 8GB | Planned ingestion, logging and dashboard host | Existing | No ORBIT service implemented |
| Antenna and pigtail | RF connection for radio tests | Not yet purchased | Connector and antenna requirements unknown |
| Battery, solar panel, charging/protection | Future independent power design | Not yet purchased | No power design or integration |
| Developer Mac | Edit, build and monitor firmware | Existing development equipment | Repository and PlatformIO workflow documented; board connection pending |
| GitHub repository | Versioned project source and documentation | Existing repository | ORBIT docs and starter firmware present |

## 7. Interfaces and protocols

| Interface | Intended use | Status and constraints |
|---|---|---|
| USB serial | Firmware upload/monitoring and Phase 1 diagnostics | Firmware uses Arduino `Serial` at 115200 baud and waits up to 3 seconds for serial. Exact board USB mode, port, and upload method must be confirmed. |
| I2C | Possible bus for BMP280, DS3231, or OLED, depending on exact hardware | Possible only; board pins, device addresses, bus sharing, voltage levels, and actual device interfaces are unverified. Do not infer wiring. |
| SPI | Expected class of interface to investigate for SX1262 and potentially other hardware | Exact radio wiring, including SPI signals and control pins, is unverified. No SPI code exists. |
| LoRa / SX1262 | Planned satellite-to-ground telemetry and later command link | Not initialized and no transmission code exists. Radio variant, frequency, antenna, legal operating conditions, and ground compatibility are unresolved. |
| Ground station to Raspberry Pi | Carry validated ground-station data to ingestion/logger | Proposed connection only. Transport (for example, serial, USB, or network) has not been selected or verified. |
| Sensor/GPS links | Acquire environmental, time, and optional position data | Wiring and electrical compatibility are unverified; no acquisition code exists. |

The PlatformIO file declares `platform = espressif32`, `framework = arduino`,
`board = esp32-s3-devkitc-1`, `monitor_speed = 115200`, and USB CDC build flags.
The board definition is explicitly provisional. Confirm the exact ordered board,
USB behavior, and upload method before treating this environment as final or
uploading firmware.

## 8. Telemetry schema

The following is a proposed logical schema, based on the project documentation.
It is not a finalized wire format or an implemented packet definition.

| Field | Meaning | Validity guidance |
|---|---|---|
| `schema_version` | Version of the telemetry schema | Required; receiver should reject unsupported versions or handle them explicitly |
| `mission_id` | Identifier for a simulator run/mission | Required; format and assignment policy TBD |
| `sequence` | Per-sender packet sequence number | Required for gap and duplicate detection; wrap behavior TBD |
| `uptime_ms` | Satellite simulator uptime in milliseconds | Available from MCU runtime; current diagnostics print this value, but it is not packetized |
| `timestamp_utc` | UTC timestamp when known | Mark unavailable if no valid time source; RTC/GPS source and synchronization policy TBD |
| `temperature_c` | Temperature in degrees Celsius | Include validity; choose sensor source and conflict policy before implementation |
| `humidity_pct` | Relative humidity percentage | Mark invalid/unavailable when DHT22 reading is absent or fails validation |
| `pressure_hpa` | Pressure in hectopascals | Mark invalid/unavailable when BMP280 reading is absent or fails validation |
| `battery_v` | Measured battery voltage | Unavailable until a verified measurement circuit exists; do not infer from supply state |
| `latitude` | Latitude coordinate | Valid only with an accepted GPS fix |
| `longitude` | Longitude coordinate | Valid only with an accepted GPS fix |
| `gps_fix` | Whether a GPS fix is valid | False/no-fix must make coordinates unavailable, not zero-valued fabricated locations |
| `status_flags` | Bitset or structured health/status indicators | Definition TBD; should report sensor validity and relevant fault state |

The eventual encoding must define numeric representation, units, byte order or
text format, maximum size, and validation rules. Prefer explicit validity
indicators or a documented null/sentinel convention so unavailable values
cannot be confused with real measurements. Never fabricate sensor or GPS data.

## 9. Power architecture

The initial development stage is intended to run from USB power while the board
is connected for serial bring-up. The exact board power path and current limits
must be confirmed from reliable board documentation before adding peripherals.

Battery and solar operation are future design work. The inventory says battery,
solar panel, and charging/protection components have not been purchased. Do not
assume the ordered board includes a suitable charger, protection, or solar input.
Before connecting a battery, define the cell type, charging and protection
design, voltage/current limits, regulator needs, and safe wiring based on
verified component specifications. Battery voltage telemetry also needs a
verified measurement circuit; it is not currently available in firmware.

## 10. Safety, reliability and fault handling

**Radio operation:** There is no transmit code in Phase 1. Before any
transmission, identify the exact radio variant and frequency, verify antenna and
connector compatibility, and establish permitted operating conditions in India
from authoritative current requirements. Do not infer a legal frequency or
antenna specification from the board name or an unidentified module.

**Packet/link safeguards:** Packet framing, integrity checks (such as a
checksum/CRC at the application layer if needed), sequence tracking, duplicate
detection, malformed-packet rejection, and timeout behavior are planned. The
radio protocol may provide some link-layer checks, but the selected device and
configuration have not been verified. These safeguards are not implemented.

**Commands:** A strict command allow-list, argument validation, safe rejection
of invalid commands, acknowledgement reporting, and bounded timeout/retry
behavior are planned. They are not implemented. No unauthenticated or
unvalidated command should be allowed to trigger an action.

**Recovery and health:** Current firmware reports uptime, free heap, chip model,
chip revision, and CPU frequency every five seconds over serial. Watchdog policy,
sensor fault handling, radio recovery, persistent fault logging, and restart
behavior are not implemented. Later designs should define which faults trigger
retry, degraded operation, or controlled restart, and should avoid reset loops.

**Data quality:** Missing sensors and missing GPS fixes must be represented as
invalid/unavailable. The receiver and dashboard should preserve validity state
and avoid presenting stale or malformed values as current measurements.

## 11. Deployment topology

This topology is an intended **terrestrial experiment** using development
boards. It is not an orbital deployment.

```text
 Terrestrial test area

 +----------------------+       LoRa link [P, U]       +----------------------+
 | ORBIT-SAT [O]        | <--------------------------> | ORBIT-GS [E]         |
 | ESP32-S3 + SX1262    |   telemetry / commands [P]  | ESP32-S3 touchscreen |
 | OLED [O]             |                              | compatible radio [U] |
 +----------+-----------+                              +----------+-----------+
            | USB serial [U]                                      |
            |                                                     | data transport
            v                                                     | [P, U]
      Developer Mac [E]                                          v
        build/monitor                                  +----------------------+
            |                                          | Raspberry Pi 4B [E]  |
            +---- GitHub repository                    | logger/dashboard [P] |
                 source/docs                           +----------------------+
```

The radio arrows indicate the intended bidirectional link after RF checks and
software implementation. They do not authorize or imply that transmission is
currently possible.

## 12. Development phases and acceptance criteria

The existing nine-phase plan is retained below. **Current phase: Phase 1.**

1. **Phase 1 — Hardware selection and bring-up (current).** Identify exact
   board/revision, verify USB serial and upload method, build for the confirmed
   target, and confirm `ORBIT-SAT BOOT OK` plus recurring diagnostics. After
   identifying the OLED and pins, validate the display. Identify SX1262 pins
   and check initialization without transmitting if supported.
2. **Phase 2 — Board bring-up and OLED validation.** Complete board-specific
   initialization only from verified documentation and confirm display output.
3. **Phase 3 — LoRa packet communication.** Verify radio, antenna, ground-radio
   compatibility, and permitted operating conditions before staged packet
   communication.
4. **Phase 4 — Sensor and telemetry integration.** Add selected sensors, data
   validation, validity representation, and the versioned telemetry schema.
5. **Phase 5 — Ground-station dashboard and data logging.** Implement ground
   display, verified Pi transport, logging, and dashboard views.
6. **Phase 6 — Commands, acknowledgements and fault handling.** Add validated
   allow-listed commands, acknowledgements, timeouts, and error handling.
7. **Phase 7 — Low-power operation and solar power.** Design and validate the
   battery, charging/protection, and solar system before connection.
8. **Phase 8 — Mission simulation and field testing.** Run documented
   terrestrial mission scenarios and assess data/link behavior.
9. **Phase 9 — Documentation, academic report and demonstration.** Consolidate
   design decisions, results, limitations, and demonstration material.

The Phase 1 checklist includes OLED output and non-transmitting SX1262
initialization as bring-up milestones after board-specific verification. The
current firmware cannot satisfy those hardware milestones: it only provides
serial output, and its current boot banner explicitly says the OLED and radio
are pending.

## 13. Open decisions and risks

- Exact Robocraze board model, PCB revision, module markings, and arrival status.
- Board-specific GPIO mapping, including verified OLED and SX1262 pins.
- OLED controller, bus, address if applicable, and supported driver library.
- SX1262 SPI/control interface details: SPI signals, RESET, BUSY, DIO mapping.
- Exact satellite radio variant and frequency; applicable Indian operating
  requirements and permitted use.
- RF connector type, antenna and pigtail compatibility/specification.
- Which existing LoRa modules are available and whether one is compatible with
  ORBIT-SAT for the ground station.
- Ground-station-to-Raspberry-Pi transport and framing.
- Sensor and GPS voltage levels, wiring, bus/address conflicts, and selection
  policy for duplicate temperature sources.
- RTC/GPS time authority and behavior when UTC is unavailable.
- Battery chemistry, capacity, charger/protection, solar design, and power
  measurement circuit.
- Packet format, integrity mechanism, sequence wrap handling, duplicate
  retention window, timeout/retry policy, and command authorization model.
- Whether the provisional PlatformIO board definition and USB build flags match
  the exact ordered board and chosen upload path.

Until these decisions are resolved, peripheral pins, frequencies, antenna
specifications, power ratings, and radio ranges remain unspecified. No measured
radio range is available.

## 14. Repository layout

Current ORBIT files, with other possible root-level project files omitted:

```text
ORBIT/
|-- README.md
|-- docs/
|   |-- ARCHITECTURE.md
|   |-- BRINGUP_CHECKLIST.md
|   |-- HARDWARE_INVENTORY.md
|   `-- PHASE_PLAN.md
|-- firmware/
|   `-- orbit_sat/
|       |-- platformio.ini
|       `-- src/
|           `-- main.cpp
`-- ground-station/
    `-- README.md
```

The ground-station README is present, but no ground-station firmware or Pi
service implementation appears in this listed project structure.

## 15. First bring-up sequence

Follow the existing board arrival checklist and keep each step within the
verified hardware facts:

1. Photograph both sides of the board. Record its exact product name, PCB
   revision, module markings, connector types, and battery connector details.
2. Before connecting power, inspect the board. Do not connect a battery or
   external antenna until their specifications and compatibility are confirmed.
3. Connect USB with a known-good data cable. Compare `/dev/cu.*` device listings
   before and after connection on macOS and identify the serial device.
4. Confirm the exact board target and upload method from reliable board
   documentation. Treat `esp32-s3-devkitc-1` as provisional. Build first; upload
   only after target and method are confirmed.
5. Open the serial monitor at 115200 baud. Confirm `ORBIT-SAT BOOT OK` and the
   recurring diagnostic fields. The current firmware waits up to three seconds
   for serial and prints status about every five seconds.
6. Identify the OLED controller, interface, address if applicable, and pins
   using exact board documentation. Confirm library compatibility, then add and
   validate OLED initialization.
7. Identify SX1262 SPI and control pins, radio variant, RF connector, and
   antenna requirements. Add initialization only after pin verification. Test
   initialization without transmitting if the selected driver supports it.
8. Keep transmission disabled until compatibility and permitted operating
   conditions have been checked. Add sensors, GPS, telemetry, and ground
   services only in later, documented steps.

## Assumptions and status conventions

- Availability labels reflect the existing inventory document, not independent
  inspection or testing.
- “Planned” describes desired future architecture; it does not imply that code
  or a working connection exists.
- The context diagrams assume the planned roles for the ordered satellite board,
  existing ground touchscreen, and existing Raspberry Pi. Every unresolved
  interface is marked unverified.
- No GPIO, bus assignment, address, radio frequency, antenna specification,
  power rating, or measured range has been inferred.
- The exact board variant and current Indian radio requirements must be verified
  before RF operation.
