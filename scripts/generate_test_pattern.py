def generate_test_pattern(width=960, height=540):
    # Calculate bytes needed (4 bits per pixel, 2 pixels per byte)
    bytes_per_row = width // 2
    total_bytes = bytes_per_row * height
    
    # Generate header
    header = """#pragma once
#include "epd_driver.h"

// Image dimensions match the display
const uint16_t IMAGE_WIDTH = EPD_WIDTH;
const uint16_t IMAGE_HEIGHT = EPD_HEIGHT;

// Test pattern with 16 grayscale levels (960x540 pixels, 4 bits per pixel)
const uint8_t IMAGE_DATA[] PROGMEM = {
"""
    
    # Generate pattern data
    pattern = []
    stripe_width = width // 16  # 16 different grayscale levels
    
    for y in range(height):
        row = []
        for x in range(0, width, 2):
            # Calculate grayscale value based on x position
            pos1 = x // stripe_width
            pos2 = min((x + 1) // stripe_width, 15)
            
            # Convert to 4-bit values (0-15)
            val1 = 15 - pos1
            val2 = 15 - pos2
            
            # Combine into byte
            byte = (val1 << 4) | val2
            row.append(byte)
        pattern.extend(row)
    
    # Format data as hex bytes
    data = ""
    for i in range(0, len(pattern), 16):
        line = pattern[i:i + 16]
        data += "    " + ", ".join(f"0x{b:02X}" for b in line) + ",\n"
    
    # Generate footer
    footer = "};\n"
    
    # Write to file
    with open("src/image.h", "w") as f:
        f.write(header + data + footer)

if __name__ == "__main__":
    generate_test_pattern()
    print("Generated test pattern in src/image.h")
