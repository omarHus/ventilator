const byte DATA_MAX_SIZE = 32;
char data[DATA_MAX_SIZE];   // an array to store the received data

void receiveData() {
  static char endMarker = '\n'; // message separator
  char receivedChar;     // read char from serial port
  int ndx = 0;          // current index of data buffer
  // clean data buffer
  memset(data, DATA_MAX_SIZE, sizeof(data));
  // read while we have data available and we are
  // still receiving the same message.
  while(Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == endMarker) {
      data[ndx] = '\0'; // end current message
      Serial.println(data);
      return;
    }
    // looks like a valid message char, so append it and
    // increment our index
    data[ndx] = receivedChar;
    ndx++;
    // if the message is larger than our max size then
    // stop receiving and clear the data buffer. this will
    // most likely cause the next part of the message
    // to be truncated as well, but hopefully when you
    // parse the message, you'll be able to tell that it's
    // not a valid message.
    if (ndx >= DATA_MAX_SIZE) {
      break;
    }
  }
  // no more available bytes to read from serial and we
  // did not receive the separato. it's an incomplete message!
//  Serial.println("error: incomplete message");
//  Serial.print(data);
  memset(data, DATA_MAX_SIZE, sizeof(data));
}

void setup() {
  // Start Serial communication
  Serial.println("hello from arduino!");
  Serial.begin(9600);
}

void loop() {

  char receivedChar = 'h';
  static char endMarker = '\n';
  int ndx = 0;
  while(Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == endMarker) {
      data[ndx] = '\0'; // end current message
      Serial.println(data);
      return;
    }
  }
  delay(1000);
}
