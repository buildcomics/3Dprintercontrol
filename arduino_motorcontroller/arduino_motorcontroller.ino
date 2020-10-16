/*
    Arduino Motorcontroller for timelapse
    https://www.buildcomics.com
    Wire ULN200x module for 28BYJ-48 5V stepper motor to STEPPERPINX defined outputs, and power using 5v/ground.
    Uses about 300 steps for half a rotation, e.g 1 hour print means (60*60)/300 = one step every 12 seconds.
    Board ==> Arduino Nano
    Processor ==> Atmega328P (Old Boatloader)
*/

#define STEPPERPIN1 2
#define STEPPERPIN2 3
#define STEPPERPIN3 4
#define STEPPERPIN4 5

#define STOPCWPIN 10
#define STOPCCWPIN 11

unsigned long int previousMillis;
const unsigned int interval = 500;

int stepSpeed = 1;
int motorDelay = 10;     //set to normal speed
int stepDelay = 0;     //variable to set stepper speed
bool initEnable = true; //We start with initialization
bool positioning = true; //We start with initialization
bool direction = true; //true = clockwise, false is counterclockwise
bool pause = false; //To pause the arm

void setup() {
  pinMode(STEPPERPIN1, OUTPUT);
  pinMode(STEPPERPIN2, OUTPUT);
  pinMode(STEPPERPIN3, OUTPUT);
  pinMode(STEPPERPIN4, OUTPUT);

  pinMode(STOPCWPIN, INPUT_PULLUP);
  pinMode(STOPCCWPIN, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  //First we get back to the starting point
  if (positioning) {
    Serial.println("Initial Positioning...");
    stepDelay = 5;
    while(digitalRead(STOPCWPIN)) {
      clockwise();
    }
    stepDelay = 0;
    direction = false;
    positioning = false;
  }
  while (initEnable) {
    Serial.println("At base position, enter step delay?");
    String delayString = Serial.readStringUntil('\n');
    stepDelay = delayString.toInt();
    if (stepDelay > 0) {
      initEnable = false; //Initialization finished
      Serial.print("starting with delay:");
      Serial.println(stepDelay);
      break;
    }
  }

  if (!digitalRead(STOPCWPIN) && direction) {
    Serial.println("CW!");
    direction = false; //reverse direction
  }
  if (!digitalRead(STOPCCWPIN) && !direction) {
    Serial.println("CCW!");
    direction = true; //reverse direction
  }

  if (!pause && !initEnable && !positioning) {
    unsigned long currentMillis = millis();
    unsigned long timeDif = currentMillis - previousMillis;
    if (timeDif >= stepDelay) {
      previousMillis = currentMillis; //reset counter
      Serial.println("Step!");
      if (direction) {
        clockwise();
      }
      else {
        counterclockwise();
      }
    }
  }

  if (Serial.available() > 0) {
    // read the incoming byte:
    char rec = Serial.read();
    if (rec == 'X' || rec == 'R' | rec == 'P') {
      digitalWrite(STEPPERPIN1, LOW);
      digitalWrite(STEPPERPIN2, LOW);
      digitalWrite(STEPPERPIN3, LOW);
      digitalWrite(STEPPERPIN4, LOW);
      if (rec == 'R') { //Reset completely
        Serial.println("Reset");
        positioning = true;
        initEnable = true;
      }
      else if (rec == 'X') { //Enter new speed
        Serial.println("SpeedChange");
        initEnable = true;
      }
    }
    if (rec == 'P' && !pause) { //Pause arm
      Serial.println("Pause");
      pause = true;
    }
    else if(rec == 'C' && pause) { //Continue arm
      Serial.println("Continue");
      pause = false;
    }
  }

}

//////////////////////////////////////////////////////////////////////////////
//set pins to ULN2003 high in sequence from 1 to 4
//delay "motorDelay" between each pin setting (to determine speed)
void counterclockwise () {
  // 1
  digitalWrite(STEPPERPIN1, HIGH);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN4, LOW);
  delay(motorDelay);
  // 2
  digitalWrite(STEPPERPIN1, HIGH);
  digitalWrite(STEPPERPIN2, HIGH);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN4, LOW);
  delay (motorDelay);
  // 3
  digitalWrite(STEPPERPIN1, LOW);
  digitalWrite(STEPPERPIN2, HIGH);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN4, LOW);
  delay(motorDelay);
  // 4
  digitalWrite(STEPPERPIN1, LOW);
  digitalWrite(STEPPERPIN2, HIGH);
  digitalWrite(STEPPERPIN3, HIGH);
  digitalWrite(STEPPERPIN4, LOW);
  delay(motorDelay);
  // 5
  digitalWrite(STEPPERPIN1, LOW);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN3, HIGH);
  digitalWrite(STEPPERPIN4, LOW);
  delay(motorDelay);
  // 6
  digitalWrite(STEPPERPIN1, LOW);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN3, HIGH);
  digitalWrite(STEPPERPIN4, HIGH);
  delay (motorDelay);
  // 7
  digitalWrite(STEPPERPIN1, LOW);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN4, HIGH);
  delay(motorDelay);
  // 8
  digitalWrite(STEPPERPIN1, HIGH);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN4, HIGH);
  delay(motorDelay);
}

//////////////////////////////////////////////////////////////////////////////
//set pins to ULN2003 high in sequence from 4 to 1
//delay "motorDelay" between each pin setting (to determine speed)
void clockwise() {
  // 1
  digitalWrite(STEPPERPIN4, HIGH);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN1, LOW);
  delay(motorDelay);
  // 2
  digitalWrite(STEPPERPIN4, HIGH);
  digitalWrite(STEPPERPIN3, HIGH);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN1, LOW);
  delay (motorDelay);
  // 3
  digitalWrite(STEPPERPIN4, LOW);
  digitalWrite(STEPPERPIN3, HIGH);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN1, LOW);
  delay(motorDelay);
  // 4
  digitalWrite(STEPPERPIN4, LOW);
  digitalWrite(STEPPERPIN3, HIGH);
  digitalWrite(STEPPERPIN2, HIGH);
  digitalWrite(STEPPERPIN1, LOW);
  delay(motorDelay);
  // 5
  digitalWrite(STEPPERPIN4, LOW);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN2, HIGH);
  digitalWrite(STEPPERPIN1, LOW);
  delay(motorDelay);
  // 6
  digitalWrite(STEPPERPIN4, LOW);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN2, HIGH);
  digitalWrite(STEPPERPIN1, HIGH);
  delay (motorDelay);
  // 7
  digitalWrite(STEPPERPIN4, LOW);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN1, HIGH);
  delay(motorDelay);
  // 8
  digitalWrite(STEPPERPIN4, HIGH);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN1, HIGH);
  delay(motorDelay);
}
