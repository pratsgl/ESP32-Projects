#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <string.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "spi_dev.h"
#include "secrets.h"

// ============================================================
// Hosyond ES3C28P - ESP32-S3, ILI9341V, 240 x 320
// Manufacturer SPI driver, verified on the actual display.
// ============================================================

#define LCD_WIDTH  240
#define LCD_HEIGHT 320

#define WR_RAM_CMD 0x2C
#define SET_X_CMD  0x2A
#define SET_Y_CMD  0x2B

static constexpr uint16_t BLACK  = 0x0000;
static constexpr uint16_t WHITE  = 0xFFFF;
static constexpr uint16_t RED    = 0xF800;
static constexpr uint16_t GREEN  = 0x07E0;
static constexpr uint16_t CYAN   = 0x07FF;
static constexpr uint16_t YELLOW = 0xFFE0;
static constexpr uint16_t ORANGE = 0xFD20;
static constexpr uint16_t GREY   = 0x8410;

static constexpr uint32_t SCAN_INTERVAL_MS = 5000;
static constexpr uint32_t NTP_SYNC_WAIT_MS = 15000;
static constexpr uint32_t WIFI_CONNECT_WAIT_MS = 15000;

static constexpr long IST_OFFSET_SECONDS = 19800;
static constexpr int DAYLIGHT_OFFSET_SECONDS = 0;

static const char* TIME_WIFI_SSID = "Airtel_prad_9003";
static const char* LOCATION = "Living Room";

struct KnownNetwork {
    const char* ssid;
    const char* routerLocation;
    const char* displayName;
};

KnownNetwork networks[] = {
    {"Airtel_prad_9003",   "Living Room", "Airtel"},
    {"Jio-2g-pratsgl",     "Living Room", "Jio Main"},
    {"Jio-2Gpratsgl_EXT",  "Living Room", "Jio Ext"},
    {"Pratsgl-1stfloor2g", "First Floor", "1st Floor"},
    {"Pratsgl2Gslr",       "Terrace",     "Terrace"}
};

static constexpr size_t NETWORK_COUNT =
    sizeof(networks) / sizeof(networks[0]);

struct NetworkReading {
    bool found;
    int rssi;
    int channel;
};

NetworkReading readings[NETWORK_COUNT] = {};

SPIClass spi = SPIClass(SPI_PORT);
bool lock_flag = true;
bool displayReady = false;
bool timeSynchronized = false;
uint32_t scanNumber = 0;

// Text canvas: 1-bit monochrome, stored in RAM.
GFXcanvas1 canvas(LCD_WIDTH, LCD_HEIGHT);

// ============================================================
// Manufacturer SPI and LCD functions
// ============================================================

void SPI_Start_Write(void) {
    if (lock_flag) {
        lock_flag = false;
        spi.beginTransaction(
            SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE)
        );
        LCD_CS_LOW;
        SET_SPI_WRITE_MODE;
    }
}

void SPI_End_Write(void) {
    if (!lock_flag) {
        lock_flag = true;
        LCD_CS_HIGH;
        SET_SPI_READ_MODE;
        spi.endTransaction();
    }
}

void LCD_Write_Reg(uint8_t val) {
    SPI_Start_Write();
    LCD_DC_LOW;
    spi_write_8bit(val);
    SPI_End_Write();
}

void LCD_Write_Data_8Bit(uint8_t val) {
    SPI_Start_Write();
    LCD_DC_HIGH;
    spi_write_8bit(val);
    SPI_End_Write();
}

void LCD_Write_Data_16Bit(uint16_t val) {
    SPI_Start_Write();
    LCD_DC_HIGH;
    spi_write_16bit(val);
    SPI_End_Write();
}

void LCD_Set_Windows(
    uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey
) {
    LCD_Write_Reg(SET_X_CMD);
    LCD_Write_Data_16Bit(sx);
    LCD_Write_Data_16Bit(ex);
    LCD_Write_Reg(SET_Y_CMD);
    LCD_Write_Data_16Bit(sy);
    LCD_Write_Data_16Bit(ey);
    LCD_Write_Reg(WR_RAM_CMD);
}

