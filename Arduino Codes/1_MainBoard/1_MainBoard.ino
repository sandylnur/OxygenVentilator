/**
 * Project Name     : MauriVent - Main Control Arduino
 * Version          : Version 0.1
 * Date Created     : 09/09/2020  **DD/MM/YYYY
 * Date Updated     : 15/05/2020
 * Author           : 
 * Board Name       : Jie Jie
 **/

/** == BEGIN SKETCH == **/
// includes library
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// potentiometer inputs
#define potentioOnePin 0
#define potentioTwoPin 1
#define potentioThreePin 2

// switches inputs
#define limitSwitchPin 3
#define checkReceivePin 2
//TODO declare push button pin

// lcd outputs
#define sdaLCDPin 4
#define sclLCDPin 5

// relay, buzzer outputs
#define buzzerPin 10
#define relayPin  11

// presets
#define IE1to1 1
#define IE1to2 2
#define IE1to3 3
#define IE2to1 0.5

// push button pin
#define pushButtonPin 6

#define checkSendPin 4

#define emergencySignal 12

// long press push button
long unsigned beginButtonTime;
bool initialPress, checkButtonPass, successLongPress = false;
const int longPressTime = 3000;

// single press push button
const int debounceTime = 500;


// lcd connectivity to I2C library, I2C Address, Rows, Columns
LiquidCrystal_I2C display(0x3F, 16, 2);

bool ventilatorInitialized = false;
bool checkReceiveFlag = false;  

bool isVentilatorRunning = false;

int lastReadBPM, lastReadRV;
float lastReadIE;

int runningBPM, runningRV;
float runningIE;

// parameters for prototype 3, values to be calibrated and change accordingly
float actuationDistance = 50;     // value in mm
float pionRadius        = 29.75;  // value in mm
float shaftRadius       = 7.88;   // value in mm
const int stepsPerRevolution = 200;

String inhalationMSteps, inhalationMS, exhalationMSteps, exhalationMS;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB
  }

  // setup for push button, use 3rd param as LOW, HIGH, CHANGE, FALLING, RISING
  // pinMode(limitSwitchPin, INPUT);
  // attachInterrupt(digitalPinToInterrupt(limitSwitchPin), LimitSwitchISR, FALLING);

  // check pin
  pinMode(checkReceivePin, INPUT);
  attachInterrupt(digitalPinToInterrupt(checkReceivePin), CheckReceiveISR, RISING);

  // relay
  pinMode(relayPin, OUTPUT);
  pinMode(emergencySignal, INPUT);


  pinMode(checkSendPin, OUTPUT);
  digitalWrite(checkSendPin, LOW);

  // push button
  pinMode(pushButtonPin, INPUT);

  // initialize ventilator and connect to young sister board
  InitializeVentilator();

}

void InitializeVentilator() {
  SetupDisplay();

  digitalWrite(relayPin, LOW);

  // CheckForReceiveSignal(true);
  while (digitalRead(checkReceivePin) == 0) {
    ;
  }

  SendCommand("zero-in;");

  // check for zero-in
  String _commandReceived = ReceiveCommand();
  if (_commandReceived == "zero-ok") {
    ventilatorInitialized = true;
  }
  checkReceiveFlag = false;
  display.setCursor(0, 0);
  display.print("     *Ready     ");
}

void loop() {
  // check for button click from Mei Mei
  ToBeNamed();

  UpdateDisplay(true); // true -> in real-time

  CheckLongPressButton();

  // TODO  change high or low depending on TAVISH work.....
  if (digitalRead(emergencySignal) == HIGH) {
    buzzer();
  }
}

