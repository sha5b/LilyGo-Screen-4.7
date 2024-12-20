#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

#include <Arduino.h>
#include "epd_driver.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Reusable display component
class EpaperDisplay {
private:
    bool initialized;
    uint8_t *framebuffer;

public:
    EpaperDisplay() : initialized(false), framebuffer(nullptr) {}

    bool init() {
        if (!initialized) {
            epd_init();
            
            framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
            if (!framebuffer) {
                Serial.println("Failed to allocate framebuffer!");
                return false;
            }
            
            // Clear framebuffer to white
            memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
            initialized = true;
        }
        return true;
    }

    void displayText(const char* text) {
        if (!initialized || !framebuffer) {
            Serial.println("Display not initialized!");
            return;
        }

        // Power on and clear display
        epd_poweron();
        epd_clear();

        // Draw a simple test pattern - black rectangle in center
        int centerX = EPD_WIDTH / 2;
        int centerY = EPD_HEIGHT / 2;
        
        // Clear to white first
        memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
        
        // Draw a black rectangle in center
        for (int y = centerY - 50; y < centerY + 50; y++) {
            for (int x = centerX - 100; x < centerX + 100; x++) {
                // Calculate position in framebuffer
                int pos = (y * EPD_WIDTH + x) / 2;
                if (x % 2 == 0) {
                    framebuffer[pos] = (framebuffer[pos] & 0x0F) | (0 << 4);
                } else {
                    framebuffer[pos] = (framebuffer[pos] & 0xF0) | 0;
                }
            }
        }

        // Display the framebuffer
        epd_draw_grayscale_image(epd_full_screen(), framebuffer);
        
        // Power off display
        epd_poweroff();
    }

    ~EpaperDisplay() {
        if (framebuffer) {
            heap_caps_free(framebuffer);
        }
    }
};

// Global display instance
EpaperDisplay display;

void setup() {
    Serial.begin(115200);
    
    if (!display.init()) {
        Serial.println("Failed to initialize display!");
        return;
    }
    
    // Display hello world
    display.displayText("Hello World");
}

void loop() {
    delay(1000);
}