void SPI_Init(void) {
    spi.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, -1);
}

void Lcd_Init(void) {
    LCD_Write_Reg(0xCF);
    LCD_Write_Data_8Bit(0x00);
    LCD_Write_Data_8Bit(0xC1);
    LCD_Write_Data_8Bit(0x30);

    LCD_Write_Reg(0xED);
    LCD_Write_Data_8Bit(0x64);
    LCD_Write_Data_8Bit(0x03);
    LCD_Write_Data_8Bit(0x12);
    LCD_Write_Data_8Bit(0x81);

    LCD_Write_Reg(0xE8);
    LCD_Write_Data_8Bit(0x85);
    LCD_Write_Data_8Bit(0x00);
    LCD_Write_Data_8Bit(0x78);

    LCD_Write_Reg(0xCB);
    LCD_Write_Data_8Bit(0x39);
    LCD_Write_Data_8Bit(0x2C);
    LCD_Write_Data_8Bit(0x00);
    LCD_Write_Data_8Bit(0x34);
    LCD_Write_Data_8Bit(0x02);

    LCD_Write_Reg(0xF7);
    LCD_Write_Data_8Bit(0x20);

    LCD_Write_Reg(0xEA);
    LCD_Write_Data_8Bit(0x00);
    LCD_Write_Data_8Bit(0x00);

    LCD_Write_Reg(0xC0);
    LCD_Write_Data_8Bit(0x13);

    LCD_Write_Reg(0xC1);
    LCD_Write_Data_8Bit(0x13);

    LCD_Write_Reg(0xC5);
    LCD_Write_Data_8Bit(0x22);
    LCD_Write_Data_8Bit(0x35);

    LCD_Write_Reg(0xC7);
    LCD_Write_Data_8Bit(0xBD);

    LCD_Write_Reg(0x21);

    LCD_Write_Reg(0x36);
    LCD_Write_Data_8Bit(0x08);

    LCD_Write_Reg(0xB6);
    LCD_Write_Data_8Bit(0x0A);
    LCD_Write_Data_8Bit(0xA2);

    LCD_Write_Reg(0x3A);
    LCD_Write_Data_8Bit(0x55);

    LCD_Write_Reg(0xF6);
    LCD_Write_Data_8Bit(0x01);
    LCD_Write_Data_8Bit(0x30);

    LCD_Write_Reg(0xB1);
    LCD_Write_Data_8Bit(0x00);
    LCD_Write_Data_8Bit(0x1B);

    LCD_Write_Reg(0xF2);
    LCD_Write_Data_8Bit(0x00);

    LCD_Write_Reg(0x26);
    LCD_Write_Data_8Bit(0x01);

    LCD_Write_Reg(0xE0);
    const uint8_t gammaPositive[] = {
        0x0F, 0x35, 0x31, 0x0B, 0x0E,
        0x06, 0x49, 0xA7, 0x33, 0x07,
        0x0F, 0x03, 0x0C, 0x0A, 0x00
    };
    for (uint8_t value : gammaPositive) {
        LCD_Write_Data_8Bit(value);
    }

    LCD_Write_Reg(0xE1);
    const uint8_t gammaNegative[] = {
        0x00, 0x0A, 0x0F, 0x04, 0x11,
        0x08, 0x36, 0x58, 0x4D, 0x07,
        0x10, 0x0C, 0x32, 0x34, 0x0F
    };
    for (uint8_t value : gammaNegative) {
        LCD_Write_Data_8Bit(value);
    }

    LCD_Write_Reg(0x11);
    delay(120);
    LCD_Write_Reg(0x29);
}

