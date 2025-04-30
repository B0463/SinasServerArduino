#pragma once

#define LED_BUILTIN 13 // define the built-in LED pin
#define PWM_PIN 5 // define the PWM pin
#define PULSE_PIN 3 // define the pulse pin

// set the initial values
int pulsesPerRotation = 1; // 1 pulse per rotation
#define MINRPM 450 // 450 RPM
#define MAXRPM 10000 // 10000 RPM
int maxPulseWidth = 60000/MINRPM; // set the maximum pulse width
int minPulseWidth = 60000/MAXRPM; // set the minimum pulse width

volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;

void onPulse();