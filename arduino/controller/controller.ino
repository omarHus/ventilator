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

// This is 255 / 50,000, see convertToDutyCycle for details
#define DUTY_CYCLE_CONV_SLOPE (0.0051)

// Loop time
static unsigned long previousTime;

// PID state variables
static double sumError, lastError;

// Valve control globals
static int duty_cycle_p1;
static int duty_cycle_p2;

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

    // Make the setpoint [mL/min] something normal by default (90 L/min)
    msg.setpoint_p1 = 90000.0;
    msg.setpoint_p2 = 90000.0;

    // Make the duty cycle something normal by default
    duty_cycle_p1 = 127;
    duty_cycle_p2 = 127;
  
    // Init previous time
    previousTime = micros();
}

void loop()
{
    // Ensure polling at desired frequency
    unsigned long currentTime = micros();
    double elapsedTime = currentTime - previousTime;
    if (elapsedTime >= POLLING_FREQ) {
        debug_double("Begin controller main code, elapsedTime = %s us", elapsedTime);

        // Convert elapsedTime from microseconds to seconds for remaining calculations
        elapsedTime = elapsedTime / 1000000.0;
  
        // Receive data from the desktop app
        comms_receive(&msg);
  
        // Read pressure [PSI] from sensors
        double pressure_p11 = patient1_sensor1->getPressure(); // CLOSEST Patient 1
        double pressure_p12 = patient1_sensor2->getPressure();
        double pressure_p21 = patient2_sensor2->getPressure(); // CLOSEST Patient 2
        double pressure_p22 = patient2_sensor2->getPressure();
        debug_double("\tPatient 1 Pressure Sensor 1 = %s PSI", pressure_p11);
        debug_double("\tPatient 1 Pressure Sensor 2 = %s PSI", pressure_p12);
        debug_double("\tPatient 2 Pressure Sensor 1 = %s PSI", pressure_p21);
        debug_double("\tPatient 2 Pressure Sensor 2 = %s PSI", pressure_p22);
  
        // Pass the upstream pressure [cmH2O] readings to the display
        msg.pressure_p1 = PSI_TO_CMH2O * pressure_p11;
        msg.pressure_p2 = PSI_TO_CMH2O * pressure_p21;
        debug_double("\tPatient 1 Pressure = %s cmH2O", msg.pressure_p1);
        debug_double("\tPatient 2 Pressure = %s cmH2O", msg.pressure_p2);
  
        // Pressure upstream must be higher than pressure downstream (inspiratory)
        if ((pressure_p11 > pressure_p12) && (pressure_p21 > pressure_p22)) {
      
            // Convert pressure readings [MPa] to flow [mL/min]
            msg.flow_p1 = flowMeasurement(PSI_TO_MPA * pressure_p11, PSI_TO_MPA * pressure_p12);
            msg.flow_p2 = flowMeasurement(PSI_TO_MPA * pressure_p21, PSI_TO_MPA * pressure_p22);
            debug_double("\tPatient 1 Flow = %s mL/min", msg.flow_p1);
            debug_double("\tPatient 2 Flow = %s mL/min", msg.flow_p2);
      
            // Approximate volume [mL] as the integral of the flow
            msg.volume_p1 += msg.flow_p1 * elapsedTime;
            msg.volume_p2 += msg.flow_p2 * elapsedTime;
            debug_double("\tPatient 1 Volume = %s mL", msg.volume_p1);
            debug_double("\tPatient 2 Volume = %s mL", msg.volume_p2);
      
            // Control the flow [mL/min] using a PID controller
            double elapsedTimeMin = elapsedTime / 60;
            double flow_adjustment_p1 = computePID(msg.flow_p1, msg.setpoint_p1, elapsedTimeMin);
            double flow_adjustment_p2 = computePID(msg.flow_p2, msg.setpoint_p2, elapsedTimeMin);
            debug_double("\tPatient 1 PID Output = %s mL/min", flow_adjustment_p1);
            debug_double("\tPatient 2 PID Output = %s mL/min", flow_adjustment_p2);
      
            // Map PID output from mL/min to duty cycle
            duty_cycle_p1 = convertToDutyCycle(flow_adjustment_p1, duty_cycle_p1);
            duty_cycle_p2 = convertToDutyCycle(flow_adjustment_p2, duty_cycle_p2);

            // Stay within range - note that these next two lines is typically not allowed for safety-critical code...
            duty_cycle_p1 = (duty_cycle_p1 > 255) ? 255 : (duty_cycle_p1 < 0) ? 0 : duty_cycle_p1;
            duty_cycle_p2 = (duty_cycle_p2 > 255) ? 255 : (duty_cycle_p2 < 0) ? 0 : duty_cycle_p2;            

            // Send signal to valves
            analogWrite(PWM_PATIENT_1_VALVE, duty_cycle_p1);
            analogWrite(PWM_PATIENT_2_VALVE, duty_cycle_p2);
            debug_double("\tPatient 1 Duty Cycle = %s percent", 100.0 * (duty_cycle_p1 / 255.0));
            debug_double("\tPatient 2 Duty Cycle = %s percent", 100.0 * (duty_cycle_p2 / 255.0));            
      
            // Send data to desktop app
            comms_send(&msg);
        } else {
            debug_msg("\tError! Pressure upstream must be greater than downstream. Skipping calculations...");
        }
        
        previousTime = currentTime;
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
double flowMeasurement(double p1, double p2)
{
    // Venturi geometry
    const double d1 = 11.05;                 // [mm]
    const double d2 = 5.80;                  // [mm]
    const double A1 = (M_PI/4.0)*square(d1); // [mm^2]
    const double A2 = (M_PI/4.0)*square(d2); // [mm^2]
    
    // Air density
    const double rho = 1.225;                // [kg/m^3]
    
    return 60.0*sqrt((2.0/rho)*(p1 - p2) / ((1.0/square(A2)) - (1.0/square(A1)))); // [mL/min]
}

/*
 * PID controller to track input to setpoint. Uses two global variables for integral and 
 * derivative portions:
 *  - sumError
 *  - lastError
 */
double computePID(double input, double setpoint, double delta_t)
{
    // Calculate error variables
    double error     = input - setpoint;
    double rateError = (error - lastError) / delta_t;

    // Update globals
    sumError  = sumError + (error * delta_t);
    lastError = error;
    
    // Compute PID output
    return PID_GAIN_P*error + PID_GAIN_I*sumError + PID_GAIN_D*rateError;
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
  
    // First deal with the edge cases, if the flow adjustment is massive in either direction, we can only turn the PWM signal all on or off
    if (abs(pid_output) >= 50,000.0) {
        // The largest change we can do
        delta = 255;
    } else if (abs(pid_output) <= 0) {
        // The smallest change we can do
        delta = 25;
    } else {
        // Adjust the duty cycle accordingly
        delta = abs(((int) (DUTY_CYCLE_CONV_SLOPE * pid_output)));
    }

    if (pid_output < 0) {
        return curr_duty_cycle + delta;
    } else {
        return curr_duty_cycle - delta;
    }
}