void Write_color_Block(uint16_t color, uint32_t len) {
    volatile uint32_t* wr_buf = spi_write_buf;

    const uint32_t color32 =
        (static_cast<uint32_t>(color << 8 | color >> 8) << 16) |
         static_cast<uint32_t>(color << 8 | color >> 8);

    uint8_t num = len % 32;
    uint8_t i = 0;

    if (num) {
        while (*spi_cmd & SPI_USR) {}

        while (i < num) {
            *wr_buf++ = color32;
            i += 2;
        }

        *spi_write_len = num * 16 - 1;
        *spi_cmd = SPI_UPDATE;
        while (*spi_cmd & SPI_UPDATE) {}
        *spi_cmd = SPI_USR;

        if (len < 32) {
            while (*spi_cmd & SPI_USR) {}
            return;
        }

        len -= num;

        for (i /= 2; i < 16; ++i) {
            *wr_buf++ = color32;
        }
    }

    while (*spi_cmd & SPI_USR) {}

    if (!num) {
        while (i < 16) {
            *wr_buf++ = color32;
            ++i;
        }
    }

    *spi_write_len = 511;

    while (len) {
        while (*spi_cmd & SPI_USR) {}
        *spi_cmd = SPI_UPDATE;
        while (*spi_cmd & SPI_UPDATE) {}
        *spi_cmd = SPI_USR;
        len -= 32;
    }

    while (*spi_cmd & SPI_USR) {}
}

void LCD_Clear_Screen(
    uint16_t sx, uint16_t sy,
    uint16_t w, uint16_t h,
    uint16_t color
) {
    if (sx >= LCD_WIDTH || sy >= LCD_HEIGHT) return;
    if (sx + w > LCD_WIDTH || sy + h > LCD_HEIGHT) return;
    if (w < 1 || w > LCD_WIDTH || h < 1 || h > LCD_HEIGHT) return;

    LCD_Set_Windows(sx, sy, sx + w - 1, sy + h - 1);
    LCD_DC_HIGH;
    SPI_Start_Write();
    Write_color_Block(color, static_cast<uint32_t>(w) * h);
    SPI_End_Write();
}

// ============================================================
// Signal quality
// ============================================================

const char* qualityLabel(int rssi) {
    if (rssi >= -50) return "Excellent";
    if (rssi >= -60) return "Good";
    if (rssi >= -67) return "Fair";
    if (rssi >= -75) return "Weak";
    return "Very weak";
}

uint16_t qualityColor(int rssi) {
    if (rssi >= -50) return GREEN;
    if (rssi >= -60) return CYAN;
    if (rssi >= -67) return YELLOW;
    if (rssi >= -75) return ORANGE;
    return RED;
}

// ============================================================
// Dashboard rendering
// ============================================================

// Draw opaque white text onto a black display. The complete
// display is cleared before this function runs, so no pixels
// from the previous frame should remain visible.
void drawCanvasText() {
    for (int y = 0; y < LCD_HEIGHT; ++y) {
        int x = 0;

        while (x < LCD_WIDTH) {
            while (x < LCD_WIDTH && !canvas.getPixel(x, y)) {
                ++x;
            }

            if (x >= LCD_WIDTH) break;

            const int startX = x;

            while (x < LCD_WIDTH && canvas.getPixel(x, y)) {
                ++x;
            }

            LCD_Clear_Screen(
                static_cast<uint16_t>(startX),
                static_cast<uint16_t>(y),
                static_cast<uint16_t>(x - startX),
                1,
                WHITE
            );
        }
    }
}

void drawSignalBars(int x, int y, int rssi) {
    int activeBars = 1;

    if (rssi >= -50) activeBars = 5;
    else if (rssi >= -60) activeBars = 4;
    else if (rssi >= -67) activeBars = 3;
    else if (rssi >= -75) activeBars = 2;

    for (int i = 0; i < 5; ++i) {
        const int barHeight = 5 + i * 4;

        LCD_Clear_Screen(
            x + i * 7,
            y + 20 - barHeight,
            5,
            barHeight,
            i < activeBars ? qualityColor(rssi) : GREY
        );
    }
}

