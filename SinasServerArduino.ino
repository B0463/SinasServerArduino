int pulsesPerRotation = 1;

void setup() {
    Serial.begin(57600);
    pinMode(3, OUTPUT);
    pinMode(4, INPUT);
}

void loop() {
    if(Serial.available() > 0) {
        char dataReceived = Serial.read();
        if(dataReceived == 0x00) {
            unsigned int rpm;

            unsigned long pulseWidth = pulseIn(4, LOW, 500000);

            rpm = 60000000UL / (pulseWidth * pulsesPerRotation);

            Serial.write(0x02);
            Serial.write(lowByte(rpm));
            Serial.write(highByte(rpm));
            Serial.flush();
        }
        if((dataReceived & 0x80) != 0) {
            uint8_t pwmValue = (dataReceived & 0x7f)*2;
            analogWrite(3, pwmValue);
        }
        Serial.write(0x01);
        Serial.write(dataReceived);
        Serial.flush();
    }
}