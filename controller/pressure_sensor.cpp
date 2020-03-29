/*
 * Device driver for the MPRLS0025PA00001A from Honeywell
 */

#include <SPI.h>
#include <stdint.h>
#include "config.h"
#include "comms.hpp"
#include "pressure_sensor.hpp"

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

    // Step 1: Enter measurement mode
    SPI.transfer(0xAAU);
    SPI.transfer(0x00U);
    SPI.transfer(0x00U);

    // Step 2 - Option 2: Wait 5 ms for conversion to happen
    delay(5U);

    // Step 3: Read the 24-bit pressure output
    uint8_t status      = SPI.transfer(0xF0U);
    uint32_t pressure_b2 = SPI.transfer(0x00U);
    uint32_t pressure_b1 = SPI.transfer(0x00U);
    uint32_t pressure_b0 = SPI.transfer(0x00U);

    // Deselect device
    digitalWrite(_pin, HIGH);

    // Combine data bytes
#if NO_ELECTRONICS_ATTACHED == 0
    uint32_t raw_value = (pressure_b2 << 16U) | (pressure_b1 << 8U) | pressure_b0;
#else
    // If we have no electronics attached, readings will just be garbage so randomly generate one
    uint32_t raw_value = random(0U, (1U << 24U) - 1U);
#endif

    // Debug
    debug_msg("\tSPI transaction complete!");
    debug_int("\t\tStatus = %08X", status);
    debug_int("\t\tSensorData = %08X", raw_value);

    // Convert to PSI
    return transferFunction(raw_value);
}

// Convert from raw 24-bit value from sensor to the PSI reading (datasheet section 8)
double PressureSensor::transferFunction(uint32_t raw_value)
{
    return (((raw_value - _min_cnt) * (_max_psi - _min_psi)) / (_max_cnt - _min_cnt)) + _min_psi;
}
