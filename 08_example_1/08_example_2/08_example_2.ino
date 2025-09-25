// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficient to convert duration to distance
#define ALPHA 0.05

unsigned long last_sampling_time = 0;   // unit: msec

float smoothDist = 200.0;   
#define ALPHA 0.2           

float exponentialMA(float newVal) {
  smoothDist = ALPHA * newVal + (1 - ALPHA) * smoothDist;
  return smoothDist;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 

  Serial.begin(57600);
}

void loop() { 
  float distance;

  if (millis() < (last_sampling_time + INTERVAL))
    return;

  float rawDist = USS_measure(PIN_TRIG, PIN_ECHO);
  distance = exponentialMA(rawDist);

  if ((distance == 0.0) || (distance > _DIST_MAX)) {
      distance = _DIST_MAX + 10.0;    
      analogWrite(PIN_LED, 255);  // OFF
  } else if (distance < _DIST_MIN) {
      distance = _DIST_MIN - 10.0;    
      analogWrite(PIN_LED, 255);  // OFF
  } else {
    if (distance <= 200) {
      analogWrite(PIN_LED, (int)(2.55 * (200 - distance)));
    } else {
      analogWrite(PIN_LED, (int)(255 - 2.55 * (300 - distance)));
    }
  }

  // 시리얼 출력
  Serial.print("raw:");       Serial.print(rawDist);
  Serial.print(",filtered:"); Serial.print(distance);
  Serial.println("");

  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
