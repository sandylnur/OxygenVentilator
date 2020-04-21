// Version 0.3
// Latest Date Modified: 11/04/2020
// defining all the inputs
const int Emergencybutton = 2;
const int ventEnabled = A1;
const int limitSwitch = A3;

//defining all the outputs
int LED = 11; //internal LED of Lilpad
int relay1 = 9;
int buzzer = 10;

//defining all the variables
long unsigned int timeAtrest;
long unsigned int timeMoving;
long unsigned int counterStart;
boolean counterreset;

//Setup
void setup()
{
    // setup for Emergency button, 3rd parameter to be used as LOW, CHANGE, FALLING, RISING
    pinMode(Emergencybutton, INPUT);
    attachInterrupt(digitalPinToInterrupt(Emergencybutton), emergencyISR, RISING);

    // defining the inputs and outputs
    pinMode(ventEnabled, INPUT);
    pinMode(limitSwitch, INPUT);

    pinMode(buzzer, OUTPUT);
    pinMode(relay1, OUTPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(relay1, HIGH);
    digitalWrite(LED, HIGH);
}

//Loop
void loop()
{
    //Start counter and signal reset while the ventilator is running
    if (ventEnabled == 0)
    {
        counterStart = millis();
        counterreset = true;
        //While the limitSwitch is Open, piston is moving -- Measure movement time and set counter
        while (limitSwitch == 0)
        {
            timeMoving = millis() - counterStart;
            counterreset = false;
            //If movement >= 7 sec activate emergency circuit
            if (timeMoving >= 7000)
            {
                digitalWrite(buzzer, HIGH);
                while (timeMoving >= 7000)
                {
                    digitalWrite(LED, !digitalRead(LED));
                    delay(1000);
                }
            }
            //Perform periodic check of vent status and reset if vent disabled
            if (ventEnabled == 1)
            {
                break;
            }
        }

        //While limitSwitch is Closed and counter hasn't been used already, piston is at rest --> Measure time at rest
        while (limitSwitch == 1 && counterreset == true)
        {
            timeAtrest = millis() - counterStart;
            //If time at rest >= 2s, activate emergency circuit
            if (timeAtrest >= 2000)
            {
                digitalWrite(buzzer, HIGH);
                while (timeAtrest >= 2000)
                {
                    digitalWrite(LED, !digitalRead(LED));
                    delay(2000);
                }
            }
            //Perform periodic check of vent status and reset if if vent disabled
            if (ventEnabled == 1)
            {
                digitalWrite(LED, LOW);
                break;
            }
        }
    }
}

//Emergency Interrupt Service Routine should turn off the arduino cct using Relay 1 and activate emergency circuit
void emergencyISR()
{
    digitalWrite(relay1, LOW);
    digitalWrite(buzzer, HIGH);
    While(Emergencybutton == 1)
    {
        digitalWrite(LED, !digitalRead(LED));
        delay(250);
    }
}
