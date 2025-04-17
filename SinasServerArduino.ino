int pulsesPerRotation = 1;
int minRpm = 450;
int maxRpm = 10000;
int maxPulseWidth = 60000/minRpm;
int minPulseWidth = 60000/maxRpm;

volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;

void onPulse() {
    if(millis()-lastPulseTime < minPulseWidth) return;
    pulseCount++;
    lastPulseTime = millis();
}

void setup() {
    Serial.begin(57600);
    pinMode(5, OUTPUT);
    pinMode(3, INPUT);
    pinMode(13, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(3), onPulse, RISING);
    digitalWrite(13, 1);
}

void loop() {
    if(Serial.available() > 0) {
        digitalWrite(13, 0);
        char dataReceived = Serial.read();

        if(dataReceived == 0x00) {
            unsigned long rpm = 0;

            noInterrupts();
            pulseCount = 0;
            interrupts();
            delay(maxPulseWidth);
            noInterrupts();
            rpm += pulseCount*pulsesPerRotation*(60000/maxPulseWidth);
            interrupts();
            
            Serial.write(0x02);
            Serial.write(lowByte(rpm));
            Serial.write(highByte(rpm));
            Serial.flush();
        }
        else if((dataReceived & 0x80) != 0) {
            uint8_t pwmValue = (dataReceived & 0x7f)*2;
            analogWrite(5, pwmValue);
            Serial.write(0x01);
            Serial.write(dataReceived);
            Serial.flush();
        }
        else {
            Serial.write(0x01);
            Serial.write(dataReceived);
            Serial.flush();
        }
        digitalWrite(13, 1);
    }
}