void renderDashboard() {
    if (!displayReady) return;

    // Critical fix: clear the physical display on every refresh.
    // This removes old characters when values or text lengths
    // change between consecutive scans.
    LCD_Clear_Screen(0, 0, LCD_WIDTH, LCD_HEIGHT, BLACK);

    canvas.fillScreen(0);
    canvas.setTextWrap(false);
    canvas.setTextSize(1);
    canvas.setTextColor(1);

    canvas.setCursor(8, 7);
    canvas.println("WIFI COVERAGE MAP");

    canvas.setCursor(8, 20);
    canvas.print("Location: ");
    canvas.println(LOCATION);

    canvas.setCursor(8, 33);
    canvas.print("Scan: ");
    canvas.print(scanNumber);
    canvas.println("  Band: 2.4GHz");

    canvas.drawFastHLine(6, 47, 228, 1);

    size_t foundCount = 0;

    for (size_t i = 0; i < NETWORK_COUNT; ++i) {
        const int y = 54 + static_cast<int>(i) * 44;

        canvas.setCursor(8, y);
        canvas.println(networks[i].displayName);

        canvas.setCursor(8, y + 12);

        if (readings[i].found) {
            ++foundCount;
            canvas.print(readings[i].rssi);
            canvas.print(" dBm  Ch ");
            canvas.println(readings[i].channel);

            canvas.setCursor(8, y + 24);
            canvas.println(qualityLabel(readings[i].rssi));
        } else {
            canvas.println("NOT FOUND");
            canvas.setCursor(8, y + 24);
            canvas.println("Move closer to router");
        }

        canvas.drawFastHLine(6, y + 37, 228, 1);
    }

    canvas.setCursor(8, 280);
    canvas.print("Networks found: ");
    canvas.print(foundCount);
    canvas.print("/");
    canvas.println(NETWORK_COUNT);

    canvas.setCursor(8, 294);
    canvas.println("Refresh: 5 seconds");

    canvas.setCursor(8, 307);
    canvas.println(
        timeSynchronized
            ? "Clock: IST synchronized"
            : "Clock: not synchronized"
    );

    // Draw separators and text after clearing the old frame.
    drawCanvasText();

    // Signal bars are overlaid after the text.
    for (size_t i = 0; i < NETWORK_COUNT; ++i) {
        if (readings[i].found) {
            drawSignalBars(
                193,
                54 + static_cast<int>(i) * 44 + 1,
                readings[i].rssi
            );
        }
    }
}

// ============================================================
// Time synchronization
// ============================================================

bool getTimestamp(char* buffer, size_t bufferSize) {
    struct tm timeinfo;

    if (!timeSynchronized ||
        !getLocalTime(&timeinfo, 100) ||
        timeinfo.tm_year < (2024 - 1900)) {
        snprintf(buffer, bufferSize, "TIME_NOT_SYNCED");
        return false;
    }

    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", &timeinfo);
    return true;
}

void synchronizeTime() {
    timeSynchronized = false;

    if (strlen(TIME_WIFI_PASSWORD) == 0 ||
        strcmp(TIME_WIFI_PASSWORD,
               "REPLACE_WITH_YOUR_WIFI_PASSWORD") == 0) {
        Serial.println("# NTP skipped: configure src/secrets.h");
        return;
    }

    Serial.printf("# Connecting to %s for NTP...\n", TIME_WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(TIME_WIFI_SSID, TIME_WIFI_PASSWORD);

    const uint32_t connectStart = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - connectStart < WIFI_CONNECT_WAIT_MS) {
        delay(250);
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("# Wi-Fi connection failed; clock not synchronized");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        delay(200);
        return;
    }

    Serial.println("# Wi-Fi connected; requesting NTP time");

    configTime(
        IST_OFFSET_SECONDS,
        DAYLIGHT_OFFSET_SECONDS,
        "pool.ntp.org",
        "time.google.com"
    );

    const uint32_t ntpStart = millis();

    while (millis() - ntpStart < NTP_SYNC_WAIT_MS) {
        struct tm timeinfo;

        if (getLocalTime(&timeinfo, 250) &&
            timeinfo.tm_year >= (2024 - 1900)) {
            timeSynchronized = true;
            Serial.println("# Clock synchronized to IST (UTC+05:30)");
            break;
        }

        delay(100);
    }

    // Disconnect after NTP. Wi-Fi scanning remains enabled in
    // station mode and does not require joining an access point.
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    delay(200);

    if (timeSynchronized) {
        Serial.println("# Disconnected from access point; scan mode active");
    } else {
        Serial.println("# NTP sync failed; timestamps will be TIME_NOT_SYNCED");
    }
}

