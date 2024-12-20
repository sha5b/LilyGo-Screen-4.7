#pragma once

#include <Arduino.h>
#include <SPIFFS.h>
#include "epd_driver.h"
#include "esp_heap_caps.h"
#include "esp32/rom/tjpgd.h"

// JPEG decoder callback functions
typedef struct {
    uint8_t* framebuffer;
    int x;
    int y;
    uint16_t width;
    uint16_t height;
} jpeg_decode_ctx_t;

static uint32_t jpegRead(JDEC* decoder, uint8_t* buf, uint32_t len) {
    File* file = (File*)decoder->device;
    return file->read(buf, len);
}

static uint32_t jpegWrite(JDEC* decoder, void* bitmap, JRECT* rect) {
    jpeg_decode_ctx_t* ctx = (jpeg_decode_ctx_t*)decoder->device;
    uint8_t* src = (uint8_t*)bitmap;
    
    int width = rect->right - rect->left + 1;
    int height = rect->bottom - rect->top + 1;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcX = rect->left + x;
            int srcY = rect->top + y;
            
            if (srcX >= ctx->width || srcY >= ctx->height) continue;
            
            // Get RGB values - JPEG decoder outputs 24-bit color (8 bits per component)
            uint8_t r = src[y * width * 3 + x * 3];
            uint8_t g = src[y * width * 3 + x * 3 + 1];
            uint8_t b = src[y * width * 3 + x * 3 + 2];
            
            // Convert to grayscale using standard weights
            uint8_t gray = (r * 77 + g * 151 + b * 28) >> 8;
            
            // Convert to 4-bit grayscale (0-15)
            gray = (gray * 15) / 255;
            
            // Calculate position in framebuffer
            int pos = srcY * (ctx->width / 2) + (srcX / 2);
            
            // Pack two 4-bit values into one byte
            if (srcX % 2 == 0) {
                ctx->framebuffer[pos] = (gray << 4);
            } else {
                ctx->framebuffer[pos] |= gray;
            }
        }
    }
    return 1;
}

class ImageHandler {
private:
    static constexpr size_t BUFFER_SIZE = 1024;
    uint8_t* buffer;
    
    // Convert RGB565 to 4-bit grayscale
    static uint8_t rgb565_to_4bit_gray(uint16_t color) {
        // Extract RGB components
        uint8_t r = (color >> 11) & 0x1F;
        uint8_t g = (color >> 5) & 0x3F;
        uint8_t b = color & 0x1F;
        
        // Convert to grayscale using standard weights
        uint16_t gray = (r * 77 + g * 151 + b * 28) >> 8;
        
        // Convert to 4-bit (0-15)
        return (gray * 15) / 255;
    }

public:
    ImageHandler() {
        buffer = (uint8_t*)heap_caps_malloc(BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    }

    ~ImageHandler() {
        if (buffer) {
            heap_caps_free(buffer);
        }
    }

    bool loadAndConvertImage(const char* filename, uint8_t* framebuffer) {
        String fullPath = String(filename);
        if (!fullPath.startsWith("/")) {
            fullPath = "/" + fullPath;
        }
        
        File file = SPIFFS.open(fullPath, "r");
        if (!file) {
            Serial.println("Failed to open image file");
            return false;
        }

        // Clear framebuffer
        memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

        // Allocate work space for JPEG decoder (increased for larger images)
        size_t workSize = 32768; // 32KB workspace
        uint8_t *work = (uint8_t*)heap_caps_malloc(workSize, MALLOC_CAP_SPIRAM);
        if (!work) {
            Serial.println("Failed to allocate JPEG workspace");
            file.close();
            return false;
        }

        JDEC decoder;
        jpeg_decode_ctx_t ctx;
        ctx.framebuffer = framebuffer;
        ctx.width = EPD_WIDTH;
        ctx.height = EPD_HEIGHT;
        ctx.x = 0;
        ctx.y = 0;

        decoder.device = &file; // Set file as device for jpegRead
        JRESULT res = jd_prepare(&decoder, jpegRead, work, workSize, &file);
        if (res != JDR_OK) {
            Serial.println("Failed to prepare JPEG decoder");
            heap_caps_free(work);
            file.close();
            return false;
        }

        decoder.device = &ctx; // Set context as device for jpegWrite
        res = jd_decomp(&decoder, jpegWrite, 1); // Scale = 1 for full resolution
        if (res != JDR_OK) {
            Serial.println("Failed to decompress JPEG");
            heap_caps_free(work);
            file.close();
            return false;
        }

        heap_caps_free(work);
        file.close();
        return true;
    }

    // Function to draw a test pattern (kept for testing purposes)
    static void drawTestPattern(uint8_t* framebuffer) {
        for (int y = 0; y < EPD_HEIGHT; y++) {
            for (int x = 0; x < EPD_WIDTH/2; x++) {
                uint8_t value = (x * 16 / (EPD_WIDTH/2)) & 0x0F;
                framebuffer[y * (EPD_WIDTH/2) + x] = (value << 4) | value;
            }
        }
    }
};
