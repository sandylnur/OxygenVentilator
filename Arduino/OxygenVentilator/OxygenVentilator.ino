/*
 *  Date: 04/04/2020 (DD:MM:YYYY)
 *  Updated on: 08/04/2020
 *  OxygenVentilator Project - Rack and Pinion
 * 
 */

#include <LiquidCrystal.h>

// #define fomula_debug // debugging message for formulae verification (uncomment to see the debug message on serial monitor) 

// push button variables
  int pushButtonPin = 3;
  int lastButtonState = 1;
  long unsigned int lastPress;
  volatile int buttonFlag;
  int debounceTime = 50;

// ventilator variables
  int ventStatus = 0;

// presets
  #define idle 0    // to be taken out if not necessary
  #define IE1to2 2
  #define IE1to3 3
  #define IE1to1 1
  #define IE2to1 0.5

// potentiometer connected pins
  const int potOnePin   = A0; // potentiometer one
  const int potTwoPin   = A1; // potentiometer two
  const int potThreePin = A2; // potentiometer three

// parameters for prototype 3, values to be calibrated and change accordingly
  float actuationDistance = 70;     // value in mm
  float pionRadius        = 29.75;  // value in mm
  float shaftRadius       = 7.88;   // value in mm

// buzzer pin
  const int piezoPin = 7;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
  const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// variables for display output
  String bpmLCD;
  String rvLCD;
  String ieLCD;

// declare pins for motor
  const int stepPin = 6;  //PUL -Pulse 6
  const int dirPin = 4;   //DIR -Direction 4
  const int enPin = 5;    //ENA -Enable 5

// declare limit switch pin
  const int limitSwitch = A5;
  int bootCheck   = 0;

// varibles for starting and looping position of motor
  int inhalationMSteps, inhalationMS, exhalationMSteps, exhalationMS;

void setup() {
  // put your setup code here, to run once:

  // start serial
  #ifdef fomula_debug
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
  #endif
  
  // setup display
    setupDisplay();
  
  // setup pin modes
    pinMode(pushButtonPin, INPUT_PULLUP);
    pinMode(limitSwitch, INPUT);
  
  // setup for push button, use 3rd param as LOW, HIGH, CHANGE, FALLING, RISING
    attachInterrupt(digitalPinToInterrupt(pushButtonPin), pushButtonISR, CHANGE);

  // setup pins for motor, as outputs
    pinMode(stepPin, OUTPUT); 
    pinMode(dirPin, OUTPUT);
    pinMode(enPin, OUTPUT);

  // boot position calibration protocol 
    bootProtocol();
}

void loop() {
  // put your main code here, to run repeatedly:
  // check if button is pressed
    checkButton();

  // if ventilator started do
    ventilatorStarted();
    
  // update display in real time only if ventilator is not running
    updateDisplay();
}

// boot protocol to zero position of stepper
void bootProtocol() {
  digitalWrite(enPin, LOW);
  
  if (bootCheck == 0){
    while (digitalRead(limitSwitch) == 0){
      runMotor(16, 2500, false); // motorMicroSteps, motorMicroSeconds, Clock-Wise -> TRUE, Anti-Clock-Wise -> FALSE
    }
    bootCheck = 1;
  }

  digitalWrite(enPin, HIGH);
}

// push button controls
#pragma region 
void pushButtonISR (){
  buttonFlag = 1;
  //Serial.println("ISR");
  //checkButton();
  buzzer();
}

void checkButton(){
  if((millis() - lastPress) > debounceTime && buttonFlag){ 
    lastPress = millis();
    if(digitalRead(pushButtonPin) == 0){ // && lastButtonState == 1
      Serial.println("Button Triggered");
      switch (ventStatus) {
        case 0:
          startVentilator();
          ventStatus = 1;
        break;

        case 1:
          stopVentilator();
          ventStatus = 0;
        break;
      }
      lastButtonState = 0;
    }
    
    else if(digitalRead(pushButtonPin) == 1 && lastButtonState == 0){
      lastButtonState = 1;
    }
    buttonFlag = 0;
  }
}
#pragma endregion