void CheckLongPressButton() {
    if (digitalRead(pushButtonPin) == HIGH) {
        if (initialPress == false) {
            beginButtonTime = millis();
            initialPress = true;
        } else if (initialPress == true && checkButtonPass == false && (millis() - beginButtonTime) > longPressTime) {
            checkButtonPass = true;
        }
        if (checkButtonPass == true && successLongPress == false) {
            successLongPress = true;
            // TODO Add action for LongPress
            /**
             * 
             * Send Signal to MotorBoard to stop
             * receive confirmation to stop
             * 
             **/
            buzzer();
            digitalWrite(checkSendPin, HIGH);
            digitalWrite(checkSendPin, LOW);

        }
    } else if (digitalRead(pushButtonPin) == LOW) {
        if (initialPress == true) {
            initialPress        = false;
            checkButtonPass     = false;
            successLongPress    = false;
        }
    }
}

//TODO Name this freaking variable
void ToBeNamed() {
  /**
   * if check for received signal is true
   *    if ventilator not running
   *      SEND COMMAND - real time values
   *      update display on first row with running values
   *      set ventilator status to running
   *    else 
   *      SEND COMMAND - real time values
   *      update display on first row with running values
   **/
  if (CheckForReceiveSignal(false) == true) {
    buzzer();
    UpdateDisplay(false);
    ProcessFormulae();
    String _commandToSend = inhalationMSteps + ":" + inhalationMS + ":" + exhalationMSteps + ":" + exhalationMS + ";";
    SendCommand(_commandToSend);
    
  } /* else {
    // SendCommand(""); // TODO find a proper command structure
    UpdateDisplay(false);
  } */
}

void ProcessFormulae() {
  // inhalation, clockwise
    float repirationDuration        = 60 / runningBPM;
    float inhalationTime            = repirationDuration / (1 + runningIE);
    float inhalationRPM             = (actuationDistance * runningRV * 60 * 7) / (inhalationTime * 44 * (pionRadius - shaftRadius) * (100));
    float inhalationNumberOfSteps   = (inhalationRPM * inhalationTime * stepsPerRevolution) / 60; // 400 -> stepsPerRevolution

  // exhalation, anti-clockwise
    float exhalationRPM             = inhalationRPM / runningIE;
    float exhalationNumberOfSteps   = inhalationNumberOfSteps;

  // inhalation, conversion for stepper
    float numOfInhaMicroSteps = inhalationNumberOfSteps * 16;
    int inhalationMotorMicroSteps = int(numOfInhaMicroSteps); // 3200

    float inhalFactorDelMicroSec = (inhalationTime * 1000000) / (2 * inhalationMotorMicroSteps);
    int inhalFacDelMicroSec = int(inhalFactorDelMicroSec);

  // exhalation, conversion for stepper
    int exhalationMotorMicroSteps = inhalationMotorMicroSteps;

    float exhalFactorDelMicroSec = ((inhalationTime * runningIE) * 1000000) / (2 * inhalationMotorMicroSteps);
    int exhalFacDelMicroSec = int(exhalFactorDelMicroSec);
    

  inhalationMSteps = inhalationMotorMicroSteps;
  inhalationMS     = inhalFacDelMicroSec;
  exhalationMSteps = exhalationMotorMicroSteps;
  exhalationMS     = exhalFacDelMicroSec;
}

void SetupDisplay() {
  // set backlight
  display.init();
  display.backlight();

  // initial display
  display.setCursor(0, 0);
  display.print("  Initializing  ");
  display.setCursor(0, 1);
  display.print("   *MauriVent   ");
}

