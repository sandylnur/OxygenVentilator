/**
 * Emergency Protocol Code
 * Version          : 0.3
 * Author           : Sandyl Nursigadoo, Tavish Hookoom
 * Date Created     : 12/04/2020 (DD :MM :YYYY)
 * Date Modified    : 25/04/2020
 **/

#define Debug // debugging message for emergency verification (uncomment to see the debug message on serial monitor)

// connected arduino pins
const int emergencyButtonPin = 2;
const int limitSwitchPin     = 3;
const int relaySwitchPin     = 9;
const int buzzerPin          = 10;
const int ventEnablePin      = A0;

// flag for status of emergency scenario
bool initializationCheck    = false;
bool ventStatus      = false;
bool emergencyMotor  = false;
bool emergencyToggle = false;

// calibration time for maximum time for limit switch to be trigger
const int timeCalibration       = 7000;   // maximum inhalation plus exhalation time
const int bootTimeCalibration   = 14000;  // maximum zero-in time
unsigned long counterLastPress;

// flag for if limit switch was toggled
bool limitSwitchFlag;

void setup() {
  // put your setup code here, to run once:
  // emergency button
  pinMode(emergencyButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(emergencyButtonPin), EmergencyISR, FALLING);

  // limit switch
  pinMode(limitSwitchPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(limitSwitchPin), LimitSwitchISR, CHANGE);

  // relay switch
  pinMode(relaySwitchPin, OUTPUT);
  digitalWrite(relaySwitchPin, HIGH);

  // buzzer
  pinMode(buzzerPin, OUTPUT);

  // ventilator status from mainboard
  pinMode(ventEnablePin, INPUT);

  #ifdef Debug
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB
    }
  #endif
}

void loop() {
  #pragma Emergency Protocol
    if (emergencyToggle == true || emergencyMotor == true) {
      // whenever emergency button or and error with limit switch identifying an issue on motor
      digitalWrite(relaySwitchPin, LOW);
      digitalWrite(buzzerPin, HIGH);
      #ifdef Debug
        Serial.println("Emergency Occured");
      #endif
    }

    if (digitalRead(emergencyButtonPin) == HIGH && emergencyToggle == true) {
      // reset when evergency button is released
      digitalWrite(relaySwitchPin, HIGH);
      digitalWrite(buzzerPin, LOW);
      emergencyToggle = false;
      emergencyMotor = false;
      ventStatus = false;
      initializationCheck = false;
    }
  #pragma endregion

  if (initializationCheck == false) {    
    #ifdef Debug
      Serial.println("Initializing Procedure Started");
    #endif
    
    while(digitalRead(ventEnablePin) == LOW) {
      CheckMotorRunning(bootTimeCalibration);
      if(emergencyMotor == true){
        break;
      }
    } 
    initializationCheck = true;
    #ifdef Debug
      Serial.println("Initializing Procedure Completed");
    #endif
  }

  if (initializationCheck == true) {
    CheckMotorRunning(timeCalibration);
  }

}

void CheckMotorRunning(int _timeCheck) {
    if (digitalRead(ventEnablePin) == LOW && ventStatus == false) {
      ventStatus = true;
      counterLastPress = millis();
      #ifdef Debug
        Serial.println("Ventilator Operational");
      #endif
    } else if (digitalRead(ventEnablePin) == HIGH && ventStatus == true) {
      ventStatus = false;
      #ifdef Debug
        Serial.println("Ventilator Not Operational");
      #endif
    }

    if (ventStatus == true && emergencyMotor == false) {
      if (limitSwitchFlag == 1) {
        #ifdef Debug
          Serial.println("Tap");
        #endif
        counterLastPress = millis();
        limitSwitchFlag = 0;
      }

      int pinState = digitalRead(limitSwitchPin);

      if (millis() - counterLastPress >= _timeCheck) {

        Serial.println(millis() - counterLastPress);
        emergencyMotor = true;
        if (pinState == HIGH) {
          #ifdef Debug
            Serial.println("Error! Constant High!!");
          #endif
          emergencyMotor = true;
        } else {
          #ifdef Debug
            Serial.println("Error! Missed Limit Switch!!");
          #endif
          emergencyMotor = true;
        }
      }
    }
}

void EmergencyISR() {
  digitalWrite(relaySwitchPin, LOW);
  digitalWrite(buzzerPin, HIGH);
  emergencyToggle = true;
}

void LimitSwitchISR() {
  limitSwitchFlag = 1;
}