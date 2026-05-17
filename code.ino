#include <Servo.h>

// --- PIN DEFINITIONS ---
const int trigPin = 13;
const int echoPin = 12;
const int servoPin = 9;

// --- MOTOR DEFINITIONS ---

// FRONT MOTORS (2 wheels in front, wired together)
// Wired to L293D Pins 2 & 7 -> Arduino Pins 5 & 6
const int frontMotorsPin1 = 5;
const int frontMotorsPin2 = 6;

// BACK MOTOR (1 single wheel in back)
// Wired to L293D Pins 10 & 15 -> Arduino Pins 4 & 3
const int backMotorPin1 = 4;
const int backMotorPin2 = 3;

// --- CONFIGURATION ---
const int WALL_DISTANCE_CM = 11;
const int REVERSE_TIME_MS = 10000;

Servo ballLauncher;
bool courseCompleted = false;

void setup() {
  Serial.begin(9600);

  // Setup Motor Pins
  pinMode(frontMotorsPin1, OUTPUT);
  pinMode(frontMotorsPin2, OUTPUT);
  pinMode(backMotorPin1, OUTPUT);
  pinMode(backMotorPin2, OUTPUT);

  // Setup Sensor & Servo
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  ballLauncher.attach(servoPin);

  // Set arm to START position (0 degrees)
  ballLauncher.write(0);

  // Wait 0.5 second before moving
  delay(500);
}

void loop() {
  if (courseCompleted) {
    stopMotors();
    return;
  }

  long distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // --- OBSTACLE LOGIC ---
  if (distance > 0 && distance <= WALL_DISTANCE_CM) {

    // 1. STOP at the wall
    stopMotors();
    delay(1000);

    // 2. LAUNCH BALL 
    ballLauncher.write(100);  // Throw!
    delay(1000);
    ballLauncher.write(0);  // Reset
    delay(500);

    // 3. RETURN TRIP (Go backwards)
    moveBackward();
    delay(REVERSE_TIME_MS);

    // 4. FINISH
    stopMotors();
    courseCompleted = true;

  } else {
    // Drive STRAIGHT
    moveForward();
  }

  delay(50);
}

// --- MOVEMENT FUNCTIONS ---

void moveForward() {
  // Front 2 wheels pull forward
  digitalWrite(frontMotorsPin1, HIGH);
  digitalWrite(frontMotorsPin2, LOW);

  // Back 1 wheel pushes forward
  digitalWrite(backMotorPin1, LOW);
  digitalWrite(backMotorPin2, HIGH);
}

void moveBackward() {
  // Front 2 wheels push backward
  digitalWrite(frontMotorsPin1, LOW);
  digitalWrite(frontMotorsPin2, HIGH);

  // Back 1 wheel pulls backward
  digitalWrite(backMotorPin1, HIGH);
  digitalWrite(backMotorPin2, LOW);
}

void stopMotors() {
  digitalWrite(frontMotorsPin1, LOW);
  digitalWrite(frontMotorsPin2, LOW);
  digitalWrite(backMotorPin1, LOW);
  digitalWrite(backMotorPin2, LOW);
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Added a 30-millisecond timeout so the robot doesn't freeze!
  long duration = pulseIn(echoPin, HIGH, 30000); 
  
  // If it times out
  if (duration == 0) {
    return 999; 
  }
  
  return duration * 0.034 / 2;
}