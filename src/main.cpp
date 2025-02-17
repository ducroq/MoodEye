#include <Wire.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <LovyanGFX.hpp>
#include "config.h"

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    LGFX(void)
    {
        { // Bus configuration
            auto cfg = _bus_instance.config();
            cfg.spi_host = VSPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 80000000; // Write frequency 80MHz
            cfg.freq_read = 16000000;  // Read frequency 16MHz
            cfg.spi_3wire = true;
            cfg.use_lock = true;
            cfg.dma_channel = 1;
            cfg.pin_sclk = TFT_SCK;  // SCK from config
            cfg.pin_mosi = TFT_MOSI; // MOSI from config
            cfg.pin_miso = -1;       // MISO not used
            cfg.pin_dc = TFT_DC;     // DC from config
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
            cfg.rgb_order = false; // The GC9A01 sometimes needs its RGB color order adjusted because different manufacturers wire the display differently.
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};

LGFX display;
LGFX_Sprite *sprite = nullptr; // sprite for double buffering

uint16_t rainbowColor(uint8_t hue) {
    uint8_t r, g, b;
    if(hue < 85) {
        r = hue * 3;
        g = 255 - hue * 3;
        b = 0;
    } else if(hue < 170) {
        hue -= 85;
        r = 255 - hue * 3;
        g = 0;
        b = hue * 3;
    } else {
        hue -= 170;
        r = 0;
        g = hue * 3;
        b = 255 - hue * 3;
    }
    return display.color565(r, g, b);
}

void setup()
{
    Serial2.begin(SERIAL_BAUD);

    // Initialize display
    if (!display.init())
    {
        printf("Display initialization failed!\n");
        return;
    }
    printf("Display initialized\n");
    display.setRotation(0);
    display.setBrightness(DISPLAY_BRIGHTNESS_DEFAULT);

    // Setup backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // Clear screen
    display.fillScreen(TFT_BLACK);

    // Create sprite
    sprite = new LGFX_Sprite(&display);
    if (!sprite)
    {
        printf("Failed to allocate sprite!\n");
        return;
    }

    sprite->setColorDepth(8); // The ESP32 doesn't have enough memory to create a full 240x240 sprite buffer (which would require 240 * 240 * 2 = 115,200 bytes for 16-bit color)

    if (!sprite->createSprite(SCREEN_WIDTH, SCREEN_HEIGHT))
    {
        printf("Failed to create sprite at full size, trying half size...\n");
        delete sprite;
        sprite = nullptr;
        return;
    }

    printf("Sprite created successfully with dimensions: %dx%d\n",
           sprite->width(), sprite->height());

    // Draw something to the display directly first to ensure it's working
    display.fillCircle(120, 120, 30, display.color565(255, 0, 0));
    delay(1000);

    printf("Setup complete\n");
}

void loop()
{
    static uint32_t hue = 0;
    static const int32_t center_x = SCREEN_WIDTH / 2;
    static const int32_t center_y = SCREEN_HEIGHT / 2;
    static const int32_t max_radius = SCREEN_WIDTH / 2 - 5;

    sprite->fillSprite(TFT_BLACK);

    // Draw multiple concentric circles with changing colors
    for (int32_t i = max_radius; i > 0; i -= 15)
    {
        uint8_t hue_val = (hue + i) & 0xFF;
        uint16_t color = rainbowColor(hue_val);
        sprite->fillCircle(center_x, center_y, i, color);
    }

    hue = (hue + 1) & 0xFF; // Cycle hue

    sprite->pushSprite(0, 0); // Push sprite to display in one operation

    // delay(20); // 50 FPS
}