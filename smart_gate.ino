#include <Servo.h>

// Pin Definitions
#define IR_ENTRY 2         // Entry IR sensor
#define IR_EXIT 3          // Exit IR sensor
#define TRIG_PIN 4         // Ultrasonic Trigger
#define ECHO_PIN 5         // Ultrasonic Echo
#define GREEN_LED 6
#define RED_LED 7
#define BUZZER 8
#define D9_SERVO_PIN 9     // Continuous rotating servo
#define D10_SERVO_PIN 10   // Gate servo

Servo radarServo;
Servo gateServo;

int gateOpenPos = 90;     // Open gate position
int gateClosePos = 0;     // Close gate position

bool gateOpen = false;

void setup() {
  Serial.begin(115200);

  pinMode(IR_ENTRY, INPUT);
  pinMode(IR_EXIT, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  radarServo.attach(D9_SERVO_PIN);
  gateServo.attach(D10_SERVO_PIN);

  radarServo.write(0);
  gateServo.write(gateClosePos); // Start with gate closed

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

void loop() {
  sweepRadar();

  long distance = getDistance();
  if (distance <= 5) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
  }

  // Entry detected → open gate
  if (digitalRead(IR_ENTRY) == LOW && !gateOpen) {
    Serial.println("ENTRY DETECTED → Gate OPEN");
    openGate();
    gateOpen = true;
  }

  // Exit detected → close gate
  if (digitalRead(IR_EXIT) == LOW && gateOpen) {
    Serial.println("EXIT DETECTED → Gate CLOSE");
    closeGate();
    gateOpen = false;
  }

  delay(100);
}

void openGate() {
  gateServo.write(gateOpenPos);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  delay(300); // small delay
}

void closeGate() {
  gateServo.write(gateClosePos);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  delay(300); // small delay
}

// Sweep radar left to right and back
int radarAngle = 0;
bool forward = true;

void sweepRadar() {
  if (forward) {
    radarAngle++;
    if (radarAngle >= 180) forward = false;
  } else {
    radarAngle--;
    if (radarAngle <= 0) forward = true;
  }
  radarServo.write(radarAngle);
  delay(10);
}

// Ultrasonic distance in cm
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}
