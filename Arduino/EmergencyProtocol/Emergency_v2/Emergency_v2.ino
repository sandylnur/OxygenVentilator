// Emergency Protocol Code Version 0.1, by Sandyl
// Latest Date Modified: 12/04/2020
// connected pins
const int emergencyButton = 2;
const int limitSwitch     = 3;
const int pushLed         = 8;
const int relaySwitch     = 9;
const int buzzer          = 10;

int ventEnablePin = A1;

bool emergencyToggle = false;
bool emergencyMotor  = false;
bool ventStatus      = false;

const int timeCalibration = 7000;
unsigned long counterStart, counterPress, counterLastPress;
int counterLimitSwitch = 1;
int counterLastLimitSwitch = 0;

// debounces
int lastButtonState = 1;
long unsigned int lastPress;
volatile int buttonFlag;
int debounceTime = 50;

void setup() {
  // put your setup code here, to run once:
  // emergency button
    pinMode(emergencyButton, INPUT);
    attachInterrupt(digitalPinToInterrupt(emergencyButton), emergencyISR, FALLING);

  // limit switch
    pinMode(limitSwitch, INPUT);
    attachInterrupt(digitalPinToInterrupt(limitSwitch), limitSwitchISR, RISING);

  // relay switch
    pinMode(relaySwitch, OUTPUT);
    digitalWrite(relaySwitch, HIGH);

    pinMode(ventEnablePin, INPUT);
    pinMode(pushLed, OUTPUT);
    pinMode(buzzer, OUTPUT);

    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
}

void loop() {
  if (emergencyToggle == true || emergencyMotor == true){
    digitalWrite(relaySwitch, LOW);
    digitalWrite(buzzer, HIGH);
  }
  
  if (digitalRead(emergencyButton) == HIGH && emergencyToggle == true){
    digitalWrite(relaySwitch, HIGH);
    digitalWrite(buzzer, LOW);
    emergencyToggle = false;
    emergencyMotor = false;
  }

  if (digitalRead(ventEnablePin) == LOW && ventStatus == false){
    ventStatus = true;
    digitalWrite(pushLed, HIGH);
    //counterLastPress = millis();
    // Serial.println("Ventilator Operational");
  } else if (digitalRead(ventEnablePin) == HIGH && ventStatus == true) {
    ventStatus = false;
    digitalWrite(pushLed, LOW);
    // Serial.println("Ventilator Not Operational"); 
  }

  if (ventStatus == true && emergencyMotor == false) {
    if((millis() - lastPress) > debounceTime && buttonFlag){ 
        lastPress = millis();
        if(digitalRead(limitSwitch) == 0){ // && lastButtonState == 1
          //Serial.println("Tap");
          counterLimitSwitch++;
          lastButtonState = 0;
        }
        
        else if(digitalRead(limitSwitch) == 1 && lastButtonState == 0){
        lastButtonState = 1;
        }
        buttonFlag = 0;
    }
    
    counterPress = millis();
    int pinState = digitalRead(limitSwitch);
    
    if (counterPress - counterLastPress >= timeCalibration) { // countdown for timeCalibration inside loop
      counterLastPress = counterPress;
      if (pinState == HIGH){
          Serial.println("Error!!");
          emergencyMotor = true;
      } else {
        if (counterLimitSwitch > counterLastLimitSwitch){
          counterLastLimitSwitch = counterLimitSwitch;
        } else {
          Serial.println("Error!!");
          emergencyMotor = true;
        }
      }

      
    }
  }
}

// Emergency Interrupt Service Routine should turn off the arduino cct using Relay 1 and activate emergency circuit
void emergencyISR() {
    emergencyToggle = true;
}

void limitSwitchISR() {
    buttonFlag = 1;
}