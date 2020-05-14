/**
 * Project Name     : MauriVent - Main Control Arduino
 * Version          : Version 0.1
 * Date Created     : 09/09/2020  **DD/MM/YYYY
 * Date Updated     : 
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

// relay, buzzer outputsS
#define buzzerPin 10
#define relayPin  11

// presets
#define IE1to1 1
#define IE1to2 2
#define IE1to3 3
#define IE2to1 0.5

// lcd connectivity to I2C library, I2C Address, Rows, Columns
LiquidCrystal_I2C display(0x3F, 16, 2);

bool ventilatorInitialized = false;
bool checkReceiveFlag = false;

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

  // initialize ventilator and connect to young sister board
  InitializeVentilator();

}

void loop() {
  

}

void SendStartCommand() {

}

void SendStopCommand() {

}

void InitializeVentilator() {
  SetupDisplay();

  digitalWrite(relayPin, LOW);

  CheckForReceiveSignal(true);

  // send command
  SendCommand("zero-in;");

  // check for zero-in
  String _commandReceived = ReceiveCommand();
  if (_commandReceived == "zero-ok") {
    ventilatorInitialized = true;
  }
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

void UpdateDisplay() {
  // ......
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
    }
  }
}

void CheckReceiveISR() {
  checkReceiveFlag = true;
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