# Project ORBIT

## A Modular Satellite Engineering and Ground-Station Platform

Project ORBIT is a hands-on engineering initiative to design and
integrate a compact, telemetry-enabled satellite platform using embedded
computing, environmental sensing, LoRa communication, and a dedicated
ground station.

The project brings together onboard electronics, sensor acquisition,
power-system development, wireless telemetry, command handling, and
ground-based data visualization. It is intended to provide a practical
environment for exploring satellite-system concepts, embedded systems,
communications, and end-to-end mission operations.

**Project scope:** ORBIT is a ground-based engineering and educational
platform. It is not currently an orbital spacecraft, and any
satellite-like enclosure, solar-panel arrangement, mission dashboard, or
operational capability shown in concept artwork should be treated as a
design concept until implemented and validated.

![Project ORBIT detailed system
architecture](docs/ORBIT_Architecture_Detailed.jpg)

## Mission Objectives

-   Develop a modular onboard electronics platform based on ESP32-S3.
-   Integrate environmental sensors and optional positioning and
    timekeeping modules.
-   Establish a LoRa telemetry link between the onboard unit and a
    ground station.
-   Design and validate a command-and-response workflow, including
    status requests and acknowledgements.
-   Explore battery-backed operation and a solar-assisted charging
    concept after the power architecture is engineered and validated.
-   Build a ground station for live status, telemetry inspection,
    logging, and visualization.
-   Maintain reproducible firmware, hardware notes, test evidence, and
    engineering documentation in version control.

## System Overview

### ORBIT-SAT --- Onboard Unit

The onboard unit is planned around an ESP32-S3 platform and an SX1262
LoRa radio. Its intended responsibilities include:

-   Collecting environmental and system telemetry.
-   Reporting health and diagnostic information.
-   Providing local status output where the board display is identified
    and supported.
-   Transmitting telemetry after radio configuration, antenna
    compatibility, and regulatory requirements have been verified.
-   Receiving and processing ground commands once the communication
    protocol and safety controls are implemented.
-   Supporting future battery and solar-assisted power experiments.

### ORBIT-GS --- Ground Station

The ground station is intended to provide an operator-facing interface
for interacting with the onboard unit. Depending on the selected
hardware and completed integrations, it may provide:

-   Incoming telemetry and device-health status.
-   Command submission and acknowledgement tracking.
-   Radio link metrics and packet statistics.
-   Sensor charts and event history.
-   Local data logging and export.

The Hosyond ESP32-S3 touchscreen board is available for ground-station
experiments. Its role as the primary console and its LoRa integration
remain subject to hardware and interface verification.

### Data Logging and Visualization

A Raspberry Pi 4B (8 GB) is available for future data collection and
dashboard services. The planned role includes local telemetry storage,
data export, and browser-based visualization. The software stack and
interfaces will be selected as the implementation develops.

## Hardware Inventory and Status

  -----------------------------------------------------------------------
  Component               Current status          Intended role
  ----------------------- ----------------------- -----------------------
  Robocraze ESP32-S3 +    Ordered / arrival and   Primary onboard unit
  SX1262 LoRa board       exact revision to be    
                          confirmed               

  Hosyond ESP32-S3        Available               Ground-station
  2.8-inch touchscreen                            interface and display
                                                  experiments

  Raspberry Pi 4B, 8 GB   Available               Data logging and
                                                  dashboard host

  DHT22                   Available               Temperature and
                                                  humidity

  BMP280                  Available               Pressure and
                                                  temperature

  DS3231 RTC              Available               Real-time clock, if
                                                  required

  NEO-6M GPS              Available               Optional position and
                                                  time data

  Battery, charging and   Design and selection    Future power-system
  solar components        pending                 work

  Radio antenna and       Compatibility to be     Required before any
  pigtail                 verified                radio transmission
  -----------------------------------------------------------------------

"Available" means the component is in the current inventory; it does not
mean it has been integrated or tested in ORBIT. Exact board variants,
pin assignments, electrical interfaces, and radio compatibility must be
confirmed before implementation.

## Current Phase: Hardware Selection and Bring-Up

