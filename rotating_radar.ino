#include <Servo.h>

//setup pins
const int trigPin = 6;
const int echoPin = 7;
const int servoPin = 11;
const int potPin = 5;
const int buzzerPin = 3;

const int HYSTERESIS = 3; //cm uncertainty band

// initial settings for components
Servo radarServo;
float duration, distance;
int angle = 0;
int direction = 1;
bool buzzing = false;

void setup() { //what components are inputs and outputs
  pinMode(trigPin, OUTPUT); //receives the signal from echoPin
  pinMode(echoPin, INPUT); //sends the signal
  pinMode(buzzerPin, OUTPUT); //makes the buzzing sound when object is detected
  radarServo.attach(servoPin); 
  Serial.begin(115200); //baud rate for HC-SR04 to send signals (number of times per second)
}

void loop() {
  // move servo
  radarServo.write(angle); //tells the servo which angle to be at
  delay(30); 

  // Ping part of this code
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 20000); // timeout of 20ms -> 3.4m max. range
  distance = (duration * 0.0343) / 2; //distance in cm instead of mm

  int threshold = map(analogRead(potPin), 0, 1023, 5, 200); //potentiometer changes trigger range (5-200cm range)

  if (distance > 0 && distance <= threshold - HYSTERESIS) {
    tone(buzzerPin, 1000);
    buzzing = true;
  } else if (distance == 0 || distance > threshold + HYSTERESIS) {
    noTone(buzzerPin);
    buzzing = false;
  }

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
