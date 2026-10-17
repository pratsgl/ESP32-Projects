# Hosyond ESP32-S3 Board Support

Hardware target: Hosyond ES3C28P family
Display: ILI9341V, 240 x 320
Touch controller: FT6336G

## Verified LCD pin mapping

- LCD CS: GPIO 10
- LCD DC: GPIO 46
- LCD SCLK: GPIO 12
- LCD MOSI: GPIO 11
- LCD MISO: GPIO 13
- LCD backlight: GPIO 45

The manufacturer reference example is retained under reference/.
The working Wi-Fi scanner is maintained separately in Wifi-Scanner-Hoysond/.

The reusable English-language display library will be extracted and
validated as a subsequent step. Check the manufacturer's original
license and attribution requirements before redistributing adapted code.
