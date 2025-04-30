#include "arduinoPackages/includes.h"

void setup() {
    Serial.begin(57600); // setup the serial port with 57600 baud rate
    //setup pins
    pinMode(PWM_PIN, OUTPUT);
    pinMode(PULSE_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT); 
    attachInterrupt(digitalPinToInterrupt(PULSE_PIN), onPulse, RISING); // attach the interrupt
    digitalWrite(LED_BUILTIN, 1);
    serverStream = protocol(); // create an instance of the protocol class
}

void loop() {
    serverStream.loop(); // call the protocol loop
}

void onPulse() { // interrupt handler for fan pulse counting
    if(millis()-lastPulseTime < minPulseWidth) return; // ignore pulses that are too long
    pulseCount++;
    lastPulseTime = millis();
}