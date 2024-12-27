#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

#include <Arduino.h>
#include "epd_driver.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "image.h"

// Keep track of current image and timing
uint8_t currentImageIndex = 0;
unsigned long lastImageChange = 0;
const unsigned long IMAGE_CHANGE_INTERVAL = 60000; // 1 minute in milliseconds

// Display component
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

    void clearScreen() {
        if (!initialized) return;
        epd_poweron();
        epd_clear();
        epd_poweroff();
    }

    void displayImage() {
        if (!initialized || !framebuffer) {
            Serial.println("Display not initialized!");
            return;
        }

        // Power on and clear display
        epd_poweron();
        epd_clear();

        // Copy current image data to framebuffer
        memcpy(framebuffer, (const uint8_t*)pgm_read_dword(&IMAGE_DATA_ARRAY[currentImageIndex]), EPD_WIDTH * EPD_HEIGHT / 2);

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
    delay(1000); // Give serial time to initialize
    
    Serial.println("\n\nStarting E-Paper Display Program");
    
    // Initialize display first
    if (!display.init()) {
        Serial.println("Failed to initialize display!");
        return;
    }

    // Clear screen on startup
    Serial.println("Clearing screen...");
    display.clearScreen();
    
    // Display the image
    Serial.println("\nDisplaying image...");
    display.displayImage();
}

void loop() {
    // Check if it's time to change the image
    if (millis() - lastImageChange >= IMAGE_CHANGE_INTERVAL) {
        // Update current image index
        currentImageIndex = (currentImageIndex + 1) % NUM_IMAGES;
        
        Serial.print("\nChanging to image ");
        Serial.print(currentImageIndex);
        Serial.println("...");
        
        // Display new image
        display.displayImage();
        
        // Update last change time
        lastImageChange = millis();
    }
    
    delay(1000); // Small delay to prevent busy waiting
}
