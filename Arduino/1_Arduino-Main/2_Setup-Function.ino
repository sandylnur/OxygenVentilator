void setup() {
/* 
    Arduino Setup Function
    #setup code run once only
*/

// put your setup code here, to run once:
Serial.begin(9600);
    // start serial
        Serial.begin(9600);
        while (!Serial) {
            ; // wait for serial port to connect. Needed for native USB
        }

    // setup EEPROM to remember state in case of powerloss
        verifyEmergency();

    // setup for push button
        setupCheckBtn();
    


}