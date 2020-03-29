/*
 * Main loop of ventilator controller
 */

#include <SPI.h>
#include <math.h>
#include "config.h"
#include "comms.hpp"
#include "pressure_sensor.hpp"

// Loop time
static unsigned long previousTime;

// PID state variables
static double sumError, lastError;

// Message globals
static message_t msg;

// Pressure sensor object globals
static PressureSensor* patient1_sensor1;
static PressureSensor* patient1_sensor2;
static PressureSensor* patient2_sensor1;
static PressureSensor* patient2_sensor2;

// Private functions
void computePID();

void setup()
{
  // Setup SPI, since this bus is common to all sensors it is done here
  SPI.begin();

  // Setup pressure sensor driver
  patient1_sensor1 = new PressureSensor(SPI_CS_PATIENT_1_SENSOR_1);
  patient1_sensor2 = new PressureSensor(SPI_CS_PATIENT_1_SENSOR_2);
  patient2_sensor1 = new PressureSensor(SPI_CS_PATIENT_2_SENSOR_1);
  patient2_sensor2 = new PressureSensor(SPI_CS_PATIENT_2_SENSOR_2);

  // Setup serial
  comms_init();

  // For test/debug
  randomSeed(analogRead(0));

  // Init previous time
  previousTime = micros();
}

void loop()
{
  // Ensure polling at desired frequency
  unsigned long currentTime = micros();
  double elapsedTime = currentTime - previousTime;
  if (elapsedTime >= POLLING_FREQ) {
      debug_msg("Begin controller main code");

      // Receive data from the desktop app
      comms_receive(&msg);

      // Read input from sensors
      double pressure_p11 = patient1_sensor1->getPressure(); // CLOSEST Patient 1
      double pressure_p12 = patient1_sensor2->getPressure();
      double pressure_p21 = patient2_sensor2->getPressure(); // CLOSEST Patient 2
      double pressure_p22 = patient2_sensor2->getPressure();

      debug_double("\tPatient 1 Pressure Sensor 1 = %s", pressure_p11);
      debug_double("\tPatient 1 Pressure Sensor 2 = %s", pressure_p12);
      debug_double("\tPatient 2 Pressure Sensor 1 = %s", pressure_p21);
      debug_double("\tPatient 2 Pressure Sensor 2 = %s", pressure_p22);

      // Pass the closest pressure readings to the display
      msg.pressure_p1 = pressure_p11;
      msg.pressure_p2 = pressure_p21;

      debug_double("\tPatient 1 Average Pressure = %s", msg.pressure_p1);
      debug_double("\tPatient 2 Average Pressure = %s", msg.pressure_p2);

      // Convert pressure readings to flow
      msg.flow_p1 = flowMeasurement(pressure_p11, pressure_p12);
      msg.flow_p2 = flowMeasurement(pressure_p11, pressure_p12);

      debug_double("\tPatient 1 Flow = %s", msg.flow_p1);
      debug_double("\tPatient 2 Flow = %s", msg.flow_p2);

      // TODO: How do we compute volume for the desktop app?
      msg.volume_p1 = 0.0;
      msg.volume_p2 = 0.0;

      debug_double("\tPatient 1 Volume = %s", msg.volume_p1);
      debug_double("\tPatient 2 Volume = %s", msg.volume_p2);

      // Compute PID output
      double flow_adjustment_p1 = computePID(msg.flow_p1, msg.setpoint_p1, elapsedTime);
      double flow_adjustment_p2 = computePID(msg.flow_p2, msg.setpoint_p2, elapsedTime);

      // TODO: Map PID output from mL/min to Amps

      // TODO: Send signal to valves

      // Send data to desktop app
      comms_send(&msg);

      previousTime = currentTime;
  }
}

// ------------------ PHYSICAL MODEL + CONTROL SYSTEM ------------------

/* 1. Flow measurement from pressure sensors:
 * Bernoulli's eqn and Flow = Area*Velocity (uniform). input p1, p2 [MPa], output [mL/min]
 * Must convert Psi from sensor to MPa
 * Must include math.h library
*/
double flowMeasurement(double p1, double p2){

  double flowRate = 0.0;

  // Venturi Geometry
  const double d1 = 11.05;                 // [mm]
  const double d2 = 5.80;                  // [mm]
  const double A1 = (M_PI/4.0)*square(d1); // [mm^2]
  const double A2 = (M_PI/4.0)*square(d2); // [mm^2]

  // air density
  const double rho = 1.225;                // [kg/m^3]

  // Catch error if p2 > p1
  if (p1 > p2) {
    flowRate = 60.0*sqrt( (2.0/rho)*(p1-p2)/( (1.0/square(A2))-(1.0/square(A1)) ) ); // [mL/min]
  }

  return flowRate;
}

double computePID(double input, double setpoint, double delta_t)
{
  // Calculate error variables
  double error     = setpoint - input;
  double rateError = (error - lastError) / delta_t;

  // Save globals
  sumError         = sumError + (error * delta_t); // TODO: This will overflow eventually...windowed sum?
  lastError        = error;

  // Compute PID output
  return PID_GAIN_P*error + PID_GAIN_I*sumError + PID_GAIN_D*rateError;
}
