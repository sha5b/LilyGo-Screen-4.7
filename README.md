# LilyGo T5 4.7" E-Paper Display Image Viewer

This project implements an image slideshow viewer for the LilyGo T5 4.7" E-Paper Display with ESP32-S3. It automatically cycles through JPEG images stored in the SPIFFS filesystem, displaying them at configurable intervals on the e-paper screen.

## Features

- 🖼️ JPEG image display support
- 🔄 Automatic image rotation (configurable interval)
- 📊 4-bit grayscale rendering
- 💾 SPIFFS filesystem integration
- 🔋 Power-efficient design for e-paper display
- 🎨 Grayscale conversion with proper RGB weighting

## Hardware Requirements

- [LilyGo T5 4.7" E-Paper Display with ESP32-S3](http://www.lilygo.cn/prod_view.aspx?TypeId=50062&Id=1384)
- USB-C cable for programming
- Computer with PlatformIO support

## Software Requirements

- [VSCode](https://code.visualstudio.com/)
- [PlatformIO IDE Extension](https://platformio.org/install/ide?install=vscode)
- [Git](https://git-scm.com/) (for cloning the repository)

### Required Libraries

The project uses several libraries, all managed through PlatformIO:

- [LilyGo-EPD47](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47.git#esp32s3)
- SensorLib (v0.1.9)
- Button2 (v2.3.2)
- esp32-arduino-jpeg
- Wire
- SPI

## Installation

1. Clone this repository:
```bash
git clone https://github.com/yourusername/LilyGo-Screen-4.7.git
cd LilyGo-Screen-4.7
```

2. Open the project in VSCode with PlatformIO:
```bash
code .
```

3. Install dependencies (PlatformIO will handle this automatically when building)

4. Configure your board:
   - Ensure PSRAM is enabled
   - Memory type should be set to QIO OPI
   - Filesystem should be set to SPIFFS

## Usage

1. Prepare your images:
   - Convert images to JPEG format
   - Place them in the `data/images/` directory
   - Image resolution should match the display (960x540 pixels recommended)

2. Upload filesystem:
   - Use PlatformIO: Upload Filesystem Image
   - This will upload your images to the ESP32's SPIFFS

3. Upload the program:
   - Connect your LilyGo T5 via USB
   - Use PlatformIO: Upload

4. Monitor the device:
   - Open Serial Monitor in PlatformIO
   - Set baud rate to 115200
   - View debug information and image loading status

## Project Structure

```
LilyGo-Screen-4.7/
├── src/
│   ├── main.cpp          # Main program logic
│   └── image.h           # Image handling and JPEG decoder
├── data/
│   └── images/           # Store your JPEG images here
├── boards/
│   └── T5-ePaper-S3.json # Board configuration
├── lib/                  # External libraries
├── include/              # Header files
├── platformio.ini        # Project configuration
└── README.md            # This file
```

## Configuration

Key parameters can be modified in `src/main.cpp`:

```cpp
const unsigned long IMAGE_CHANGE_INTERVAL = 60000; // Image rotation interval (ms)
```

## Memory Management

The project uses ESP32's PSRAM for frame buffer allocation, crucial for handling the 4.7" display's resolution. Make sure PSRAM is enabled in your board configuration.

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## External Dependencies

This project relies on the following external repositories:

1. [LilyGo-EPD47](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47.git#esp32s3)
   - E-Paper display driver
   - Branch: esp32s3

## Troubleshooting

1. **Display not initializing:**
   - Verify PSRAM is enabled
   - Check USB connection
   - Ensure proper power supply

2. **Images not loading:**
   - Verify images are in JPEG format
   - Check SPIFFS upload was successful
   - Monitor serial output for specific errors

3. **Compilation errors:**
   - Ensure all dependencies are properly installed
   - Verify PlatformIO environment is correctly set up
   - Check ESP32-S3 toolchain is installed

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- LilyGo team for the E-Paper display hardware and base library
- ESP32 community for various tools and libraries
- Contributors to the JPEG decoder implementation
