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

// emergency button control