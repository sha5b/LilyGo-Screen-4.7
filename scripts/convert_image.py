from PIL import Image
import os

def convert_image_to_header(input_path, output_path, max_width=960, max_height=540):
    # Open and convert image to grayscale
    img = Image.open(input_path).convert('L')
    
    # Calculate aspect ratio preserving resize dimensions
    aspect = img.width / img.height
    if aspect > (max_width / max_height):
        new_width = max_width
        new_height = int(max_width / aspect)
    else:
        new_height = max_height
        new_width = int(max_height * aspect)
    
    # Center the image
    x_offset = (max_width - new_width) // 2
    y_offset = (max_height - new_height) // 2
    
    # Create new image with black background
    final_img = Image.new('L', (max_width, max_height), 0)
    
    # Resize image
    img = img.resize((new_width, new_height), Image.Resampling.LANCZOS)
    
    # Paste centered
    final_img.paste(img, (x_offset, y_offset))
    
    # Convert to 16 grayscale levels (4-bit)
    final_img = final_img.point(lambda x: int(x / 16) * 16)
    
    # Generate header
    header = """#pragma once
#include "epd_driver.h"

// Image dimensions match the display
const uint16_t IMAGE_WIDTH = EPD_WIDTH;
const uint16_t IMAGE_HEIGHT = EPD_HEIGHT;

// Image data - 4-bit grayscale (16 levels)
const uint8_t IMAGE_DATA[] PROGMEM = {
"""
    
    # Convert image data to bytes
    data = ""
    for y in range(max_height):
        row = []
        for x in range(0, max_width, 2):
            # Get two 4-bit pixels and combine into one byte
            p1 = final_img.getpixel((x, y)) >> 4
            p2 = final_img.getpixel((min(x + 1, max_width - 1), y)) >> 4
            byte = (p1 << 4) | p2
            row.append(byte)
        
        # Add row data
        data += "    " + ", ".join(f"0x{b:02X}" for b in row) + ",\n"
    
    # Write to file
    with open(output_path, "w") as f:
        f.write(header + data + "};\n")

if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(script_dir)
    
    input_image = os.path.join(project_dir, "data", "images", "ComfyUI_00617_.jpg")
    output_header = os.path.join(project_dir, "src", "image.h")
    
    convert_image_to_header(input_image, output_header)
    print(f"Converted {input_image} to {output_header}")
