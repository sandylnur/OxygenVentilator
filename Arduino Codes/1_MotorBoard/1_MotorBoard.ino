/**
 * Project Name     : MauriVent - Motor Arduino
 * Version          : Version 0.1
 * Date Created     : 09/09/2020  **DD/MM/YYYY
 * Date Updated     : 15/05/2020
 * Author           : 
 * Board Name       : Mei Mei
 **/

/* == BEGIN SKETCH == */
#define enaPin 8
#define dirPin 9
#define pulPin 10

#define pushButtonPin 2
#define limitSwitch 5

#define checkSendPin 7

const int buttonDebounceDelay = 1000;
unsigned long buttonlastDebounceTime;
bool buttonFlag, buttonFirstToggle = false;

bool isVentilatorRunning = false;

int inhalationMSteps, inhalationMS, exhalationMSteps, exhalationMS;

void setup() {
  // put your setup code here, to run once:

  // setup pin mode for inputs then outputs
  pinMode(pushButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pushButtonPin), PushButtonISR, FALLING);

  pinMode(limitSwitch, INPUT);

  // motor driver control pins
  pinMode(enaPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(pulPin, OUTPUT);

  pinMode(checkSendPin, OUTPUT);

  // begin serial at 9600, shall be same as mainboard
  Serial.begin(9600);

  // initialize ventilator and connect to Jie Jie board
  InitializeVentilator();

}

void loop() {
  // put your main code here, to run repeatedly:
  ButtonClicked(CheckForButtonClick());

  //OperateVentilator();
}

void ButtonClicked(bool _wasButtonClicked) {
  if (_wasButtonClicked == true) {
    if (isVentilatorRunning == true) {
      // ventilator is in stop position and will now initiate
      /**
       * send check signal
       * receive command containing new values
       * set new values to variables
       * begin operation with new updated values
       * 
       **/
      SendCheckSignal();
      
    } else {
      // ventilator is in stop position and will now initiate
      /**
       * send check signal
       * receive command containing values to begin operation
       * set values to variables
       * set status to allow ventilator to operate
       * 
       **/
      SendCheckSignal();

      isVentilatorRunning = true;
    }
  }
}

void InitializeVentilator() {
  digitalWrite(checkSendPin, HIGH);
  // SendCheckSignal();

  String _commandReceived = ReceiveCommand();
  if (_commandReceived == "zero-in") {
    while (digitalRead(limitSwitch) == 0) {
      RunMotor(16, 2500, false);
    }
  }
  
  SendCommand("zero-ok;");
  digitalWrite(checkSendPin, LOW);
}

void SendCheckSignal() {
  digitalWrite(checkSendPin, HIGH);
  digitalWrite(checkSendPin, LOW);
}

void OperateVentilator() {
  if (isVentilatorRunning == true) {
    // execute series of steps for motor to perform inhalation and exhalation
    // ...
    // motor run clockwise
    RunMotor(inhalationMSteps, inhalationMS, true); // motorMicroSteps, motorMicroSeconds, Clock-Wise -> TRUE, Anti-Clock-Wise -> FALSE
    // motor run anti-clockwise
    RunMotor(exhalationMSteps, exhalationMS, false); // motorMicroSteps, motorMicroSeconds, Clock-Wise -> TRUE, Anti-Clock-Wise -> FALSE

    // keep rotating motor clockwise to check for limit-switch
    while (digitalRead(limitSwitch) == 0) {
      RunMotor(1, exhalationMS, false); // motorMicroSteps, motorMicroSeconds, Clock-Wise -> TRUE, Anti-Clock-Wise -> FALSE
    }
  }
}

void RunMotor(int motorMicroSteps, int motorMicroSeconds, bool direction) {
  for (int index = 0; index < (motorMicroSteps); index++) {
    digitalWrite(pulPin, HIGH); 
    delayMicroseconds(motorMicroSeconds);
    digitalWrite(pulPin, LOW);
    delayMicroseconds(motorMicroSeconds);
  }
}

bool CheckForButtonClick() {
  if (buttonFlag == true) {
    if ((millis() - buttonlastDebounceTime) > buttonDebounceDelay) {
      buttonFirstToggle = false;
    }
    if (buttonFirstToggle == false) {
      buttonlastDebounceTime = millis();
      buttonFirstToggle = true;
      // run your action once on button click
        buttonFlag = false;
        return true;
    }
    buttonFlag = false;
    return false;
  } else {
    return false;
  }
}

void PushButtonISR() {
  buttonFlag = true;
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