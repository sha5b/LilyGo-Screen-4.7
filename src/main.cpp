#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

/* Simple firmware for a ESP32 displaying a static image on an EPaper Screen.
 *
 * Write an image into a header file using a 3...2...1...0 format per pixel,
 * for 4 bits color (16 colors - well, greys.) MSB first. At 80 MHz, screen
 * clears execute in 1.075 seconds and images are drawn in 1.531 seconds.
 */
#include <Arduino.h>
#include "epd_driver.h"
#include "utilities.h"

void setup()
{
    Serial.begin(115200);
    epd_init();
}

void loop()
{
    // Just clear the display once and power off
    epd_poweron();
    epd_clear();
    epd_poweroff();
    
    // Do nothing after clearing
    while(1) {
        delay(1000);
    }
}