void UpdateDisplay(bool _inRealTime) {
  int _realBPM = int(GetPotInput("BPM"));
  int _realRV  = int(GetPotInput("RV"));
  float _realIE  = GetPotInput("PRESETS");
  String _toDisplay, _toPrintBPM, _toPrintRV, _toPrintIE;

  //if (_realBPM != lastReadBPM || _realRV != lastReadRV || _realIE != lastReadIE) {
    /*
    lastReadBPM = _realBPM;
    lastReadRV  = _realRV;
    lastReadIE  = _realIE;
    */

    // string to print BPM
    _toPrintBPM = String(_realBPM);

    // string to print RV
    if (int(_realRV) >= 100) {
      _toPrintRV = String(_realRV);
    } else if (int(_realRV) < 10) {
      _toPrintRV = "  " + String(_realRV);
    } else if (int(_realRV) < 100) {
      _toPrintRV = " " + String(_realRV);
    }

    // string to print I:E
    if (_realIE == IE1to1) {
      _toPrintIE = "1:1";
    } else if (_realIE == IE1to2) {
      _toPrintIE = "1:2";
    } else if (_realIE == IE1to3) {
      _toPrintIE = "1:3";
    } else if (_realIE == IE2to1) {
      _toPrintIE = "2:1";
    }

    _toDisplay = _toPrintBPM + "    " + _toPrintRV + "    " + _toPrintIE;
  //}

  if (_inRealTime == false) {
    display.setCursor(0, 0);
    display.print(_toDisplay);

    runningBPM = _realBPM;
    runningRV  = _realRV;
    runningIE  = _realIE;

  } else {
    display.setCursor(0, 1);
    display.print(_toDisplay);
  }
}

float GetPotInput(String _potentiometer) {
    if (_potentiometer == "BPM") {
      int _potValueOne = analogRead(potentioOnePin);
      int _potMappedValueOne = map(_potValueOne, 0, 1023, 10, 31);  // return 10 to 30
      return _potMappedValueOne;
    }

    else if (_potentiometer == "RV") {
      int _potValueTwo = analogRead(potentioTwoPin);
      int _potMappedValueTwo = map(_potValueTwo, 0, 1023, 0, 101);    // return 0 to 100
      return _potMappedValueTwo;
    }

    else if (_potentiometer == "PRESETS") {
      int _potValueThree = analogRead(potentioThreePin);
      int _potMappedValueThree = map(_potValueThree, 0, 1023, 0, 99);
      int _potDiscreteValue = _potMappedValueThree / 25;
      
      switch(_potDiscreteValue)
      {
        case 0:
          return IE1to1;
          break;
        case 1:
          return IE1to2;
          break;
        case 2:
          return IE1to3;
          break;
        case 3:
          return IE2to1;
          break;
      }
    }
}

bool CheckForReceiveSignal(bool _waitForCheck) {
  if (_waitForCheck == true) {
    while (checkReceiveFlag == false) {
      if (checkReceiveFlag == true) {
        break;
      }
    }
    checkReceiveFlag = false;
    return true;
  } else {
    if (checkReceiveFlag == true) {
      checkReceiveFlag = false;
      return true;
    } else {
      return false;
    }
  }
}

void CheckReceiveISR() {
  checkReceiveFlag = true;
}

void buzzer() {
    tone(buzzerPin, 1000, 500);
}

// TODO to be transfered to a class
#pragma region SC 
void SendCommand(String _command) {
  int _commandLength = _command.length() + 1;
  char _commandToSend[_commandLength];
  _command.toCharArray(_commandToSend, _commandLength);

  // send the command
  Serial.write(_commandToSend);
  
  // wait for all data to be sent
  Serial.flush();

  // check if command was sent
  bool _sentStatus = false;
  while (_sentStatus == false) {
    if (Serial.available() > 0 ) {
      // read string if available until character ";"
      String _receivedCommand = Serial.readStringUntil(';');
      if (_receivedCommand == "Ok") {
        _sentStatus = true;
      }
    }
  }
}

String ReceiveCommand() {
// keep checking until a command is received
  bool _receiveStatus = false;
  while (_receiveStatus == false) {
    if (Serial.available() > 0 ) {
      // read string if available until character ";"
      String _receivedCommand = Serial.readStringUntil(';');
      Serial.write("Ok;");
      Serial.flush();
      _receiveStatus = true;
      return _receivedCommand;
    }
  }
}
#pragma endregion