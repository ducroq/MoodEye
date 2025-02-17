#include <Wire.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <LovyanGFX.hpp>
// #include <LGFX_AUTODETECT.hpp>
#include "config.h"


class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    LGFX(void) {
        { // Bus configuration
            auto cfg = _bus_instance.config();
            cfg.spi_host = VSPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;    // Write frequency 40MHz
            cfg.freq_read = 16000000;     // Read frequency 16MHz
            cfg.spi_3wire = true;
            cfg.use_lock = true;
            cfg.dma_channel = 1;
            cfg.pin_sclk = TFT_SCK;       // SCK from config
            cfg.pin_mosi = TFT_MOSI;      // MOSI from config
            cfg.pin_miso = -1;            // MISO not used
            cfg.pin_dc = TFT_DC;          // DC from config
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        { // Display configuration
            auto cfg = _panel_instance.config();
            cfg.pin_cs = TFT_CS;
            cfg.pin_rst = TFT_RST;
            cfg.pin_busy = -1;
            cfg.memory_width = SCREEN_WIDTH;
            cfg.memory_height = SCREEN_HEIGHT;
            cfg.panel_width = SCREEN_WIDTH;
            cfg.panel_height = SCREEN_HEIGHT;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = true;
            cfg.rgb_order = true;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};

LGFX display;

void setup() {
    Serial2.begin(SERIAL_BAUD);
    
    // Initialize I2C for touch
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    
    // Initialize display
    display.init();
    display.setRotation(0);
    display.setBrightness(DISPLAY_BRIGHTNESS_DEFAULT);
    
    // Setup backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    
    // Clear screen
    display.fillScreen(TFT_BLACK);

    printf("Display initialized successfully\n");
    
    // Serial.println("Display initialized successfully");
}

void loop() {
    static uint32_t hue = 0;
    static int32_t x = 120;
    static int32_t y = 120;
    static int32_t r = 60;

    // Create a smooth circular animation
    display.fillScreen(TFT_BLACK);
    
    // Draw multiple concentric circles with changing colors
    for (int32_t i = r; i > 0; i -= 15) {
        display.fillCircle(x, y, i, 
            display.color565(
                (hue + i) & 0xFF,       // R
                (hue + i + 85) & 0xFF,  // G
                (hue + i + 170) & 0xFF  // B
            )
        );
    }
    
    hue = (hue + 1) & 0xFF;  // Cycle hue
    
    delay(100);  // 50 FPS
}