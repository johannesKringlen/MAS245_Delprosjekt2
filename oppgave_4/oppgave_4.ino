#include <MPU9250.h>
#include <Wire.h>
#include <Adafruit_GFX.h>      // Core graphics library
#include <Adafruit_SSD1306.h>  // Hardware-specific library for SSD1306
#include <Fonts/FreeMono9pt7b.h>

MPU9250 IMU(Wire, 0x68);
int status;

void setup() {

  Serial.begin(9600);
  status = IMU.begin();


}

void loop() {
  // put your main code here, to run repeatedly:
  
  IMU.readSensor();
  float aZ = IMU.getAccelZ_mss();
  Serial.println(aZ);
  float aY = IMU.getAccelY_mss();
  Serial.println(aY);
  float aX = IMU.getAccelX_mss();
  Serial.println(aX);
}


