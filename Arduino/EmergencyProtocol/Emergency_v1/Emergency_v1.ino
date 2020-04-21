// Emergency Protocol Code Version 0.3, by Tavish
// Latest Date Modified: 12/04/2020
// defining all the inputs
const int Emergencybutton = 2;
const int ventEnabled = A1;
const int limitSwitch = 3;

//defining all the outputs
int LED = 8; //internal LED of Lilypad
int relay1 = 9;
int buzzer = 10;

//defining all the variables
long unsigned int timeAtrest;
long unsigned int timeMoving;
long unsigned int counterStart;
boolean counter1reset;
boolean counterBreak;
boolean emergencyToggle;

//Setup
void setup()
{
    // setup for Emergency button, 3rd parameter to be used as LOW, CHANGE, FALLING, RISING
    pinMode(Emergencybutton, INPUT);
    attachInterrupt(digitalPinToInterrupt(Emergencybutton), emergencyISR, FALLING);

    // defining the inputs and outputs
    pinMode(ventEnabled, INPUT);
    pinMode(limitSwitch, INPUT);
    attachInterrupt(digitalPinToInterrupt(limitSwitch), limitISR, RISING);

    pinMode(buzzer, OUTPUT);
    counter1reset = false;
    counterBreak = false;
    emergencyToggle = false;
    pinMode(relay1, OUTPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(relay1, HIGH);
    digitalWrite(LED, LOW);
        Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
    
}

//Loop
void loop()
{
    //Start counter and signal reset while the ventilator is running
    Serial.println(digitalRead(Emergencybutton));
    if (digitalRead(Emergencybutton) == 1) {
      emergencyToggle == false;
      digitalWrite(relay1,HIGH);
      digitalWrite(buzzer,LOW);
      }
    
    if (digitalRead(ventEnabled) == 0 && emergencyToggle == false)
    {
        counterStart = millis();
        counterBreak = false;
        digitalWrite(LED,HIGH);
        //While the limitSwitch is Open, piston is moving -- Measure movement time and set counter
        if (digitalRead(limitSwitch) == 0) {
          counter1reset = false;
           
          }
        while (digitalRead(limitSwitch) == 0 && counter1reset == false)
        {
            timeMoving = millis() - counterStart;
            counter1reset = false;
            counterBreak = true;
            Serial.println("Before, Moving: " + String(timeMoving));
            //If movement >= 7 sec activate emergency circuit
            if (timeMoving >= 7000)
            {
                digitalWrite(relay1,LOW);
                digitalWrite(buzzer, HIGH);
                if (digitalRead(Emergencybutton) == 1)
                  {
                    break;
                  }
                Serial.println("After, Moving: " + String(timeMoving));
              /**  while (timeMoving >= 7000)
                {
                    digitalWrite(LED, !digitalRead(LED));
                    delay(1000);
                }**/
            }
            //Perform periodic check of vent status and reset if vent disabled
            if (digitalRead(ventEnabled) == 1)
            {
                break;
            }
        }

        //While limitSwitch is Closed and counter hasn't been used already, piston is at rest --> Measure time at rest
        while (digitalRead(limitSwitch) == 1 && counter1reset == true && counterBreak == false)
        {
            timeAtrest = millis() - counterStart;
            counter1reset = true;
            Serial.println("Before Stop: " + String(timeAtrest));
            //If time at rest >= 2s, activate emergency circuit
            if (timeAtrest >= 2000)
            {
              Serial.println("After Stop: " + String(timeAtrest));
                digitalWrite(relay1,LOW);
                digitalWrite(buzzer, HIGH);

                if (digitalRead(Emergencybutton) == 1)
                  {
                    break;
                  }
                    
                /*while (timeAtrest >= 2000)
                {
                    digitalWrite(LED, !digitalRead(LED));
                    delay(2000);
                }*/
            }
            //Perform periodic check of vent status and reset if if vent disabled
            if (digitalRead(ventEnabled) == 1)
            {
               
                break;
            }
        }
    }

    if (digitalRead(ventEnabled) == 1){
      digitalWrite(LED, LOW);
    }
}

//Emergency Interrupt Service Routine should turn off the arduino cct using Relay 1 and activate emergency circuit
void emergencyISR()
{
    digitalWrite(relay1, LOW);
    digitalWrite(buzzer, HIGH);
    emergencyToggle = true;
    
    /*
    while(digitalRead(Emergencybutton) == 1)
    {
        digitalWrite(LED, !digitalRead(LED));
        delay(250);
    }
    */
}

void limitISR()
{
   counter1reset= true;
}