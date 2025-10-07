#include <Servo.h>

Servo leftServo;
Servo rightServo;

void setup() {
  Serial.begin(9600);
  //leftServo.attach(9);
  rightServo.attach(9);
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // lê até '\n'
    int commaIndex = data.indexOf(',');
    if (commaIndex > 0) {
      int leftPos = data.substring(0, commaIndex).toInt();
      int rightPos = data.substring(commaIndex + 1).toInt();
      leftServo.write(leftPos);
      rightServo.write(rightPos);
    }
  }
}
