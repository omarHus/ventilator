/*
 * Header file for pressure_sensor.cpp
 */

#include <stdint.h>

 #ifndef PRESSURE_SENSOR_H
 #define PRESSURE_SENSOR_H

class PressureSensor {
public:
    PressureSensor(int pin);
    double getPressure();

private:
    void initiate_SPI_transfer(int pin);
    uint8_t connect_to_sensor(int pin);
    double transferFunction(uint32_t raw_value);

    // Chip select pin
    int _pin;

    // Calibration parameters, based on product code (datasheet figure 4)
    const double _max_psi = 25.0;
    const double _min_psi = 0.0;
    const uint32_t _max_cnt = 15099494;
    const uint32_t _min_cnt = 1677722;
    const int SPI_CLOCK_SPEED = 800000;
};

#endif // PRESSURE_SENSOR_H
