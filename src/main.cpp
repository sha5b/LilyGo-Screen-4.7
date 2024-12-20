#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

#include <Arduino.h>
#include "epd_driver.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <vector>

#include "image.h"

// Keep track of current image and timing
int currentImageIndex = 0;
unsigned long lastImageChange = 0;
const unsigned long IMAGE_CHANGE_INTERVAL = 60000; // 1 minute in milliseconds
std::vector<String> images;

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

        if (images.empty()) {
            Serial.println("No images available!");
            return;
        }

        // Power on and clear display
        epd_poweron();
        epd_clear();

        // Create image handler
        static ImageHandler imageHandler;
        
        Serial.print("Attempting to load image: ");
        Serial.println(images[currentImageIndex]);
        
        // Load and convert current image
        if (!imageHandler.loadAndConvertImage(images[currentImageIndex].c_str(), framebuffer)) {
            Serial.println("Failed to load image! Error details:");
            Serial.print("Image path: ");
            Serial.println(images[currentImageIndex]);
            
            // Draw test pattern as fallback
            Serial.println("Drawing test pattern instead");
            ImageHandler::drawTestPattern(framebuffer);
        } else {
            Serial.println("Successfully loaded image");
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

// Get list of all images in directory
void listImages() {
    images.clear();
    File root = SPIFFS.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("Failed to open /images directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String filename = file.name();
            if ((filename.endsWith(".jpg") || filename.endsWith(".jpeg")) && filename.startsWith("/images/")) {
                images.push_back(filename);
                Serial.print("Found image: ");
                Serial.println(filename);
            }
        }
        file = root.openNextFile();
    }
    root.close();
}

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
    
    Serial.println("Mounting SPIFFS...");
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
        return;
    }

    // List all files in SPIFFS for debugging
    Serial.println("\nListing all files in SPIFFS:");
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file) {
        Serial.print("  ");
        Serial.print(file.name());
        Serial.print("  (");
        Serial.print(file.size());
        Serial.println(" bytes)");
        file = root.openNextFile();
    }
    root.close();

    Serial.println("\nScanning for images...");
    listImages();
    
    if (images.empty()) {
        Serial.println("No images found in /images directory!");
        return;
    }

    Serial.println("\nFound the following images:");
    for(const String& img : images) {
        Serial.println(img);
    }
    
    // Display first image
    Serial.println("\nDisplaying first image...");
    display.displayImage();
    lastImageChange = millis();
}

void loop() {
    // Check if it's time to change the image
    if (millis() - lastImageChange >= IMAGE_CHANGE_INTERVAL) {
        Serial.println("\nTime to change image!");
        currentImageIndex = (currentImageIndex + 1) % images.size();
        Serial.print("Switching to image index ");
        Serial.print(currentImageIndex);
        Serial.print(" (");
        Serial.print(images[currentImageIndex]);
        Serial.println(")");
        
        display.displayImage();
        lastImageChange = millis();
    }

    delay(1000); // Small delay to prevent busy waiting
}