// ventilator start, stop procedures
#pragma region 
// function to start ventilator
void startVentilator(){
  // write code to start ventilator
    Serial.println("Starting ventilator");
    buzzer();
    
  // inhalation, clockwise
    float repirationDuration        = 60 / getPotInput("bpm_in");
    float inhalationTime            = repirationDuration / (1 + getPotInput("presets_in"));
    float inhalationRPM             = (actuationDistance * getPotInput("rv_in") * 60 * 7) / (inhalationTime * 44 * (pionRadius - shaftRadius) * (100));
    float inhalationNumberOfSteps   = (inhalationRPM * inhalationTime * 400) / 60; // 400 -> stepsPerRevolution

  // exhalation, anti-clockwise
    float exhalationRPM             = inhalationRPM / getPotInput("presets_in");
    float exhalationNumberOfSteps   = inhalationNumberOfSteps;

  // inhalation, conversion for stepper
    float numOfInhaMicroSteps = inhalationNumberOfSteps * 16;
    int inhalationMotorMicroSteps = int(numOfInhaMicroSteps); // 3200

    float inhalFactorDelMicroSec = (inhalationTime * 1000000) / (2 * inhalationMotorMicroSteps);
    int inhalFacDelMicroSec = int(inhalFactorDelMicroSec);

  // exhalation, conversion for stepper
    int exhalationMotorMicroSteps = inhalationMotorMicroSteps;

    float exhalFactorDelMicroSec = ((inhalationTime * getPotInput("presets_in")) * 1000000) / (2 * inhalationMotorMicroSteps);
    int exhalFacDelMicroSec = int(exhalFactorDelMicroSec);

  //double speed_rpm, int N_step, bool Direction
  #ifdef fomula_debug
    Serial.println("Potentiometer Values");
    Serial.println(getPotInput("bpm_in"));
    Serial.println(getPotInput("rv_in"));
    Serial.println(getPotInput("presets_in"));

    Serial.println();

    Serial.println("Inhalation");
    Serial.println(inhalationMotorMicroSteps);
    Serial.println(inhalFacDelMicroSec);

    Serial.println();

    Serial.println("Exhalation");
    Serial.println(exhalationMotorMicroSteps);
    Serial.println(exhalFacDelMicroSec);
  #endif

  inhalationMSteps = inhalationMotorMicroSteps;
  inhalationMS     = inhalFacDelMicroSec;
  exhalationMSteps = exhalationMotorMicroSteps;
  exhalationMS     = exhalFacDelMicroSec;

  // start motor
    digitalWrite(enPin, LOW);
}

void ventilatorStarted(){
  if (ventStatus == 1){
    // run automated procedure for motor
    // motor run clockwise
    runMotor(inhalationMSteps, inhalationMS, true); // motorMicroSteps, motorMicroSeconds, Clock-Wise -> TRUE, Anti-Clock-Wise -> FALSE
    // motor run anti-clockwise
    runMotor(exhalationMSteps, exhalationMS, false); // motorMicroSteps, motorMicroSeconds, Clock-Wise -> TRUE, Anti-Clock-Wise -> FALSE
  }
}

void stopVentilator(){
  // write code to stop ventilator
    Serial.println("Stopping ventilator");
    buzzer();
    // stop motor
    digitalWrite(enPin, HIGH);
}
#pragma endregion

// inputs and outputs procedures
#pragma region 
// input for potentiometer one, two, three
float getPotInput(String potentiometer) {
    if (potentiometer == "bpm_in") {
      int potValueOne = analogRead(potOnePin);
      int potMappedValueOne = map(potValueOne, 0, 1023, 10, 31); // return 10 to 30
      return potMappedValueOne;
    }

    else if (potentiometer == "rv_in") {
      int potValueTwo = analogRead(potTwoPin);
      int potMappedValueTwo = map(potValueTwo, 0, 1023, 0, 101); // return 0 to 100
      //float fractionOfMaxVol = potMappedValueTwo / 100;
      return potMappedValueTwo;
    }

    else if (potentiometer == "presets_in") {
      int potValueThree = analogRead(potThreePin);
      int potMappedValueThree = map(potValueThree, 0, 1023, 0, 99);
      int potDiscreteValue = potMappedValueThree / 20;
      
      // enter switch case
      switch(potDiscreteValue)
      {
        case 0:
          return IE1to1;
          break;
        case 1:
          return IE1to1;
          break;
        case 2:
          return IE1to2;
          break;
        case 3:
          return IE1to3;
          break;
        case 4:
          return IE2to1;
          break;
        default:
          // Serial.println("Potentiometer 3, Error!");
          break;
      }
    }
}

// output function to call buzzer
void buzzer(){
    tone(piezoPin, 1000, 500);
}

// output function to display
void setupDisplay(){
  // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
  // Print a message to the LCD.
    lcd.setCursor(0,0) ;
    lcd.print("BPM   RV%    I:E");
}

void updateDisplay(){
  if (ventStatus == 0){
    bpmLCD = String(int(getPotInput("bpm_in")));
    if (int(getPotInput("rv_in")) >= 100){
      rvLCD  = "  " + String(int(getPotInput("rv_in")));
    } else if (int(getPotInput("rv_in")) < 10) {
      rvLCD  = "    " + String(int(getPotInput("rv_in")));
    } else if (int(getPotInput("rv_in")) < 100){
      rvLCD  = "   " + String(int(getPotInput("rv_in")));
    }

    if (getPotInput("presets_in") == IE1to1){
      ieLCD = "1:1";
    } else if (getPotInput("presets_in") == IE1to2){
      ieLCD = "1:2";
    } else if (getPotInput("presets_in") == IE1to3){
      ieLCD = "1:3";
    } else if (getPotInput("presets_in") == IE2to1){
      ieLCD = "2:1";
    }
    
    lcd.setCursor(0,1);
    String toPrint = " " + bpmLCD + " " + rvLCD + "    " + ieLCD;
    lcd.print(toPrint);
  }
}

// motorMicroSteps, motorMicroSeconds, Clock-Wise -> TRUE, Anti-Clock-Wise -> FALSE
void runMotor(int motorMicroSteps, int motorMicroSeconds, bool direction) {
    if (direction == true){
      digitalWrite(dirPin, HIGH);
    } else if (direction == false){
      digitalWrite(dirPin, LOW);
    }
    
    for(int x = 0; x < (motorMicroSteps); x++){
      digitalWrite(stepPin, HIGH); 
      delayMicroseconds(motorMicroSeconds);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(motorMicroSeconds);
    }
}
#pragma endregion