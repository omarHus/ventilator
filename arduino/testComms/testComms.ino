void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.println("Hello World!\r\n");

  randomSeed(analogRead(0));
}

void loop()
{  
  String s = Serial.readString();
  if (s.length() > 0)
    Serial.println("Input Command From User:\t" + s);

  const byte RAW_DATA_SIZE = 4;
  int rawData[RAW_DATA_SIZE];
  for (int i = 0; i < RAW_DATA_SIZE; i++)
    rawData[i] = random(0, 1000);

  char displayData[256];
  sprintf(displayData, "Pressure Sensor Readings: %d\t%d\t%d\t%d\r\n", rawData[0], rawData[1], rawData[2], rawData[3]);
  Serial.println(displayData);

  delay(1000);
}
