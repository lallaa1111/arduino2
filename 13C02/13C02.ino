#include <Servo.h>
#define PIN_SERVO 10
#define _DUTY_MIN 550
#define _DUTY_MAX 2350
#define INTERVAL 20
#define _SERVO_SPEED 0.3

Servo myservo;
unsigned long last_time = 0;
double pos_deg = 0.0;
int duty_per_deg = (_DUTY_MAX - _DUTY_MIN) / 180.0;

void setup() {
  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_MIN);
  delay(300);
  last_time = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - last_time >= INTERVAL) {
    last_time = now;
    pos_deg += _SERVO_SPEED * (INTERVAL / 1000.0);
    if (pos_deg >= 90.0) pos_deg = 90.0;
    int duty = _DUTY_MIN + pos_deg * duty_per_deg;
    myservo.writeMicroseconds(duty);
    if (pos_deg >= 90.0) while (1);
  }
}
