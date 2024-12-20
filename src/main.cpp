#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

#include <Arduino.h>
#include "epd_driver.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "image.h"

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
            
            // Clear framebuffer
            memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
            initialized = true;
        }
        return true;
    }

    void displayImage() {
        if (!initialized || !framebuffer) {
            Serial.println("Display not initialized!");
            return;
        }

        // Power on and clear display
        epd_poweron();
        epd_clear();

        // Draw test pattern
        draw_test_pattern(framebuffer);

        // Display the framebuffer with grayscale test pattern
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
    
    // Display the image
    display.displayImage();
}

void loop() {
    delay(1000);
}
