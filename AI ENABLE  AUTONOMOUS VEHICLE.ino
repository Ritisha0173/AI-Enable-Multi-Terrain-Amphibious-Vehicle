#include <ESP32Servo.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// ---------------- PIN DEFINITIONS ----------------
#define TRIG_PIN 5
#define ECHO_PIN 18

#define SERVO_PIN 14

#define ENA 32
#define IN1 33
#define IN2 25
#define IN3 26
#define IN4 27
#define ENB 23

#define PROX_PIN 34
#define BUZZER_PIN 13

// ---------------- VARIABLES ----------------
Servo myServo;
bool isAutonomous = true;

char command = 'S'; // stores last command

long duration;
int distance;

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  SerialBT.begin("RAISING SPARKS");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(PROX_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(90);

  stopMotors();
}

// ---------------- LOOP ----------------
void loop() {

  // 🔵 Always read Bluetooth FIRST
  if (SerialBT.available()) {
    command = SerialBT.read();

    // 🔁 Mode switching (from app)
    if (command == 'A') {
      isAutonomous = true;
      stopMotors();
    }
    else if (command == 'M') {
      isAutonomous = false;
      stopMotors();
    }
  }

  // 🔔 Metal Detection
  if (digitalRead(PROX_PIN) == HIGH) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // 🤖 Autonomous Mode
  if (isAutonomous) {
    autonomousMode();
  }
  
  // 🎮 Manual Mode (App Control)
  else {
    handleManual(command);
  }
}

// ---------------- MANUAL CONTROL ----------------
void handleManual(char cmd) {
  switch (cmd) {
    case 'F': moveForward(); break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft(); break;
    case 'R': turnRight(); break;
    case 'S': stopMotors(); break;
  }
}

// ---------------- AUTONOMOUS MODE ----------------
void autonomousMode() {
  int frontDist = getDistance();

  if (frontDist > 20) {
    moveForward();
  } else {
    stopMotors();

    myServo.write(150);
    delay(400);
    int leftDist = getDistance();

    myServo.write(30);
    delay(400);
    int rightDist = getDistance();

    myServo.write(90);
    delay(200);

    if (leftDist > rightDist) {
      turnLeft();
      delay(400);
    } else {
      turnRight();
      delay(400);
    }
  }
}

// ---------------- ULTRASONIC ----------------
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

// ---------------- MOTOR FUNCTIONS ----------------
void moveForward() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}