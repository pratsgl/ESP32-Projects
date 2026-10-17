# Hosyond ESP32-S3 Board Support

## Overview

Hardware target: **Hosyond ES3C28P family**

- **Display:** ILI9341V, 240 × 320 pixels
- **Touch controller:** FT6336G
- **Microcontroller:** ESP32-S3

This directory is the central reference location for shared Hosyond ESP32-S3 board-support code and manufacturer reference examples.

## Verified LCD Pin Mapping

| Signal | GPIO |
|---|---:|
| LCD CS | 10 |
| LCD DC | 46 |
| LCD SCLK | 12 |
| LCD MOSI | 11 |
| LCD MISO | 13 |
| LCD backlight | 45 |

### Touch Controller Pin Mapping

| Signal | GPIO |
|---|---:|
| Touch SDA | 16 |
| Touch SCL | 15 |
| Touch RESET | 18 |
| Touch INTERRUPT | 17 |

### Other Board Connections

| Signal | GPIO |
|---|---:|
| RGB LED | 42 |
| Battery ADC | 9 |
| Amplifier enable | 1 |

The amplifier enable is active-low. Verify the manufacturer's reference implementation before using additional peripherals.

## Repository Structure

```text
ESP32-Projects-git/
├── Hoysond/
│   ├── README.md
│   ├── spi_dev.h
│   └── reference/
│       ├── Simple_test.ino
│       └── spi_dev.h
└── Wifi-Scanner-Hoysond/
    ├── README.md
    ├── platformio.ini
    └── src/
        ├── main.cpp
        ├── spi_dev.h
        └── secrets.example.h
```

The manufacturer reference example is retained under `reference/`. The working Wi-Fi scanner is maintained separately in `Wifi-Scanner-Hoysond/`.

## Reusing Hosyond Drivers in Other Projects

**GitHub directory:**  
https://github.com/pratsgl/ESP32-Projects/tree/main/Hoysond

**Local development path:**

```text
/Users/pradeepl/Documents/ESP32/ESP32-Projects-git/Hoysond/
```

Use this maintained directory as the reference for future Hosyond projects.

### Current Reuse Approach

The reusable display driver has not yet been packaged and validated as a standalone PlatformIO library. Until that work is completed, use one of the following approaches.

1. **Copy the required driver files**

   Copy the required driver files into the new project's source directory. This is straightforward, but future driver changes must be copied into each project that uses them.

2. **Reference shared files locally**

   For projects within this repository, a shared driver can potentially be included using a relative path. Verify the include path and compile the project before relying on this approach.

3. **Create a reusable PlatformIO library later**

   The planned approach is to package the validated driver files as a proper PlatformIO library. Each project can then declare the library as a dependency instead of maintaining separate copies.

### Development Workflow

1. Maintain shared board-support changes in `Hoysond/`.
2. Preserve the verified GPIO mappings.
3. Use the manufacturer's reference implementation when investigating hardware behaviour.
4. Build and test affected projects on the actual Hosyond hardware.
5. Commit and push verified changes to GitHub.
6. Keep Wi-Fi passwords, API keys, and other private configuration out of Git.

## Related Project: Wi-Fi Scanner

The Wi-Fi scanner project is maintained separately:

```text
Wifi-Scanner-Hoysond/
```

It uses the Hosyond ESP32-S3 touchscreen to display nearby Wi-Fi network information. Refer to that project's README for its setup, build, and usage instructions.

## Future Work

- Extract the reusable display driver from the current implementation.
- Validate the driver independently of the Wi-Fi scanner.
- Package it as a PlatformIO library.
- Add a documented example showing how a new project can use the library.
- Document the touch controller interface and additional peripherals as they are validated.

## Licensing and Attribution

The manufacturer's reference example and driver code are retained for development and reference purposes. Before redistributing adapted manufacturer code, check the original license and any attribution requirements.

