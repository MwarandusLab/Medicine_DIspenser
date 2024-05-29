#include <Servo.h>

Servo myServo;    // Create a Servo object to control the servo
int currentPosition = 0;  // Variable to store the current position of the servo

int Tracker_1 = 0;

const int buttonPin1 = 3;
const int Buzzer = 8;

volatile boolean buttonState1 = HIGH;          // current state of the button
volatile boolean lastButtonState1 = HIGH;      // previous state of the button
volatile unsigned long lastDebounceTime1 = 0;  // last time the output pin was toggled
volatile unsigned long debounceDelay1 = 50;    // the debounce time; increase if the output flickers
volatile boolean objectDetected = false;

unsigned long countdown = 10000; // 60 seconds countdown
unsigned long startTime;
bool countdownActive = false;

enum State {
  IDLE,
  MORNING,
  AFTERNOON,
  REFILL,
  BEEP
};

State currentState = IDLE;

void setup() {
  Serial.begin(9600);
  pinMode(Buzzer, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);

  digitalWrite(Buzzer, LOW);
  myServo.attach(9);

  attachInterrupt(digitalPinToInterrupt(buttonPin1), buttonInterrupt1, FALLING);
  myServo.write(currentPosition);
  delay(2000);

  startCountdown();
}

void loop() {
  switch (currentState) {
    case IDLE:
      idle();
      break;
    case MORNING:
      morningtab();
      break;
    case AFTERNOON:
      afternoontab();
      break;
    case REFILL:
      refiltabs();
      break;
    case BEEP:
      beep();
      break;
  }

}

void idle() {
  buttonInterrupt1();
  if (countdownActive && (millis() - startTime >= countdown)) {
    countdownActive = false;
    digitalWrite(Buzzer, HIGH);
  }
  Tracker_1 = 0;
}

void morningtab() {
  buttonInterrupt1();
  moveServoSlowly(80); // Move to position 80 slowly
  Tracker_1 = 1;
  if (countdownActive && (millis() - startTime >= countdown)) {
    countdownActive = false;
    digitalWrite(Buzzer, HIGH);
  }
}

void afternoontab() {
  buttonInterrupt1();
  moveServoSlowly(180); // Move to position 180 slowly
  Tracker_1 = 2;
  if (countdownActive && (millis() - startTime >= countdown)) {
    countdownActive = false;
    digitalWrite(Buzzer, HIGH);
    currentState = REFILL;
  }
}

void refiltabs() {
  buttonInterrupt1();
  moveServoSlowly(0); // Move to position 0 slowly
  Tracker_1 = 3;
  currentState = BEEP;
}
void beep() {
  buttonInterrupt1();
  digitalWrite(Buzzer, HIGH);
  delay(1000);
  digitalWrite(Buzzer, LOW);
  delay(1000);
  Tracker_1 = 4;
}

void run(int angle) {
  int increment = (angle > currentPosition) ? 1 : -1; // Determine the direction
  for (int pos = currentPosition; pos != angle; pos += increment) {
    myServo.write(pos);
    delay(15); // Adjust delay for desired speed
  }
  currentPosition = angle; // Update current position
}

void buttonInterrupt1() {
  if (millis() - lastDebounceTime1 > debounceDelay1) {
    buttonState1 = digitalRead(buttonPin1);
    //Serial.println("Button 1 is Pressed");
    if (buttonState1 == LOW && lastButtonState1 == HIGH && Tracker_1 == 0 && currentState == IDLE) {
      digitalWrite(Buzzer, LOW);
      Serial.println("Morning Dose");
      startCountdown();
      currentState = MORNING;
    } else if (buttonState1 == LOW && lastButtonState1 == HIGH && Tracker_1 == 1 && currentState == MORNING) {
      digitalWrite(Buzzer, LOW);
      Serial.println("Afternoon Dose");
      startCountdown();
      currentState = AFTERNOON;
    } else if (buttonState1 == LOW && lastButtonState1 == HIGH && Tracker_1 == 3 && currentState == REFILL) {
      digitalWrite(Buzzer, LOW);
      currentState = BEEP;
    } else if (buttonState1 == LOW && lastButtonState1 == HIGH && Tracker_1 == 4 && currentState == BEEP) {
      digitalWrite(Buzzer, LOW);
      startCountdown();
      currentState = IDLE;
    }
    lastButtonState1 = buttonState1;
    lastDebounceTime1 = millis();
  }
}
void startCountdown() {
  countdownActive = true;
  startTime = millis();
}

void moveServoSlowly(int targetPosition) {
  int direction = (targetPosition > currentPosition) ? 1 : -1;
  for (int pos = currentPosition; pos != targetPosition; pos += direction) {
    myServo.write(pos);
    delay(50); // Adjust delay for desired speed
  }
  currentPosition = targetPosition;
}
