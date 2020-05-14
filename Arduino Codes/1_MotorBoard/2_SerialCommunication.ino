void SendCommand(String _command) {
  int _commandLength = _command.length() + 1;
  char _commandToSend[_commandLength];
  _command.toCharArray(_commandToSend, _commandLength);

  // send the command
  Serial.write(_commandToSend);
  
  // wait for all data to be sent
  Serial.flush();

  // check if command was sent
  bool _sentStatus = false;
  while (_sentStatus == false) {
    if (Serial.available() > 0 ) {
      // read string if available until character ";"
      String _receivedCommand = Serial.readStringUntil(';');
      if (_receivedCommand == "Ok") {
        _sentStatus = true;
      }
    }
  }
}

String ReceiveCommand() {
// keep checking until a command is received
  bool _receiveStatus = false;
  while (_receiveStatus == false) {
    if (Serial.available() > 0 ) {
      // read string if available until character ";"
      String _receivedCommand = Serial.readStringUntil(';');
      Serial.write("Ok;");
      Serial.flush();
      _receiveStatus = true;
      return _receivedCommand;
    }
  }
}