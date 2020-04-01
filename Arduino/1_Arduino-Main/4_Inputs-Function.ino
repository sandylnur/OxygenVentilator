// potentiometer connected pins
  const int potOnePin = A1;    // potentiometer one
  const int potTwoPin = A2;    // potentiometer two

// function to get the potentiometer input "potentioOne" "potentioTwo"
  int getPotInput(String potentiometer) {
      if (potentiometer == "bpm_in") {
        int potValue = analogRead(potOnePin);
        int potMappedValue = map(potValue, 0, 1024, 0, 100);
        return potMappedValue;
      } 
      else if (potentiometer == "rv_in"){
        int potValue = analogRead(potTwoPin);
        int potMappedValue = map(potValue, 0, 1024, 0, 100);
        return potMappedValue;
      }
  }

// button control
  const int epushBtnPin = 5;   // push button input pin
  int epushStatus = 0;

  void setupCheckBtn() {
    pinMode(epushBtnPin, INPUT); // declare pushbutton as input
  }

  void checkBtnInput() {
    epushStatus = digitalRead(epushBtnPin);  // read input value

    if (epushStatus == HIGH) {         // button being pushed
        
        delay(500);

        if (bool check = checkVentilator() == false){
          startVentilator();

        } else {
          stopVentilator();
        }

    } else {
      // button not being pushed
    }
  }

// emergency button input
  bool emergency = false;
  const int emerBtnPin = 4;   // push button input pin
  int emerStatus = 0;

  void setupEmergencyBtn() {
    pinMode(emerBtnPin, INPUT); // declare pushbutton as input
  }

  void emerBtnInput() {
    emerStatus = digitalRead(emerBtnPin);  // read input value

    if (emerStatus == HIGH) {         // button being pushed
        
        delay(500);

        if (emergency == false){
          Serial.println("Emmergency Occured");
          // stop the ventilator
          stopVentilator();
          emergency = true;
          //EEPROM.write(1, 1);

        } else {
          Serial.println("Emmergency Resolved");
          emergency = false; // testing only
        }
        
    } else {
      // button not being pushed
    }
  }

  void emergencyAction(){
    if (emergency == true){
      buzzer();
    }
  }

  int eepromValueOne = 0;
  const int eepromValueCheck = 0;

  void verifyEmergency() {
    int eepromValue = EEPROM.read(0);
    int eepromValueOne = EEPROM.read(1);

    if (eepromValueCheck == 0) {
      Serial.println("A Power Error Occured");
      // stop the ventilator
      stopVentilator();
      emergency = true;

      if (eepromValue == 1) {
        Serial.println("Ventilator was active");
        emergency = true;
        //startVentilator();

      } else {
        Serial.println("Ventilator was in-active");

      }
    }
  }

