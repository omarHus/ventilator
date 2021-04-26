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

    // Reset Pin to get rid of junk readings
    digitalWrite(_pin, HIGH);
    delay(5);
    digitalWrite(_pin, LOW);
    delay(5);
    digitalWrite(_pin, HIGH);
    delay(5);
    
}

// Get pressure from the sensor (datasheet section 7)
double PressureSensor::getPressure()
{     
  
    // Step 1 and 2: Check pressure sensor status
    uint8_t stat = connect_to_sensor(_pin);

    // Step 3: If electronics are attached then read pressure count (3x8bit = 24bit pressure count)
#if NO_ELECTRONICS_ATTACHED == 0
    uint32_t raw_value = 0;
    for(uint8_t i = 0; i<3; i++)
    {
      raw_value |= SPI.transfer(0x00);
      if (i != 2) raw_value = raw_value << 8;
    }
#else
    // If we have no electronics attached, readings will just be garbage so randomly generate one
    uint32_t raw_value = random(_min_cnt, _max_cnt);
#endif

     // De-select pin and end SPI transaction
    digitalWrite(_pin, HIGH);
    SPI.endTransaction();

    // Debug
//    debug_msg("\tSPI transaction complete!");
//    debug_int("\t\tStatus Byte = %02X", stat);

    // Wait to clear bus before reading next sensor
    delay(5);

    // Convert to PSI using Transfer Function A from Datasheet
    return transferFunction(raw_value);
}

// Convert from raw 24-bit value from sensor to the PSI reading (datasheet section 8)
double PressureSensor::transferFunction(uint32_t raw_value)
{
    return (((raw_value - _min_cnt) * (_max_psi - _min_psi)) / (_max_cnt - _min_cnt)) + _min_psi;
}

/* Call before any spi transfer sequence to ensure master and slave are communicating at the right end of clock pulses */
void PressureSensor::initiate_SPI_transfer(int _pin)
{
  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0));
  digitalWrite(_pin, LOW);
}

/* Connect to pressure sensor based on commands from the datasheet section 7.
 * Get status byte from the sensor. 64 = Connected
 */
uint8_t PressureSensor::connect_to_sensor(int _pin)
{
  // Start SPI transaction with pin
  initiate_SPI_transfer(_pin);

  // Step 1 - Datasheet SPI commands pp 18 to ask sensor to send master the pressure reading
  SPI.transfer(0xAA);
  SPI.transfer(0x00);
  SPI.transfer(0x00);

  // End SPI sequence
  digitalWrite(_pin, HIGH);
  SPI.endTransaction();

  // Step 2: Delay to clear MISO line
  delay(5);

  // Step 3: Get status Byte
  initiate_SPI_transfer(_pin);
  uint8_t stat = SPI.transfer(0xF0);
//  Serial.print("status = ");
//  Serial.println(stat);

  return stat;
}
