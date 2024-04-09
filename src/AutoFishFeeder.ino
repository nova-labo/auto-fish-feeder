#include <Servo.h>

#define BLINK_INTERVAL 1000 // Status LED blink interval
#define SERVO_TIME 10      // SG90 micro servo operation time per degree

const byte LED_STATUS = 12;   // Status LED
const byte SERVO_CTRL = 9;    // SG90 micro servo
const byte PUSH_BUTTON = 2;   // Push button for user input
const byte CLOSE_ANGLE = 110; // Latch close position angle
const byte OPEN_ANGLE = 70;   // Latch open position angle

volatile bool isButtonPushed = false;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

int sg90ServoPos;
Servo sg90Servo;

void setup() {
  // Assign pins to I/O
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);

  // Assign pin to SG90 micro servo
  sg90Servo.attach(SERVO_CTRL);
  sg90Servo.write(CLOSE_ANGLE);
  delay(SERVO_TIME);

  // Assign pin to push button
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), buttonPushed, FALLING);
}

void loop() {
  // Open and close latch when button pushed manually
  if (isButtonPushed) {
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

  // Blink status led
  currentMillis = millis();
  if (currentMillis - previousMillis >= BLINK_INTERVAL) {
    previousMillis = currentMillis;
    digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
  }
}

void buttonPushed() {
  isButtonPushed = true;
}