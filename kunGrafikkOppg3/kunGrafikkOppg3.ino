// grafikk for oppgave3, uten sending og mottak av CAN-meldinger

#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5

Adafruit_SSD1306 display(
    SCREEN_WIDTH, SCREEN_HEIGHT,
    &SPI, OLED_DC, OLED_RESET, OLED_CS
);


void drawParabola(float h, float k, float x1, float y1)
{
    if (x1 == h) return;

    float a = (y1 - k) / ((x1 - h) * (x1 - h));

    float x2 = 2 * h - x1;

    for (int x = (int)x1; x <= (int)x2; x++)
    {
        float y = a * (x - h) * (x - h) + k;
        display.drawPixel(x, (int)y, SSD1306_WHITE);
    }
}


FlexCAN_T4<CAN0> Can0;

void setup() {

// Konfigurerer strømforsyningen
display.begin(SSD1306_SWITCHCAPVCC);
// Initialiserer Can0 med en baudrate på 250 kbits/s
Can0.begin();
Can0.setBaudRate(250000);
}

void loop() {

// Setup
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);


display.drawRoundRect(0, 0, 128, 64, 7, SSD1306_WHITE);

display.setCursor(13, 8);
display.print("MAS245 - Gruppe 8");


// midtpunktet (h,k) = (64, 25). Hvert ytterpunkt er 40 pixler unna midtpunktet. Derfor er x-posisjonen til venstre 24. Buen ønskes å være 5 pixler dyp, så dermed er y-posisjonen til ytterpunktet 20
drawParabola(64, 25, 24, 20);

display.drawFastHLine(8, 35, 112, SSD1306_WHITE);

display.setCursor(8, 39);
display.print("RX:200");


display.setCursor(48, 39);
display.print("TX:200");

display.setCursor(88, 39);
display.print("ID:200");

display.setCursor(8, 52);
display.print("IMU Z: ");
display.print(9.81, 2);
display.print(" m/s2");

display.display();

}