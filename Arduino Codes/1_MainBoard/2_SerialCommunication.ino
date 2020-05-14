void SendCommand(String _command) {
  int _commandLength = _command.length() + 1;
  char _commandToSend[_commandLength];
  _command.toCharArray(_commandToSend, _commandLength);
  Serial.write(_commandToSend);
  Serial.flush();

  bool _sentStatus = false;
  while(_sentStatus == false) {
    if (Serial.available() > 0 ) {
      String _receivedCommand = Serial.readStringUntil(';');
      if (_receivedCommand == "Ok") {
        _sentStatus = true;
      }
    }
  }
}

String ReceiveCommand() {
  bool _receiveStatus = false;
  while(_receiveStatus == false) {
    if (Serial.available() > 0 ) {
      String _receivedCommand = Serial.readStringUntil(';');
      Serial.write("Ok;");
      Serial.flush();
      _receiveStatus = true;
      return _receivedCommand;
    }
  }
}

void CommandLibrary() {

}