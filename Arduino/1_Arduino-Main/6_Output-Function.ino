// output for emergency buzzer
int piezoPin = 3;

void buzzer(){
    tone(piezoPin, 1000, 500);
}