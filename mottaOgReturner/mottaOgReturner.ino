#include <FlexCAN_T4.h>

FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can0; 

void setup() {
    Serial.begin(9600);

    can0.begin();
    can0.setBaudRate(500000);
}

void loop() {
    CAN_message_t message;

    if (can0.read(message)) {

        can0.write(message);

        Serial.print("CAN-ID: ");
        Serial.println(message.id, HEX);

        if (message.len >= 2) {
            Serial.print("Byte 0: ");
            Serial.println(message.buf[0]);

            Serial.print("Byte 1: ");
            Serial.println(message.buf[1]);
        }
    }
}