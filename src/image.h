#pragma once

#include <pgmspace.h>

// Define screen dimensions
#define EPD_WIDTH 960
#define EPD_HEIGHT 540

// Raw image data array - each byte represents two 4-bit grayscale pixels
const uint8_t image_data[] PROGMEM = {
    // Fill with alternating black and white pattern for testing
    0x00, 0x00, 0x00, 0x00, // Black pixels
    0xFF, 0xFF, 0xFF, 0xFF, // White pixels
    // Repeat pattern for entire screen
    // This is just a test pattern - we'll replace with actual image data
};

// Function to draw a test pattern
inline void draw_test_pattern(uint8_t* framebuffer) {
    // Create a simple gradient pattern
    for (int y = 0; y < EPD_HEIGHT; y++) {
        for (int x = 0; x < EPD_WIDTH/2; x++) {
            uint8_t value = (x * 16 / (EPD_WIDTH/2)) & 0x0F;
            framebuffer[y * (EPD_WIDTH/2) + x] = (value << 4) | value;
        }
    }
}
