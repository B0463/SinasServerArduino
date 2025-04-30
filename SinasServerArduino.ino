// set the initial values
int pulsesPerRotation = 1; // 1 pulse per rotation
int minRpm = 450; // 450 RPM
int maxRpm = 10000; // 10000 RPM
int maxPulseWidth = 60000/minRpm; // set the maximum pulse width
int minPulseWidth = 60000/maxRpm; // set the minimum pulse width

volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;

void onPulse() { // interrupt handler for fan pulse counting
    if(millis()-lastPulseTime < minPulseWidth) return; // ignore pulses that are too long
    pulseCount++;
    lastPulseTime = millis();
}

void setup() {
    Serial.begin(57600); // setup the serial port with 57600 baud rate
    pinMode(5, OUTPUT); // setup the PWM pin
    pinMode(3, INPUT); // setup the interrupt pin
    pinMode(13, OUTPUT); // setup the buildin LED pin
    attachInterrupt(digitalPinToInterrupt(3), onPulse, RISING); // attach the interrupt to pin 3
    digitalWrite(13, 1);
}

void loop() {
    if(Serial.available() > 0) { // check for available data
        digitalWrite(13, 0); // turn off the buildin led to indicate data is being received
        char dataReceived = Serial.read(); // get the byte

        // check the command received
        if(dataReceived == 0x00) { // get the RPM
            unsigned long rpm = 0;

            noInterrupts(); // disable interrupts to reset the pulse count
            pulseCount = 0;
            interrupts(); // enable interrupts again to start counting pulses
            delay(maxPulseWidth); // wait for the maximum pulse width to count the pulses
            noInterrupts(); // disable interrupts to read the pulse count
            rpm += pulseCount*pulsesPerRotation*(60000/maxPulseWidth); // calculate the RPM
            interrupts(); // enable interrupts again
            
            // send the RPM response
            Serial.write(0x02); // len
            Serial.write(lowByte(rpm));
            Serial.write(highByte(rpm));
            Serial.flush(); // send the data
        }
        else if((dataReceived & 0x80) != 0) { // if the command has the MSB 1 (1000 0000), the receive the first 7bits to set the PWM
            uint8_t pwmValue = (dataReceived & 0x7f)*2; // get the first 7 bits and multiply by 2 to get the PWM value
            analogWrite(5, pwmValue); // set the PWM value

            // send some response
            Serial.write(0x01);
            Serial.write(dataReceived);
            Serial.flush();
        }
        else { // if the command isn't registred, send the command back
            Serial.write(0x01);
            Serial.write(dataReceived);
            Serial.flush();
        }
        digitalWrite(13, 1); // turn on the buildin led to indicate that the communication is done
    }
}