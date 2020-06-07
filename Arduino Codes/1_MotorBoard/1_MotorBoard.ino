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
#define checkReceivePin 3

const int buttonDebounceDelay = 1000;
unsigned long buttonlastDebounceTime;
bool buttonFlag, buttonFirstToggle = false;

bool isVentilatorRunning = false;

int inhalationMSteps, inhalationMS, exhalationMSteps, exhalationMS;

int longPressFlag = 0;

void setup() {
  // put your setup code here, to run once:

  // setup pin mode for inputs then outputs
  pinMode(pushButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pushButtonPin), PushButtonISR, FALLING);

  // check pin
  pinMode(checkReceivePin, INPUT);
  attachInterrupt(digitalPinToInterrupt(checkReceivePin), CheckReceiveISR, RISING);

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
  
  if (longPressFlag == 0) {
    ButtonClicked(CheckForButtonClick());
  } else if (longPressFlag == 1) {
    isVentilatorRunning = false;
    longPressFlag = 0;
    buttonFlag = false;
  }

  OperateVentilator();
}

void ButtonClicked(bool _wasButtonClicked) {
  if (_wasButtonClicked == true) {
    //if (isVentilatorRunning == true) {
      // ventilator is in stop position and will now initiate
      /**
       * send check signal
       * receive command containing new values
       * set new values to variables
       * begin operation with new updated values
       * 
       **/
      SendCheckSignal();
      // inhalationMSteps + ":" + inhalationMS + ":" + exhalationMSteps + ":" + exhalationMS + ";"
      String _commandToDissect = ReceiveCommand();


      char _characterRead = "";
      int _characterPosition      = 0,
          _characterReadPosition  = 0,
          _numberOfParameters     = 3;

      String _parameterArray[_numberOfParameters + 1];

      for (int parameter = 0; parameter <= _numberOfParameters; parameter++) {
        String _parameterString;
        
        while (_characterRead != ':' && _characterReadPosition != _commandToDissect.length() ) {
          _characterRead = _commandToDissect[_characterReadPosition];
          
          if (_characterRead != ':') {
            _parameterString = _parameterString + _characterRead;
          }
        
          _characterReadPosition++;
        }
        
        _parameterArray[parameter] = _parameterString;
        _parameterString  = "";
        _characterRead    = "";
      }

      inhalationMSteps = _parameterArray[0].toInt();
      inhalationMS     = _parameterArray[1].toInt();
      exhalationMSteps = _parameterArray[2].toInt();
      exhalationMS     = _parameterArray[3].toInt();

      isVentilatorRunning = true;
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

    /* keep rotating motor clockwise to check for limit-switch
    while (digitalRead(limitSwitch) == 0) {
      RunMotor(1, exhalationMS, false); // motorMicroSteps, motorMicroSeconds, Clock-Wise -> TRUE, Anti-Clock-Wise -> FALSE
    }
    */
  }
}

void RunMotor(int motorMicroSteps, int motorMicroSeconds, bool direction) {
  if (direction == true){
    digitalWrite(dirPin, HIGH);
  } else if (direction == false){
    digitalWrite(dirPin, LOW);
  }

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

void CheckReceiveISR() {
  longPressFlag = 1;
}
