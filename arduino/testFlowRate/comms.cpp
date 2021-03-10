/*
 * This file handles all serial communication
 *
 * Even though this is a cpp file, the interface is "C-style" (not object
 * oriented) since there is very little dynamic behavior
 */

#include "config.h"
#include "comms.hpp"
#include <ArduinoJson.h>

//static char tempBuf[256];
//static char floatBuf[64];
static char tempBuf[130];
static char floatBuf[64];
static StaticJsonDocument<300> jsonDoc;

// Initialize serial port
void comms_init(void)
{
    // Init serial port
    Serial.begin(9600);
    Serial1.begin(9600);

    // Wait for serial port to initialize
    while (!Serial);
}

// Convert msg to JSON and send to desktop app over serial
void comms_send(message_t* msg)
{
    jsonDoc["volume_p1"] = msg->volume_p1;
    jsonDoc["pressure_p1"] = msg->pressure_p1;
    jsonDoc["flow_p1"] = msg->flow_p1;
    jsonDoc["volume_p2"] = msg->volume_p2;
    jsonDoc["pressure_p2"] = msg->pressure_p2;
    jsonDoc["flow_p2"] = msg->flow_p2;

    // Send these to keep the JSON packet consistent, but we don't change values
    jsonDoc["setpoint_p1"] = msg->setpoint_p1;
    jsonDoc["setpoint_p2"] = msg->setpoint_p2;

    // debug stringify json
    serializeJson(jsonDoc, Serial1);

    // TODO: Necessary?
    Serial1.write("\n");
    Serial.println();
}

// Read from desktop app
void comms_receive(message_t* msg)
{
    String jsonRaw = Serial1.readString();
    if (jsonRaw.length() > 0) {
        deserializeJson(jsonDoc, jsonRaw);

        /* Only get setpoint fields */
        msg->setpoint_p1= jsonDoc["setpoint_p1"];
        msg->setpoint_p2= jsonDoc["setpoint_p2"];
    }
}

// ------------------ DEBUGGING FUNCTIONS ------------------

// These functions are only relevant if NO_DESKTOP_APP_ATTACHED == 1

void debug_msg(const char* str)
{
#if NO_DESKTOP_APP_ATTACHED == 1U
    Serial.println(str);
#endif
}

void debug_int(const char* str, int x)
{
#if NO_DESKTOP_APP_ATTACHED == 1U
    sprintf(tempBuf, str, x);
    Serial.println(tempBuf);
#endif
}

void debug_double(const char* str, double x)
{
    const byte precision = 4U;
#if NO_DESKTOP_APP_ATTACHED == 1U
    dtostrf(x, precision + 3U, precision, floatBuf);
    sprintf(tempBuf, str, floatBuf);
    Serial.println(tempBuf);
#endif
}
