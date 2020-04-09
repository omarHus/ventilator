/*
 * Device driver for the MPRLS0025PA00001A from Honeywell
 */

#include <SPI.h>
#include <stdint.h>
#include "config.h"
#include "comms.hpp"
#include "pressure_sensor.hpp"

#define COMMAND_MEASURE  (0xAA)
#define COMMAND_DATA     (0x00)

// Constructor
PressureSensor::PressureSensor(int pin) : _pin(pin)
{
    // Set chip select GPIO as an output
    pinMode(_pin, OUTPUT);

    // Deselect device
    digitalWrite(_pin, HIGH);
}

// Get pressure from the sensor (datasheet section 7)
double PressureSensor::getPressure()
{     
    // Select device
    digitalWrite(_pin, LOW);

    // Step 1
    uint8_t stat = SPI.transfer(COMMAND_MEASURE);
//    SPI.transfer(COMMAND_DATA);
//    SPI.transfer(COMMAND_DATA);
//
//    // Step 2
//    delay(5);
//    SPI.transfer(0xF0);
//
//    
//    // Step 3. Read the 8-bit status and 24-bit pressure output
//    stat = SPI.transfer(0xF0);
    uint8_t pressure_b2 = SPI.transfer(COMMAND_DATA);
    uint8_t pressure_b1 = SPI.transfer(COMMAND_DATA);
    uint8_t pressure_b0 = SPI.transfer(COMMAND_DATA);
   
    // Deselect device
    digitalWrite(_pin, HIGH);

    // Delay between sensor reads to avoid cross talk on the wires
    delay(100U);

    // Combine data bytes
#if NO_ELECTRONICS_ATTACHED == 0
    uint32_t b2 = (uint32_t) pressure_b2 << 16;
    uint32_t b1 = (uint32_t) pressure_b1 << 8;
    uint32_t b0 = (uint32_t) pressure_b0;

    uint32_t raw_value = 0; //clear it out
    raw_value          = b2 | b1 | b0; // combine
#else
    // If we have no electronics attached, readings will just be garbage so randomly generate one
    uint32_t raw_value = random(_min_cnt, _max_cnt);
#endif

    // Debug
    debug_msg("\tSPI transaction complete!");
    debug_int("\t\tStatus Byte = %02X", stat);
    debug_int("\t\tData Byte 2 = %02X", pressure_b2);
    debug_int("\t\tData Byte 1 = %02X", pressure_b1);
    debug_int("\t\tData Byte 0 = %02X", pressure_b0);
    Serial.println(raw_value); // prints count in decimal form

    // Convert to PSI
    return transferFunction(raw_value);
}

// Convert from raw 24-bit value from sensor to the PSI reading (datasheet section 8)
double PressureSensor::transferFunction(uint32_t raw_value)
{
    return (((raw_value - _min_cnt) * (_max_psi - _min_psi)) / (_max_cnt - _min_cnt)) + _min_psi;
}
