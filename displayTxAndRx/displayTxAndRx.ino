/* 
 *Programmet er en modifisert versjon av:
 *Teensy CAN-Bus with OLED 128x64 demo
 * www.skpang.co.uk
 */
#include <FlexCAN_T4.h>  //canbus communication
#include <SPI.h> // SPI communication with screen
#include <Adafruit_GFX.h> // text and grapix functionality
#include <Adafruit_SSD1306.h> // for controller for screen

#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5
Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);

static CAN_message_t msg,rxmsg; //sent message, recived message
IntervalTimer TX_timer;
volatile uint32_t can_msg_tx_count = 0;
uint32_t can_msg_rx_count = 0;

FlexCAN_T4<CAN0> Can0;

void setup(){
  
  Can0.begin();
  Can0.setBaudRate(500000);   
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(0);
  display.setTextColor(WHITE);
  
  msg.flags.extended = 0; 
  msg.flags.remote = 0;
  TX_timer.begin(tx_CAN, 1000000);    /* Start interrutp timer */
}

/* From Timer Interrupt */
void tx_CAN(void)
{
  msg.len = 8;
  msg.id = 0x7DF;
  msg.buf[0] = 0x02;
  msg.buf[1] = 0x01;
  msg.buf[2] = 0x0c;
  msg.buf[3] = 0;
  msg.buf[4] = 0;
  msg.buf[5] = 0;
  msg.buf[6] = 0;
  msg.buf[7] = 0;

  Can0.write(msg);
  can_msg_tx_count++;
}

void loop() {
  //recive message:
  while (Can0.read(rxmsg))
  {
    can_msg_rx_count ++;
  }

    display.clearDisplay();
    display.setCursor(0,0);
    display.print("RX: ");
    display.println(can_msg_rx_count);
    display.print("TX: ");
    display.println(can_msg_tx_count);
    display.display();
}


