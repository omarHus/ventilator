/*
 * This file defines top-level configuration macros
 */

#ifndef CONFIG_H
#define CONFIG_H

// Set to 1 if no electronics are attached, and we are just faking sensor data
#define NO_ELECTRONICS_ATTACHED (0U)

// Set to 1 if no desktop app is attached, and we want to just debug from serial port monitor
#define NO_DESKTOP_APP_ATTACHED (1U)

// Setup the superloop frequency, in microseconds
#define POLLING_FREQ (100000U)

// Setup PID gain
#define PID_GAIN_P (12.0)
#define PID_GAIN_D (1.0)
#define PID_GAIN_I (0.0)


// Setup the digital pins used by the different pressure sensors
#define SPI_CS_INLET_SENSOR (10U)
#define SPI_CS_PATIENT_1    (9U)
#define SPI_CS_PATIENT_2    (2U)

// Setup the analog pins used by the different valves
#define PWM_PATIENT_1_VALVE (4U)
#define PWM_PATIENT_2_VALVE (5U)

#endif // CONFIG_H
