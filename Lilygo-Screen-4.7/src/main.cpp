#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  // Draw some test graphics
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 10);
  tft.println("LilyGo T-Display-S3");
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_RED);
}

void loop() {
  // Add animation to show the display is working
  static uint8_t color = 0;
  tft.fillCircle(tft.width()/2, tft.height()/2, 50, color++ * 8);
  delay(100);
}
