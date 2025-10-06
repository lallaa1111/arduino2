#include <Servo.h>

#define PIN_TRIG 12
#define PIN_ECHO 13
#define PIN_SERVO 10

#define SND_VEL 346.0
#define SCALE (0.001 * 0.5 * SND_VEL)
#define PULSE_DURATION 10
#define INTERVAL 100

#define DETECT_DISTANCE 300.0
#define ANGLE_DOWN 90
#define ANGLE_UP 0
#define MOVE_TIME 2000

Servo myServo;
unsigned long lastMeasure = 0;
float distance = 999;
bool gateOpen = false;

float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

float easeInOut(float t) {
  return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

float measureDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(PIN_TRIG, LOW);
  long duration = pulseIn(PIN_ECHO, HIGH, 30000);
  if (duration == 0) return distance;
  return duration * SCALE;
}

void moveServoSigmoid(int startAngle, int stopAngle, unsigned long moveTime) {
  unsigned long startTime = millis();
  while (millis() - startTime < moveTime) {
    float progress = (float)(millis() - startTime) / moveTime;
    float s = sigmoid((progress - 0.5) * 10.0);
    int angle = startAngle + (stopAngle - startAngle) * s;
    myServo.write(angle);
    delay(15);
  }
}

void moveServoEase(int startAngle, int stopAngle, unsigned long moveTime) {
  unsigned long startTime = millis();
  while (millis() - startTime < moveTime) {
    float progress = (float)(millis() - startTime) / moveTime;
    float e = easeInOut(progress);
    int angle = startAngle + (stopAngle - startAngle) * e;
    myServo.write(angle);
    delay(15);
  }
}

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  myServo.attach(PIN_SERVO);
  myServo.write(ANGLE_DOWN);
  delay(500);
}

void loop() {
  if (millis() - lastMeasure >= INTERVAL) {
    distance = measureDistance();
    lastMeasure = millis();
  }

  if (distance <= DETECT_DISTANCE && !gateOpen) {
    gateOpen = true;
    moveServoSigmoid(ANGLE_DOWN, ANGLE_UP, MOVE_TIME);
  }

  if (distance > DETECT_DISTANCE && gateOpen) {
    gateOpen = false;
    moveServoEase(ANGLE_UP, ANGLE_DOWN, MOVE_TIME);
  }
}
