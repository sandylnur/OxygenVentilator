// potentiometer connected pins
  const int potOnePin = A1;    // potentiometer one
  const int potTwoPin = A2;    // potentiometer two
  const int potThreePin = A3;  // potentiometer three

// function to get the potentiometer input "potentioOne" "potentioTwo"
  int getPotInput(String potentiometer) {
      if (potentiometer == "bpm_in") {
        int potValueOne = analogRead(potOnePin);
        int potMappedValueOne = map(potValueOne, 0, 1024, 0, 100);
        return potMappedValueOne;
      } 
      else if (potentiometer == "rv_in") {
        int potValueTwo = analogRead(potTwoPin);
        int potMappedValueTwo = map(potValueTwo, 0, 1024, 0, 100);
        return potMappedValueTwo;
      }
      else if (potentiometer =="presets_in") {
        int potValueThree = analogRead(potThreePin);
        int potMappedValueThree = map(potValueThree, 0, 1024, 0, 100); // returns values from 0 to 99
        //  values must be divided into (0 to 19, 19 to 39 .......) and return a string Variable as "PresetOne", "PresetTwo"  


        // errors
        if (potMappedValue == 0) && (potMappedValue == 20){
          potValue = map(val,0,240,0,20);
          return potMappedValueTwo;
        }
        else if (potMappedValue == 21) && (potMappedValue == 40){
          potValue= map (val,241,480,21,40);
          
        }
        else if (potMappedVal == 41) && (potMappedValue == 60) {
          potValue= map (val,481,720, 41,60);
          
        }
        else if (potMappedVal == 61) && ( potMappedValue == 80){
          potValue= map (val,721,960, 61,80);
          
        }
        else if (potMappedVal == 81) && (potMappedValue == 99){
          potValue= map (val,961,1024,81,99);
          
        }
        // errors
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

