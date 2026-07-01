#include <Servo.h>

const int trigPin = 4;
const int echoPin = 5;
const int servoPin = 8;

Servo radarServo;

float duration, distance;
int angle = 0;
int direction = 1;


void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  radarServo.attach(servoPin);
  Serial.begin(115200);

}

void loop() {
  // move servo
  radarServo.write(angle);
  delay(30);

   // Ping
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.343) / 2;

  // Send both values together
  Serial.print("Angle:");
  Serial.print(angle);
  Serial.print(",Distance:");
  Serial.println(distance);

  // Sweep 0-180 and back
  angle += direction * 2;
  if (angle >= 180) {
  direction = -1;}
  if (angle <= 0) {  
  direction = 1;}
}
