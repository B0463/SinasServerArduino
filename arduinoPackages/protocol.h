#pragma once
#include "includes.h" // include the includes file

struct Package {
    uint8_t length;
    uint8_t* payload;
};

class protocol {
    private:
        uint8_t dataReceived = 0; // variable to store the received data

        void sendPackage(Package pkg) {
            for(int i=0;i<pkg.length;i++) {
                Serial.write(pkg.payload[i]); // send the package payload
            }
            Serial.flush(); // send the package
        }
        void processRequest() {
            dataReceived = Serial.read(); // get the byte
        
            // check the command received

            if(dataReceived == 0x00) {
                getRPM(); // get the RPM
            }
            else if((dataReceived & 0x80) != 0) { // if the command has the MSB 1 (1000 0000), the receive the first 7bits to set the PWM
                setPWM(); // set the PWM
            }
            else { // if the command isn't registred, send the command back
                uint8_t payload[] = {dataReceived};
                Package pkg = {
                    1,
                    payload
                };
                sendPackage(pkg);
            }
        }
        void getRPM() {
            unsigned long rpm = 0;

            noInterrupts(); // disable interrupts to reset the pulse count
            pulseCount = 0;
            interrupts(); // enable interrupts again to start counting pulses
            delay(maxPulseWidth); // wait for the maximum pulse width to count the pulses
            noInterrupts(); // disable interrupts to read the pulse count
            rpm += pulseCount*pulsesPerRotation*(60000/maxPulseWidth); // calculate the RPM
            interrupts(); // enable interrupts again

            uint8_t payload[] = {
                lowByte(rpm),
                highByte(rpm)
            };
            Package pkg = {
                2,
                payload
            };
            sendPackage(pkg);
        }
        void setPWM() {
            uint8_t pwmValue = (dataReceived & 0x7f)*2; // get the first 7 bits and multiply by 2 to get the PWM value
            analogWrite(PWM_PIN, pwmValue); // set the PWM value

            // send some response
            uint8_t payload[] = {dataReceived};
            Package pkg = {
                1,
                payload
            };
            sendPackage(pkg);
        }
    public:
        void loop() {
            if(Serial.available() > 0) { // check for available data
                digitalWrite(LED_BUILTIN, 0); // turn off the buildin led to indicate data is being received
                processRequest(); // process the request
                digitalWrite(LED_BUILTIN, 1); // turn on the buildin led to indicate that the communication is done
            }
        }
};