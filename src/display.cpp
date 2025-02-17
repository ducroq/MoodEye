#include "display.h"
#include "math.h"

LGFX::LGFX(void)
{
    { // Bus configuration
        auto cfg = _bus_instance.config();
        cfg.spi_host = VSPI_HOST;
        cfg.spi_mode = 0;
        cfg.freq_write = 80000000;
        cfg.freq_read = 16000000;
        cfg.spi_3wire = true;
        cfg.use_lock = true;
        cfg.dma_channel = 1;
        cfg.pin_sclk = TFT_SCK;
        cfg.pin_mosi = TFT_MOSI;
        cfg.pin_miso = -1;
        cfg.pin_dc = TFT_DC;
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

static LGFX display;
static LGFX_Sprite *sprite = nullptr;

static int mapRSSItoRadius(int rssi)
{
    const int MIN_RADIUS = 20;
    const int MAX_RADIUS = 90;
    return map(constrain(rssi, -100, -40),
               -100, -40,
               MIN_RADIUS, MAX_RADIUS);
}

RGBColor hsv2rgb(float H, float S, float V) {
	float r, g, b;
	
	float h = H / 360;
	float s = S / 100;
	float v = V / 100;
	
	int i = floor(h * 6);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);
	
	switch (i % 6) {
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
	}
	
	RGBColor color;
	color.r = r * 255;
	color.g = g * 255;
	color.b = b * 255;
	
	return color;
}

void displayTask(void *parameter)
{
    printf("Display Task starting...\n");

    display.init();
    display.setRotation(0);
    display.setBrightness(DISPLAY_BRIGHTNESS_DEFAULT);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    display.fillScreen(TFT_BLACK);

    sprite = new LGFX_Sprite(&display);
    if (!sprite)
    {
        printf("Failed to allocate sprite!\n");
        vTaskDelete(NULL);
        return;
    }

    sprite->setColorDepth(8); // The ESP32 doesn't have enough memory to create a full 240x240 sprite buffer (which would require 240 * 240 * 2 = 115,200 bytes for 16-bit color)

    if (!sprite->createSprite(SCREEN_WIDTH, SCREEN_HEIGHT))
    {
        printf("Failed to create sprite at full size, trying half size...\n");
        delete sprite;
        sprite = nullptr;
        vTaskDelete(NULL);
        return;
    }
    printf("Sprite created successfully with dimensions: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);

    printf("Display initialized\n");

    float hue = 0;
    ProximityData lastData = {.rssi = -100, .timestamp = 0};

    while (1)
    {
        ProximityData newData;

        if (xQueueReceive(rssiQueue, &newData, 0) == pdTRUE)
        {
            lastData = newData;
        }

        if (millis() - lastData.timestamp > 5000)
        {
            lastData.rssi = -100;
        }

        float rssiIntensity = map(lastData.rssi, -100, -40, 50, 255) / 255.0;
        int32_t targetRadius = mapRSSItoRadius(lastData.rssi);
        int numCircles = map(targetRadius, 20, 90, 2, 6);

        sprite->fillSprite(TFT_BLACK);

        for (int i = 0; i < numCircles; i++)
        {
            int32_t currentRadius = targetRadius * (1.0 - (float)i / numCircles);
            // Use HSV for smooth color transitions
            // hue: 0-359, saturation: 0-255, value: 0-255
            float saturation = 100;  // Full saturation
            float value = 100 * (rssiIntensity);  // Value controlled by RSSI intensity
            RGBColor color = hsv2rgb((float)hue, saturation, value);

            sprite->fillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, currentRadius, 
                display.color565(color.r, color.b, color.g));
        }

        sprite->pushSprite(0, 0);

        printf("RSSI: %f, Radius: %i, hue %f \n", rssiIntensity, targetRadius, hue);

        hue = (hue < 359) ? hue + 1 : 0;

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}