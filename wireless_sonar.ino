#include <Servo.h>

const int trigPin = 7;
const int echoPin = 6;
const int servoPin = 9;
const int potPin = 5;
const int buzzerPin = 4;

const int HYSTERESIS = 3;

Servo radarServo;
float duration, distance;
int angle = 0;
int direction = 1;
bool buzzing = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  radarServo.attach(servoPin);
  Serial.begin(9600);   // CHANGED: was 115200 — must match the HC-05's UART setting
}

void loop() {
  radarServo.write(angle);
  delay(30);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 20000);
  distance = (duration * 0.0343) / 2;

  int threshold = map(analogRead(potPin), 0, 1023, 5, 200);

  if (distance > 0 && distance <= threshold - HYSTERESIS) {
    tone(buzzerPin, 2000);
    buzzing = true;
  } else if (distance == 0 || distance > threshold + HYSTERESIS) {
    noTone(buzzerPin);
    buzzing = false;
  }

  Serial.print("Angle:");
  Serial.print(angle);
  Serial.print(",Distance:");
  Serial.println(distance);

  angle += direction * 2;
  if (angle >= 180) { direction = -1; }
  if (angle <= 0)   { direction = 1; }
}
