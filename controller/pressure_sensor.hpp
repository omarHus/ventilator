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
    double transferFunction(uint32_t raw_value);

    // Chip select pin
    int _pin;

    // Calibration parameters, based on product code (datasheet figure 4)
    const double _max_psi = 25.0;
    const double _min_psi = 0.0;
    const double _max_cnt = 15099494.4;
    const double _min_cnt = 1677721.6;
};

#endif // PRESSURE_SENSOR_H
