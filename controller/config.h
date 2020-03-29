/*
 * This file defines top-level configuration macros
 */

#ifndef CONFIG_H
#define CONFIG_H

// Set to 1 if no electronics are attached, and we are just faking sensor data
#define NO_ELECTRONICS_ATTACHED (1U)

// Set to 1 if no desktop app is attached, and we want to just debug from serial port monitor
#define NO_DESKTOP_APP_ATTACHED (1U)

// Setup the superloop frequency, in microseconds
#define POLLING_FREQ (1000000U)

// Setup PID gain
#define PID_GAIN_P (1.0)
#define PID_GAIN_I (0.2)
#define PID_GAIN_D (0.1)

// Setup the pins used by the different pressure sensors
#define SPI_CS_PATIENT_1_SENSOR_1 (10U)
#define SPI_CS_PATIENT_1_SENSOR_2 (11U)
#define SPI_CS_PATIENT_2_SENSOR_1 (12U)
#define SPI_CS_PATIENT_2_SENSOR_2 (13U)

#endif // CONFIG_H
