// ---------------------------------------------------------------------------
// This code shows a simple controller for a 3D PotterBot extruder only
// A Button is used to enable or disable the extruder
// A 24 v Relay shield is used to read I/Os from the robot to activate the tool (if required)
// The max. and minimum ranges of speed are controlled through a Potentiometer
// LCD displays Extruder state (Enable/ Disabled, ON, OFF and Speed %)
// 2016 Susana Alarcon
// v 1.0 


#include <SoftwareSerial.h>

//Kuka Pin for reading I/O 2
const int KukaPin = 2;

//LCD 
SoftwareSerial myLCD(3, 4); // pin 3 = TX, pin 4 = RX (unused)
char powerstring[10], spdstring[10]; // create string arrays

//fan is controlled through a Relay
const int fanPin = 10;

//
const int driverPin = 11;
const int dirPin = 8;
const int stepperPin = 9;

//Potentiometer
const int potPin = A0;

String powerState;
int speedExtrude, speedPrint;
int KukaPinState, potValue, reading, extruderState;
boolean secondOFF = false;


// Variables button:
const int buttonPin = 7;
int driverState = LOW;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 30;    // the debounce time


void setup() {

  //LCD setup
  myLCD.begin(9600);   // set up serial port for 9600 baud
  delay(500);          // wait for display to boot up
  myLCD.write(254);    // cursor to beginning of first line
  myLCD.write(128);
  myLCD.write("Power:          "); // clear display + legends
  myLCD.write("Speed:          ");

  pinMode(KukaPin, INPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepperPin, OUTPUT);
  pinMode(potPin, INPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(driverPin, OUTPUT);
  digitalWrite(driverPin, driverState);
  delay(1000);

}

void readKuka () {
  //Uncomment if not reading from Kuka I/O
  //KukaPinState = LOW;
  if (KukaPinState == LOW) {
    powerState = "  ON     ";
    digitalWrite(fanPin, HIGH);
    secondOFF = false;
  } else {
    powerState = " OFF     ";
    digitalWrite(fanPin, LOW);
    //Retraction test
    //if (secondOFF == false) {
    //step(false, 2000);
    //secondOFF= true;
    //}
  }
  //Turn ON/OFF extruder
  digitalWrite(driverPin, !KukaPinState);
  //digitalWrite(fanPin, driverState);
}

//Function reads I/O from Kuka to enable/disable the extruder
void readPower() {
  reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      // only toggle the extruderState (Enabled/Diabled)  if the new button state is HIGH
      if (buttonState == HIGH) {
        extruderState = !extruderState;
      }
    }
  }
  lastButtonState = reading;
  if (extruderState == HIGH) {
    //powerState = "  ENABLED ";
    readKuka();
  } else {
    powerState = " DISABLED ";
    digitalWrite(driverPin, LOW);
  }
}

//Function reads potentiometer to change extrusion speed and prints data
void readPot() {
  potValue = analogRead(potPin);
  // 500 and 2000 were extreme working values according to test
  speedExtrude = map (potValue, 0, 1023, 500, 2000);
  // 0-100% of speed allowance
  speedPrint = map (speedExtrude, 500, 2000, 100, 0);

  //Print feedback data in LCD
  sprintf(spdstring, "%4d", speedPrint); // right-justify to 4 spaces
  //Print POWER ON/OFF state
  myLCD.write(254); // cursor to 7th position on first line
  myLCD.write(134);
  myLCD.print(powerState);
  //Print SPEED %
  myLCD.write(254);
  myLCD.write(198);
  myLCD.write(spdstring);

}

//Step function
void step(boolean dir, int steps) {
  if (dir)
    digitalWrite(dirPin, HIGH);
  else
    digitalWrite(dirPin, LOW);
  for (int i = 0; i < steps; i++) {
    delayMicroseconds(speedExtrude);
    digitalWrite(stepperPin, HIGH);
    delayMicroseconds(speedExtrude);
    digitalWrite(stepperPin, LOW);
  }
}


void loop() {
  readPower();
  readPot();
  // The motor should revolve one revolution/200 steps in one direction
  step(true, 200);
  //delay(5);
}
