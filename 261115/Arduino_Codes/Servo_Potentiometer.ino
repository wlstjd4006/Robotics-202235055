#include <ESP32Servo.h>

#define SERVO_PIN 14
#define POT_PIN 39

Servo myServo;

void setup() {
    myServo.attach(SERVO_PIN);
    myServo.write(0);
    Serial.begin(115200);
}

void loop() {
    int value = analogRead(POT_PIN);

    int value2 = map (value, 0, 4095, 0, 180);

    myServo.write(value2);
}