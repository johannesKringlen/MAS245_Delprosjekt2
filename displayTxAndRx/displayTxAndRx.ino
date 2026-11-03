/* 
 *Programmet er en modifisert versjon av:
 *Teensy CAN-Bus with OLED 128x64 demo
 * www.skpang.co.uk
 */
#include <FlexCAN_T4.h>  //canbus communication
#include <SPI.h> // SPI communication with screen
#include <Adafruit_GFX.h> // text and grapix functionality
#include <Adafruit_SSD1306.h> // for controller for screen

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5

Adafruit_SSD1306 display(
    SCREEN_WIDTH, SCREEN_HEIGHT,
    &SPI, OLED_DC, OLED_RESET, OLED_CS
);

static CAN_message_t msg,rxmsg; //sent message, recived message
IntervalTimer TX_timer;
volatile uint32_t can_msg_tx_count = 0;
uint32_t can_msg_rx_count = 0;

FlexCAN_T4<CAN0> Can0;


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


void setup(){
  
  Can0.begin();
  Can0.setBaudRate(500000);   
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  msg.flags.extended = 0; 
  msg.flags.remote = 0;
  TX_timer.begin(tx_CAN, 1000000);    /* Start interrutp timer */
}

/* From Timer Interrupt */
void tx_CAN(void)
{
  msg.len = 8;
  msg.id = 0x7DF;
  msg.buf[0] = 'H';
  msg.buf[1] = 'e';
  msg.buf[2] = 'i';
  msg.buf[3] = 0;
  msg.buf[4] = 0;
  msg.buf[5] = 0;
  msg.buf[6] = 0;
  msg.buf[7] = 0;

  Can0.write(msg);
  can_msg_tx_count++;
}

void loop()
{
  //recive message:
  while (Can0.read(rxmsg))
  {
    can_msg_rx_count ++;
  }

  
  display.clearDisplay();

  display.drawRoundRect(0, 0, 128, 64, 7, SSD1306_WHITE);

  display.setCursor(13, 8);
  display.print("MAS245 - Gruppe 8");


  // midtpunktet (h,k) = (64, 25). Hvert ytterpunkt er 40 pixler unna midtpunktet. Derfor er x-posisjonen til venstre 24. Buen ønskes å være 5 pixler dyp, så dermed er y-posisjonen til ytterpunktet 20
  drawParabola(64, 25, 24, 20);

  display.drawFastHLine(8, 35, 112, SSD1306_WHITE);

  display.setCursor(8, 39);
  display.print("TX:");
  display.print(can_msg_tx_count);

  display.setCursor(48, 39);
  display.print("RX:");
  display.print(can_msg_rx_count);
  

  display.setCursor(88, 39);
  display.print("ID:");
  display.print(rxmsg.id);


  // display.setCursor(8, 52);
  // display.print("IMU Z: ");
  // display.print(9.81, 2);
  // display.print(" m/s2");

  display.display();

}