// ============================================================
// Wi-Fi scanning and CSV output
// ============================================================

void scanKnownNetworks() {
    const int count = WiFi.scanNetworks(false, true);

    if (count < 0) {
        Serial.printf("#,SCAN_ERROR,%d\n", count);
        WiFi.scanDelete();
        return;
    }

    ++scanNumber;

    for (size_t n = 0; n < NETWORK_COUNT; ++n) {
        readings[n] = {false, -127, 0};

        for (int i = 0; i < count; ++i) {
            if (WiFi.SSID(i) == networks[n].ssid) {
                const int rssi = WiFi.RSSI(i);

                if (!readings[n].found || rssi > readings[n].rssi) {
                    readings[n].found = true;
                    readings[n].rssi = rssi;
                    readings[n].channel = WiFi.channel(i);
                }
            }
        }

        char timestamp[32];
        getTimestamp(timestamp, sizeof(timestamp));

        Serial.printf(
            "%lu,\"%s\",%lu,\"%s\",\"%s\",\"%s\",",
            static_cast<unsigned long>(scanNumber),
            timestamp,
            static_cast<unsigned long>(millis()),
            LOCATION,
            networks[n].ssid,
            networks[n].routerLocation
        );

        if (readings[n].found) {
            Serial.printf(
                "%d,%d,\"%s\",FOUND\n",
                readings[n].rssi,
                readings[n].channel,
                qualityLabel(readings[n].rssi)
            );
        } else {
            Serial.println(",,NOT_FOUND,NOT_FOUND");
        }
    }

    size_t foundCount = 0;

    for (size_t i = 0; i < NETWORK_COUNT; ++i) {
        if (readings[i].found) ++foundCount;
    }

    Serial.printf(
        "#,SCAN_COMPLETE,%lu,%u,%u\n",
        static_cast<unsigned long>(scanNumber),
        static_cast<unsigned>(foundCount),
        static_cast<unsigned>(NETWORK_COUNT)
    );

    WiFi.scanDelete();
    renderDashboard();
}

// ============================================================
// Arduino setup and loop
// ============================================================

void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println();
    Serial.println("# ESP32 Wi-Fi Coverage Mapper");
    Serial.println("# Band: 2.4 GHz");
    Serial.printf("# Measurement location: %s\n", LOCATION);

    pinMode(LCD_CS, OUTPUT);
    digitalWrite(LCD_CS, HIGH);

    pinMode(LCD_DC, OUTPUT);
    digitalWrite(LCD_DC, HIGH);

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    SPI_Init();
    Lcd_Init();

    displayReady = true;

    LCD_Clear_Screen(0, 0, LCD_WIDTH, LCD_HEIGHT, BLACK);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.disconnect();
    delay(100);

    synchronizeTime();

    Serial.println(
        "scan_id,timestamp_ist,uptime_ms,location,ssid,"
        "router_location,rssi_dbm,channel,quality,status"
    );

    renderDashboard();
}

void loop() {
    const uint32_t scanStart = millis();

    scanKnownNetworks();

    const uint32_t elapsed = millis() - scanStart;

    if (elapsed < SCAN_INTERVAL_MS) {
        delay(SCAN_INTERVAL_MS - elapsed);
    }
}

