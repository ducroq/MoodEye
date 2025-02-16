#pragma once

// Display Pin Configuration (Matching the hardware example)
#define TFT_DC   27    // DC
#define TFT_CS   5     // CS
#define TFT_SCK  14    // SCK
#define TFT_MOSI 15    // MOSI
#define TFT_RST  33    // RES
#define TFT_BL   32    // BLK (Backlight)

// Touch Configuration
#define TOUCH_I2C_ADDR 0x15
#define TOUCH_SDA     21     // Default ESP32 I2C pins
#define TOUCH_SCL     22     // Default ESP32 I2C pins

// Display Configuration
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 240
#define DISPLAY_BRIGHTNESS_DEFAULT 128   // 0-255

// System Configuration
#define SERIAL_BAUD 115200