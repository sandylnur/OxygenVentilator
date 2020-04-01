/* 
    Arduino Loop Function
    #run main code repeatedly
*/

void loop() {
    // put your main code here, to run repeated
    // emergency functions
        emerBtnInput();
        emergencyAction();

    // check if button is pressed and excute action from function
        checkBtnInput();   
   
}