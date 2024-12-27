from PIL import Image
import os

def convert_image(img, max_width=960, max_height=540):
    # Rotate portrait images to landscape
    if img.height > img.width:
        img = img.rotate(90, expand=True)
    
    # Calculate scaling to fill screen completely
    width_ratio = max_width / img.width
    height_ratio = max_height / img.height
    
    # Use larger ratio to ensure image fills screen
    scale = max(width_ratio, height_ratio)
    
    # Calculate new dimensions
    new_width = int(img.width * scale)
    new_height = int(img.height * scale)
    
    # Resize image to fill screen
    img = img.resize((new_width, new_height), Image.Resampling.LANCZOS)
    
    # Calculate crop coordinates to center the image
    left = (new_width - max_width) // 2
    top = (new_height - max_height) // 2
    right = left + max_width
    bottom = top + max_height
    
    # Crop image to screen dimensions
    img = img.crop((left, top, right, bottom))
    
    # Convert to 16 grayscale levels (4-bit) with proper range mapping
    # Input range is 0-255, output range should be 0-15 scaled back to 0-255
    return img.point(lambda x: int((x / 255.0) * 15) * 17)

def generate_header_content(images_data):
    header = """#pragma once
#include "epd_driver.h"

// Image dimensions match the display
const uint16_t IMAGE_WIDTH = EPD_WIDTH;
const uint16_t IMAGE_HEIGHT = EPD_HEIGHT;

// Number of available images
const uint8_t NUM_IMAGES = %d;

""" % len(images_data)
    
    # Add each image's data
    for idx, (name, img) in enumerate(images_data):
        header += f"// Image {idx}: {name}\n"
        header += f"const uint8_t IMAGE_{idx}_DATA[] PROGMEM = {{\n"
        
        # Convert image data to bytes
        for y in range(img.height):
            row = []
            for x in range(0, img.width, 2):
                # Get two 4-bit pixels and combine into one byte
                p1 = img.getpixel((x, y)) >> 4
                p2 = img.getpixel((min(x + 1, img.width - 1), y)) >> 4
                byte = (p1 << 4) | p2
                row.append(byte)
            
            # Add row data
            header += "    " + ", ".join(f"0x{b:02X}" for b in row) + ",\n"
        
        header += "};\n\n"
    
    # Add array of image data pointers
    header += "// Array of pointers to image data\n"
    header += "const uint8_t* const IMAGE_DATA_ARRAY[] PROGMEM = {\n"
    for idx in range(len(images_data)):
        header += f"    IMAGE_{idx}_DATA,\n"
    header += "};\n"
    
    return header

def convert_all_images(input_dir, output_path):
    images_data = []
    
    # Process each image in the directory
    for filename in os.listdir(input_dir):
        if filename.lower().endswith(('.jpg', '.jpeg', '.png')):
            print(f"Converting {filename}...")
            
            # Open and convert image to grayscale
            img_path = os.path.join(input_dir, filename)
            img = Image.open(img_path).convert('L')
            
            # Convert and process image
            processed_img = convert_image(img)
            
            # Add to images data list
            images_data.append((filename, processed_img))
    
    # Generate and save header file
    header_content = generate_header_content(images_data)
    with open(output_path, "w") as f:
        f.write(header_content)
    
    print(f"\nConverted {len(images_data)} images to {output_path}")

if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(script_dir)
    
    input_dir = os.path.join(project_dir, "data", "images")
    output_path = os.path.join(project_dir, "src", "image.h")
    
    convert_all_images(input_dir, output_path)
