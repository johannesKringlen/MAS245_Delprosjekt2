
#include <MPU9250.h>
#include <Wire.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Metro.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5

//Nytt
Metro startMsgTimer = Metro(5000);
//

int status;
static CAN_message_t msg,rxmsg,startMsg; //sent message, recived message
IntervalTimer TX_timer;
volatile uint32_t can_msg_tx_count = 0;
uint32_t can_msg_rx_count = 0;
bool startMsgReceived = false;
Metro imuTimer  = Metro(1000);
int a_max = 30;
int g_max = 10;



MPU9250 IMU(Wire, 0x68);
FlexCAN_T4<CAN0> Can0;
FlexCAN_T4<CAN1> Can1;

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

void setup()
{
    // Konfigurerer strømforsyningen
    display.begin(SSD1306_SWITCHCAPVCC);
    display.clearDisplay();
    display.display();
    // Initialiserer Can0 med en baudrate på 500 kbits/s
    Can0.begin();
    Can0.setBaudRate(500000);
      // initialiserer Can1
    Can1.begin();
    Can1.setBaudRate(500000);

    Serial.begin(9600);
    status = IMU.begin();

    msg.flags.extended = 0; 
    msg.flags.remote = 0;
}


void loop()
{

    if (!startMsgReceived)
    {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    }

    // IMU read 
    IMU.readSensor();

    // Accel
    float aZ = IMU.getAccelZ_mss();
   
    float aY = IMU.getAccelY_mss();
    
    float aX = IMU.getAccelX_mss();

    // Gyro
    float gX = IMU.getGyroX_rads();

    float gY = IMU.getGyroY_rads();

    float gZ = IMU.getGyroZ_rads();

    //Omgjøring

    int8_t aZq = round((aZ/a_max)*127);
    int8_t aYq = round((aY/a_max)*127);
    int8_t aXq = round((aX/a_max)*127);

    int8_t gXq = round((gX/g_max)*127);
    int8_t gYq = round((gY/g_max)*127);
    int8_t gZq = round((gZ/g_max)*127);


    // Draw
    display.drawRoundRect(0, 0, 128, 64, 7, SSD1306_WHITE);

    display.setCursor(13, 8);
    display.print("MAS245 - Gruppe 8");


    // midtpunktet (h,k) = (64, 25). Hvert ytterpunkt er 40 pixler unna midtpunktet. Derfor er x-posisjonen til venstre 24. Buen ønskes å være 5 pixler dyp, så dermed er y-posisjonen til ytterpunktet 20
    drawParabola(64, 25, 24, 20);

    display.drawFastHLine(8, 35, 112, SSD1306_WHITE);

    display.display();

        //Send startmelding Fra Can1
    if (!startMsgReceived && startMsgTimer.check())
    {
        startMsg.len = 8;
        startMsg.id = 0x21;
        startMsg.buf[0] = 0;
        startMsg.buf[1] = 0;
        startMsg.buf[2] = 0;
        startMsg.buf[3] = 0;
        startMsg.buf[4] = 0;
        startMsg.buf[5] = 0;
        startMsg.buf[6] = 0;
        startMsg.buf[7] = 0;


        if (Can1.write(startMsg))
        
        {
        ++can_msg_tx_count;

        startMsgTimer.reset();
        }
    }

   
    //Se etter start melding på Can0
    if (Can0.read(rxmsg))
    {
        if (rxmsg.id == 0x21)
        {
            startMsgReceived = true;
        }
    }

    //send melding fra Can0
    if (startMsgReceived  && imuTimer.check())
    {
        msg.len = 8;
        msg.id = 0x22;
        msg.buf[0] = aXq;
        msg.buf[1] = aYq;
        msg.buf[2] = aZq;
        msg.buf[3] = gXq;
        msg.buf[4] = gYq;
        msg.buf[5] = gZq;
        msg.buf[6] = 0;
        msg.buf[7] = 0;

        Can0.write(msg);


        imuTimer.reset();
    }


    // Motta melding på Can1
    if (Can1.read(rxmsg) && rxmsg.id == 0x22)
    {
        can_msg_rx_count++;
        // read:
        int8_t accelZq = rxmsg.buf[2];
        int8_t gyroXq = rxmsg.buf[3];

        float accelZq1 = accelZq;
        float gyroXq1 = gyroXq;

        // back to full tall:

        float readAccelZ = (accelZq1/127)*a_max;
        float readGyroX = (gyroXq1/127)*g_max;

        // Compare
        Serial.print("orginal :" );
        Serial.println(aZ);
        Serial.print("oversatt: " );
        Serial.println(readAccelZ);


        display.clearDisplay();
        display.drawRoundRect(0, 0, 128, 64, 7, SSD1306_WHITE);

        display.setCursor(13, 8);
        display.print("MAS245 - Gruppe 8");

        // midtpunktet (h,k) = (64, 25). Hvert ytterpunkt er 40 pixler unna midtpunktet. Derfor er x-posisjonen til venstre 24. Buen ønskes å være 5 pixler dyp, så dermed er y-posisjonen til ytterpunktet 20
        drawParabola(64, 25, 24, 20);

        display.drawFastHLine(8, 35, 112, SSD1306_WHITE);        
        display.setCursor(8, 39);
        display.print("RX:");
        display.print(can_msg_rx_count);


        display.setCursor(48, 39);
        display.print("TX:");
        display.print(can_msg_tx_count);


        display.setCursor(88, 39);
        display.print("ID:");
        display.print(rxmsg.id, HEX);

        display.setCursor(8, 52);
        display.print("IMU Z: ");
        display.print(aZ);
        display.print(" m/s2");
        display.display();
    }

    

}

