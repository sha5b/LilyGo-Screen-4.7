#include <Arduino.h>
#include "epd_driver.h"

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting EPD example");

    // Initialize EPD
    epd_init();
    Serial.println("EPD initialized");

    // Power on EPD and clear screen
    epd_poweron();
    epd_clear();
    Serial.println("Screen cleared");

    // Power off EPD
    epd_poweroff();
    Serial.println("EPD powered off");
}

void loop()
{
    delay(1000);
}
