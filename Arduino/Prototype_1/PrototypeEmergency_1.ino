// Version 0.2
// defining all the inputs
const int Emergencybutton = 2;
const int ventEnabled = A1;
const int limitSwitch = A3;

//defining all the outputs
int LED = 8;
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
}

//Loop

void loop()
{
    if (ventEnabled == 0)
    {                            //only execute while the ventilator is running
        counterStart = millis(); //Start counter
        digitalWrite(LED, HIGH);
        counterreset = true; //Counter has been reset

        while (limitSwitch == 0)
        {                                         //While the limitSwitch is Open, piston is moving
            timeMoving = millis() - counterStart; //Measure Moving Time and compare to 7000ms
            counterreset = false;                 //Counter has been used for one check
            if (timeMoving >= 7000)
            {
                digitalWrite(buzzer, HIGH); //If condition is satisfied, activate emergency circuit
            }
            if (ventEnabled == 1)
            { //Perform periodic check of vent status and reset if turned off
                break;
            }
        }

        while (limitSwitch == 1 && counterreset == true)
        {                                         //While the limitSwitch is Closed, piston is at rest
            timeAtrest = millis() - counterStart; //Measure time at rest and compare to 2000ms
            if (timeAtrest >= 2000)
            {
                digitalWrite(buzzer, HIGH); //If condition satisfied, activate emergency circuit
            }
            if (ventEnabled == 1)
            { //Perform periodic check of vent status and reset if turned off
                digitalWrite(LED, LOW);
                break;
            }
        }
    }
}

//Emergency Interrupt Service Routine should turn off the arduino cct using Relay 1
void emergencyISR()
{
    digitalWrite(relay1, LOW);
    digitalWrite(buzzer, HIGH);
}