The project is currently in Phase 1. The first milestone is to establish
a reliable, repeatable baseline on the exact onboard board revision.

1.  Inspect the board markings, connectors, radio module, display, and
    revision.
2.  Connect over USB and identify the serial port and supported upload
    method.
3.  Build and flash the serial-only bring-up firmware after confirming
    the correct target configuration.
4.  Verify the `ORBIT-SAT BOOT OK` message and diagnostic output.
5.  Identify the display controller, interface, and board-specific pins
    from reliable documentation before enabling display output.
6.  Identify the SX1262 interface and pins, then verify radio
    initialization without transmitting.
7.  Record the confirmed hardware details and test results in the
    repository.

The initial PlatformIO target is provisional. It must not be treated as
confirmation of board-specific wiring or upload settings.

## Safety and Engineering Constraints

-   **No radio transmission by default.** Do not enable transmission
    until the exact board variant, antenna and connector compatibility,
    permitted frequency, regional regulations, and applicable operating
    requirements have been verified.
-   **No assumed pin mapping.** Confirm GPIO assignments and peripheral
    interfaces from reliable documentation or direct measurements.
-   **Power-system validation first.** Battery chemistry, cell
    configuration, charging controller, protection, regulator
    requirements, solar-panel characteristics, and load budget must be
    selected and reviewed before connecting a battery or solar source.
-   **Distinguish concepts from verified features.** Architecture
    diagrams and renderings may show planned elements. They are not
    evidence that those elements have been purchased, integrated, or
    tested.
-   **Preserve test evidence.** Record firmware versions, build results,
    serial logs, measured values, and known limitations as work
    progresses.

## Repository Layout

``` text
ORBIT/
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── ORBIT_Architecture.jpg
│   ├── HARDWARE_INVENTORY.md
│   ├── BRINGUP_CHECKLIST.md
│   └── PHASE_PLAN.md
├── firmware/
│   └── orbit_sat/
│       ├── platformio.ini
│       └── src/
│           └── main.cpp
└── ground-station/
    └── README.md
```

The listed documentation files should be checked against the current
repository; filenames may differ if the documentation has not yet been
added.

## Build the Initial Firmware

Open `firmware/orbit_sat/` in VS Code with the PlatformIO IDE, or run:

``` bash
cd firmware/orbit_sat
pio run
```

The initial firmware is intended for safe, serial-only bring-up. Confirm
the exact board target and upload method before flashing:

``` bash
pio run -t upload
pio device monitor
```

Do not run the upload command until the board variant, connection, and
upload settings have been confirmed. A successful build alone does not
prove that the firmware is appropriate for the physical board.

## Development Roadmap

1.  **Hardware identification and bring-up** --- confirm the board, USB
    connection, serial diagnostics, and target configuration.
2.  **Local status display** --- identify and validate the onboard
    display interface.
3.  **Radio interface validation** --- confirm SX1262 wiring and
    initialization without transmission.
4.  **Telemetry acquisition** --- integrate selected sensors and define
    a versioned telemetry message.
5.  **Ground-station link** --- implement a verified LoRa link, then
    test packet reception and link statistics under compliant
    conditions.
6.  **Commands and acknowledgements** --- define supported commands,
    validation, acknowledgements, timeouts, and fault handling.
7.  **Logging and visualization** --- connect the ground station to
    local storage and dashboards.
8.  **Power-system experiments** --- engineer and validate
    battery-backed and solar-assisted operation.
9.  **System verification and documentation** --- conduct controlled
    end-to-end tests and publish results, limitations, and build notes.

Each phase depends on verified results from the preceding phase. The
roadmap describes intended work, not completed functionality.

## Project Principles

-   Modular design and incremental integration.
-   Documented decisions and traceable test results.
-   Hardware compatibility verified before wiring or purchasing.
-   Safe defaults for radio and power electronics.
-   Clear separation between implemented, planned, and unverified
    capabilities.
-   Reproducible firmware builds and version-controlled documentation.

## Project Status

**Current milestone:** Phase 1 --- hardware selection and bring-up.

Progress should be reported from observed results and recorded evidence.
Features represented in concept artwork or the roadmap must not be
described as operational until they have been implemented and tested.
