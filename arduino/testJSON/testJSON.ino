#include <ArduinoJson.h>

void setup() {
  // Setup serial
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // Test serial
  Serial.println("Hello World!");

  // Create JSON data
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["pressure"] = 20;
  jsonDoc["volume"] = 5;
  jsonDoc["action"] = "read";

  // Send JSON
  serializeJson(jsonDoc, Serial);
  Serial.println();

  // Receive JSON
  String jsonRaw = Serial.readString();
  DeserializationError error = deserializeJson(jsonDoc, jsonRaw);
  if (error) {
    Serial.println("JSON parsing failed!");
    return;
  }

  int pressure = jsonDoc["pressure"];
  int volume = jsonDoc["volume"];
  const char *action = jsonDoc["action"];
  
  // Test that we got what we expected
  char buf[128];

  sprintf(buf, "%d", pressure);
  Serial.println(buf);

  sprintf(buf, "%d", volume);
  Serial.println(buf);
  
  sprintf(buf, "%s", action);
  Serial.println(buf);
}

void loop() {
  // Unused
}
