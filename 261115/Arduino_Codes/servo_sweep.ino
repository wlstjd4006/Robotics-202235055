#include <ESP32Servo.h>

#define SERVO_PIN 14

Servo myServo;

void setup() {
    myServo.attach(SERVO_PIN);
    myServo.write(0);
}

void loop() {
    for(int i=0;i<=180;i=i+3){
      myServo.write(i);
    }
    for(int i=180;i>=0;i=i-3){
      myServo.write(i);
    }
}