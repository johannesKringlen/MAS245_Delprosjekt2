#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_DC     6
#define OLED_CS    10
#define OLED_RESET  5

Adafruit_SSD1306 display(
  128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS
);

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    while (true) {}
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hei fra Teensy!");
  display.display();
}

void loop() {
}
