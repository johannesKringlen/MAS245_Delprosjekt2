/* 
 *Programmet er en modifisert versjon av:
 *Teensy CAN-Bus with OLED 128x64 demo
 *  
 * www.skpang.co.uk
 * 
 * V1.0 July 2017
 *  
 * For use with Teensy 3.6 Dual CAN-Bus Breakout board:
 * http://skpang.co.uk/catalog/teensy-36-dual-canbus-breakout-board-include-teensy-36-p-1532.html
 * requires OLED display
 * http://skpang.co.uk/catalog/oled-128x64-display-for-teensy-breakout-board-p-1508.html
 * 
 *  
 */
#include <FlexCAN.h>  //canbus communication
#include <SPI.h> // SPI communication with screen
#include <Adafruit_GFX.h> // text and grapix functionality
#include <Adafruit_SSD1306.h> // for controller for screen

#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5
Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);

static CAN_message_t msg,rxmsg; //sent message, recived message
volatile uint32_t count = 0;
IntervalTimer TX_timer;
String CANStr(""); 
volatile uint32_t can_msg_tx_count = 0;

const unsigned long timeoutMs = 2000;

void setup(){
  
  Can0.begin(500000); //set speed here. 
  Can1.begin(500000); //set speed here.  
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // Clear the buffer.
  display.clearDisplay();
  
  delay(1000);
  Serial.println(F("CAN Bus Tx test"));

  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(0,15);
  display.println(" Teensy 3.6 CAN demo");
  display.println(" ");
  display.println(" skpang.co.uk");
  display.println(" ");
  display.println(" 07/17");
  display.display();

  delay(1000);
  
  display.clearDisplay();
  
  msg.flags.extended = 0; 
  msg.flags.remote = 0;
//  msg.timeout = 500;
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
  /*
  msg.buf[4] = can_msg_tx_count >> 24;
  msg.buf[5] = can_msg_tx_count >> 16;
  msg.buf[6] = can_msg_tx_count >> 8;
  msg.buf[7] = can_msg_tx_count;
  */
  Can0.write(msg);
  Can1.write(msg);
  can_msg_tx_count++;
}

void loop() {

  // while (Can0.read(rxmsg)) {
  //   String CANStr = "";

  //   for (int i = 0; i < rxmsg.len; i++) {
  //     CANStr += String(rxmsg.buf[i], HEX);
  //     CANStr += " ";
  //   }

  while (Can0.write(msg))
  {
  String CANStr = "";

    for (int i = 0; i < msg.len; i++) {
      CANStr += String(msg.buf[i], HEX);
      CANStr += " ";
    }

    // Skriv til Serial Monitor
    Serial.print(rxmsg.id, HEX);
    Serial.print(' ');
    Serial.print(rxmsg.len);
    Serial.print(' ');
    Serial.println(CANStr);

    // Vis mottatt melding
    display.clearDisplay();
    display.setCursor(0, 15);
    display.println(msg.id, HEX);
    display.println(msg.len);
    display.println(CANStr);
    display.println(can_msg_tx_count);
    display.display();
  }
}


