# Hosyond Wi-Fi Coverage Mapper

PlatformIO / Arduino project for the Hosyond ESP32-S3 touchscreen board.

## Features

- 2.4 GHz Wi-Fi scanning
- Five configured network entries
- RSSI, channel and signal-quality reporting
- Touchscreen dashboard with signal bars
- IST timestamps through NTP
- CSV output over serial

## Build

Install PlatformIO and run:

    pio run

## Configure local credentials

Copy src/secrets.example.h to src/secrets.h and enter your Wi-Fi
password locally. Do not commit src/secrets.h.

## Upload

    pio run --target upload

Serial monitor:

    pio device monitor --baud 115200
