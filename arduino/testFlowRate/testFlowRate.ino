/*
 * Main loop of ventilator controller
 */

#include <SPI.h>
#include <math.h>
#include "config.h"
#include "comms.hpp"
#include "pressure_sensor.hpp"

// Scaling factors for unit conversions
#define PSI_TO_CMH2O (70.307)
#define PSI_TO_MPA (0.00689476)
#define M3_S_TO_ML_MIN (6000.0)

// This is 255 / MAX_VALVE_FLOW_RATE, see convertToDutyCycle for details
#define MAX_VALVE_FLOW_RATE (23500) // mL/min

// Valve control globals
static int duty_cycle_p1;
static int duty_cycle_p2;

// Message globals
static message_t msg;

// Pressure sensor object globals
static PressureSensor* inlet_sensor;
static PressureSensor* patient1_sensor;
static PressureSensor* patient2_sensor;

// Private functions
void computePID();

void setup()
{    
    // Setup SPI, since this bus is common to all sensors it is done here
    SPI.begin();
  
    // Setup pressure sensor driver
    inlet_sensor    = new PressureSensor(SPI_CS_INLET_SENSOR);
    patient1_sensor = new PressureSensor(SPI_CS_PATIENT_1);
    patient2_sensor = new PressureSensor(SPI_CS_PATIENT_2);
  
    // Setup serial
    comms_init();

    // Open valves to begin
    pinMode(PWM_PATIENT_1_VALVE, OUTPUT);
    pinMode(PWM_PATIENT_2_VALVE, OUTPUT);
    analogWrite(PWM_PATIENT_1_VALVE, 255);
    analogWrite(PWM_PATIENT_2_VALVE, 255);

    duty_cycle_p1 = 255;
    
}

void loop()
{
  
      // Receive data from the mobile app
      if (Serial1.available()){
        duty_cycle_p1 = Serial1.read();
      }


      // Read pressure [PSI] from sensors
      double inlet_pressure = inlet_sensor->getPressure();
      double p1 = patient1_sensor->getPressure();
      double p2 = patient2_sensor->getPressure();

      debug_double("\tInlet Pressure = %s PSI", inlet_pressure);
      debug_double("\tPatient 1 Pressure Sensor = %s PSI", p1);
      debug_double("\tPatient 2 Pressure Sensor = %s PSI", p2);


      // Pressure upstream must be lower than pressure downstream (inspiratory)
      if ((p1 <= inlet_pressure) && (p2 <= inlet_pressure)) {
    
          // Convert pressure readings [MPa] to flow [mL/min]
          msg.flow_p1 = flowMeasurement(PSI_TO_MPA * inlet_pressure, PSI_TO_MPA * p1);
          msg.flow_p2 = flowMeasurement(PSI_TO_MPA * inlet_pressure, PSI_TO_MPA * p2);
          debug_double("\tPatient 1 Flow = %s mL/min", msg.flow_p1);
          debug_double("\tPatient 2 Flow = %s mL/min", msg.flow_p2);
    

          // Send signal to valves
          analogWrite(PWM_PATIENT_1_VALVE, duty_cycle_p1);
          analogWrite(PWM_PATIENT_2_VALVE, duty_cycle_p1);
          debug_double("\tPatient 1 Duty Cycle = %s percent", 100.0 * (duty_cycle_p1 / 255.0));
          debug_double("\tPatient 2 Duty Cycle = %s percent", 100.0 * (duty_cycle_p1 / 255.0));            
    
          // Send data to desktop app
          comms_send(&msg);

        } else {
            debug_msg("\tError! Pressure upstream must be less than downstream. Skipping calculations...");
        }
        
}

// ------------------ PHYSICAL MODEL + CONTROL SYSTEM ------------------

/* Calculate flow  from pressure sensors using Bernoulli's equation:
 * Flow = Area*Velocity (uniform).
 * Input p1, p2 [MPa]
 * Output: [mL/min]
 *  - Must convert pressure values from PSI to MPa
 *  - Must include math.h library
 */
double flowMeasurement(double inlet_pressure, double p1)
{
    // Venturi geometry
    const double d1 = 11.0;                 // [mm]
    const double d2 = 5.00;                  // [mm]
    const double A_1 = (M_PI/4.0)*square(d1); // [mm^2]
    const double A_2 = (M_PI/4.0)*square(d2); // [mm^2]
    const double A_Ratio = A_2/A_1;
    
    // Air density
    const double rho = 1.225;                // [kg/m^3]

    // Bernoulli Pressure Equation
    double delta_P = 2.0*(inlet_pressure - p1)/rho;

    // Outlet speed
    double v_outlet = sqrt(delta_P/(1-square(A_Ratio)));

    // Flow rate
    return A_2*v_outlet*M3_S_TO_ML_MIN; // convert 1x10^-4 m^3/s to mL/min
}

/*
 * PID controller to track input to setpoint. Uses two global variables for integral and 
 * derivative portions:
 *  - sumError
 *  - lastError
 *  - Pass these values by reference so the function changes the value of the actual Globals
 */
double computePID(double input, double setpoint, double *sumError, double *lastError, double delta_t)
{
    // Calculate error variables
    double error     = input - setpoint;
    double rateError = (error - *lastError) / delta_t;

    // Update globals
    *sumError  = *sumError + (error * delta_t);
    *lastError = error;
    
    // Compute PID output
    return PID_GAIN_P*error + PID_GAIN_I*(*sumError) + PID_GAIN_D*rateError;
}

/*
 * Map the output of the PID controller to a PWM duty cycle, which is proportional to the amount of flow the valve will allow.
 * Input: pid_output [mL/min]
 * Output: Duty cycle [0 - 255], where 0 represents always off and 255 represents always on
 * 
 * How do we do this? 
 *  - The PID output represents how much we need to adjust our input variable (flow) in order to hit the set point
 *    If it is positive, it means we need to decrease the flow. If it is negative, we need to increase the flow.
 *  - The valve we are using roughly follows this relationship when relating flow to PWM duty cycle: 
 *    https://www.tlxtech.com/uploads/images/Flowrate-12Vdc.gif
 *  - We approximate this curve as linear
 *  - We know the current duty cycle, which is giving us the current flow.
 *  - We can convert the PID output from flow to "duty cycle units" (0 - 255) using a linear mapping:
 *    https://stackoverflow.com/a/5732390
 *  - Then we adjust our current duty cycle by that amount
 */
int convertToDutyCycle(double pid_output, int curr_duty_cycle)
{  
    int delta;
    int delta_current;
  
    // First deal with the edge cases, if the flow adjustment is massive in either direction, we can only turn the PWM signal all on or off
    if (abs(pid_output) >= MAX_VALVE_FLOW_RATE) {
        // largest change
        delta = 255;
    } 
    else {
        // Adjust the duty cycle accordingly
        
        // map from inverted pid_output (0, max flow rate) to current (60, 180 mA) based on valve data sheet
        delta_current = map(abs(pid_output), 0, MAX_VALVE_FLOW_RATE, 60, 180);
        
        // map from current to duty cycle
        delta = map(delta_current, 60, 180, 0, 255);
    }

    if (pid_output < 0) {
        return curr_duty_cycle + delta;
    } else {
        return curr_duty_cycle - delta;
    }
}

double square(double number)
{
  return number*number;
}
