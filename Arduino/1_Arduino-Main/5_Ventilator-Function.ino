// boolean to know the status of ventilator at anymoment in time
bool ventilatorStatus = false;

bool checkVentilator(){
    return ventilatorStatus;
}


void startVentilator(){
    Serial.println("Ventilator Active");  // post on serial if ventilator started
    EEPROM.write(0, 1); // emergency backup
    buzzer();
    // write code to start ventilator





    ventilatorStatus = true;
}

void stopVentilator(){
    Serial.println("Ventilator Innactive");  // post on serial if ventilator stopped
    EEPROM.write(0, 0); // emergency backup
    buzzer();
    // write code to stop ventilator





    ventilatorStatus = false;
}