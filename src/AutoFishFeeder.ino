/*
  Automatic Fish Feeder

  Automatically feeding fish with a specified time interval, or manually with a push of a button.

  This program should be compatible with most Arduinos out on the market.
  However, it has only been tested on an Arduino UNO Rev.3 board. LED_BUILTIN is set to the
  correct LED pin independent of which board is used.

  To make all the magic happen, you will also need the following components:

  + An Arduino UNO (Rev.3) board (preferred, but could be any other Arduino board of your choice)
  + A SG90 Micro Servo (or something similar)
  + A Red LED (preferred, but necessary)
  + A 220 Ohm resistor (LED over current protection)
  + A momentary push button (for manual control if needed)
  + some kind of container (to store fish food)
  + An obsoleted credit card (or any kind of plastic card to be used as latch)

  And of course, you will also need a couple of jump wires to connect all things up! ;-)

  Since the code is pretty much self-explanatory, please feel free to read through and make
  some tweaks as you wish.
  Yes, that's right! One push of the button will reset automatic feeding timer.

  Created 10 April 2024
  By Shunsuke Kawamura

  This code is in the public domain.

  https://github.com/nova-labo/auto-fish-feeder?tab=MIT-1-ov-file
*/

// ----- Header File Includes ----- //
#include <Servo.h>

// #define DEBUG

// ----- Preprocessor Directives ----- //
#ifdef DEBUG
#define LED_STATUS 12       // Use external LED for easy debugging
#define BLINK_INTERVAL 500  // [ms] Status led blink interval
#define FEED_INTERVAL 5000  // [ms] Automatic feed interval for debugging purpose
#else
#define LED_STATUS LED_BUILTIN   // Use interanl LED for saving electrical energy
#define BLINK_INTERVAL 30000     // [ms] Status led blink interval
#define FEED_INTERVAL 259200000  // [ms] Automatic feed interval for normal operation
#endif
#define SERVO_TIME 10  // [ms/degree] servo horn movement wait time per degree

// ----- Global Constants ----- //
const byte SERVO_CTRL = 9;     // SG90 micro servo
const byte PUSH_BUTTON = 2;    // Push button
const byte CLOSE_ANGLE = 115;  // Close position angle
const byte OPEN_ANGLE = 70;    // Open position angle

// ----- Global Variables ----- //
volatile bool isButtonPushed = false;
unsigned long currTime = 0;
unsigned long prevBlinkTime = 0;
unsigned long prevFeedTime = 0;

int sg90ServoPos = CLOSE_ANGLE;
Servo sg90Servo;

/*
* Hardware setup & configurations to get things ready ;-)
*/
void setup() {
  // Configure GPIO pins
  pinMode(LED_STATUS, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);

  digitalWrite(LED_STATUS, LOW);

  // Configure SG90 micro servo
  sg90Servo.attach(SERVO_CTRL);
  sg90Servo.write(CLOSE_ANGLE);
  delay(SERVO_TIME);

  // Configure push button interrupt
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), buttonPushed, FALLING);
}

/*
* Super loop to get it work ;-)
*/
void loop() {
  // Get current time
  currTime = millis();

  if (!isButtonPushed && currTime - prevBlinkTime >= BLINK_INTERVAL) {
    prevBlinkTime = currTime;
    digitalWrite(LED_STATUS, HIGH);  // Blink status led for automatic mode
    delay(SERVO_TIME);
  }

  // Open and close latch when button pushed manually
  // Servo operation takes longer than 25msec,
  // we won't need to worry about signal bouncing caused by the momentary push button.
  if (isButtonPushed || currTime - prevFeedTime >= FEED_INTERVAL) {
    prevFeedTime = currTime;
    digitalWrite(LED_STATUS, HIGH);  // Blink status led for manual mode

    for (sg90ServoPos = CLOSE_ANGLE; sg90ServoPos >= OPEN_ANGLE; sg90ServoPos--) {
      sg90Servo.write(sg90ServoPos);
      delay(SERVO_TIME);
    }

    for (sg90ServoPos = OPEN_ANGLE; sg90ServoPos <= CLOSE_ANGLE; sg90ServoPos++) {
      sg90Servo.write(sg90ServoPos);
      delay(SERVO_TIME);
    }

    isButtonPushed = false;
  }
  
  digitalWrite(LED_STATUS, LOW);
}

/*
* Interrupt Service Routine for the push button
*/
void buttonPushed() {
  isButtonPushed = true;
}