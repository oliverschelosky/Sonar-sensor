#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial bt(10, 11);   // HC-05: TXD->10, RXD->11 via divider

Servo sweepServo;
const int trigPin = 7;       // match these to YOUR pins
const int echoPin = 6;
const int servoPin = 9;

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  sweepServo.attach(servoPin);
}

int readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);  // timeout = no echo
  return duration * 0.034 / 2;                    // cm
}

void sendReading(int angle, int distance) {
  Serial.print(angle); Serial.print(","); Serial.println(distance);
  bt.print(angle);     bt.print(",");    bt.println(distance);
}

void loop() {
  for (int angle = 15; angle <= 165; angle++) {
    sweepServo.write(angle);
    delay(30);
    sendReading(angle, readDistance());
  }
  for (int angle = 165; angle >= 15; angle--) {
    sweepServo.write(angle);
    delay(30);
    sendReading(angle, readDistance());
  }
}
