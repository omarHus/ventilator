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
#define MAX_VALVE_CURRENT   (185) //mA
#define MIN_VALVE_CURRENT   (60) // mA

// Loop time
static unsigned long previousTime;

// PID state variables
static double sumError1, lastError1, sumError2, lastError2;

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
  
    // For test/debug
    randomSeed(analogRead(0));

    // Make the setpoint [mL/min] something normal by default (9L/min)
    msg.setpoint_p1 = 5000.0;
    msg.setpoint_p2 = 5000.0;

    // Make the duty cycle something normal by default
    duty_cycle_p1 = 255;
    duty_cycle_p2 = 255;
  
    // Init previous time
    previousTime = micros();

    // initialize PID Globals
    sumError1  = 0.0;
    lastError1 = 0.0;
    sumError1  = 0.0;
    lastError1 = 0.0;

    // Open valves to begin
    pinMode(PWM_PATIENT_1_VALVE, OUTPUT);
    pinMode(PWM_PATIENT_2_VALVE, OUTPUT);
    analogWrite(PWM_PATIENT_1_VALVE, duty_cycle_p1);
    analogWrite(PWM_PATIENT_2_VALVE, duty_cycle_p2);
    
}

void loop()
{
    // Ensure polling at desired frequency
    unsigned long currentTime = micros();
    double elapsedTime = currentTime - previousTime;
    if (elapsedTime >= POLLING_FREQ) {
//        debug_double("Begin controller main code, elapsedTime = %s us", elapsedTime);

        // Convert elapsedTime from microseconds to seconds for remaining calculations
        elapsedTime = elapsedTime / 1000000.0;
  
        // Receive data from the desktop app
        comms_receive(&msg);
  
        // Read pressure [PSI] from sensors
        double inlet_pressure = inlet_sensor->getPressure();
        double p1 = patient1_sensor->getPressure();
        double p2 = patient2_sensor->getPressure();

//        debug_double("\tInlet Pressure = %s PSI", inlet_pressure);
//        debug_double("\tPatient 1 Pressure Sensor = %s PSI", p1);
//        debug_double("\tPatient 2 Pressure Sensor = %s PSI", p2);
  
        // Pass the upstream pressure [cmH2O] readings to the display
        msg.pressure_p1 = PSI_TO_CMH2O * p1;
        msg.pressure_p2 = PSI_TO_CMH2O * p2;
//        debug_double("\tPatient 1 Pressure = %s cmH2O", msg.pressure_p1);
//        debug_double("\tPatient 2 Pressure = %s cmH2O", msg.pressure_p2);
  
        // Pressure upstream must be lower than pressure downstream (inspiratory)
        if ((p1 <= inlet_pressure) && (p2 <= inlet_pressure)) {
      
            // Convert pressure readings [MPa] to flow [mL/min]
            msg.flow_p1 = flowMeasurement(PSI_TO_MPA * inlet_pressure, PSI_TO_MPA * p1);
            msg.flow_p2 = flowMeasurement(PSI_TO_MPA * inlet_pressure, PSI_TO_MPA * p2);
//            debug_double("\tPatient 1 Flow = %s mL/min", msg.flow_p1);
//            debug_double("\tPatient 2 Flow = %s mL/min", msg.flow_p2);
            Serial.print(msg.flow_p1);
            Serial.print("\t");
            Serial.println(msg.flow_p2);
      
            // Approximate volume [mL] as the integral of the flow
            msg.volume_p1 += msg.flow_p1 * elapsedTime;
            msg.volume_p2 += msg.flow_p2 * elapsedTime;
//            debug_double("\tPatient 1 Volume = %s mL", msg.volume_p1);
//            debug_double("\tPatient 2 Volume = %s mL", msg.volume_p2);
      
            // Control the flow [mL/min] using a PID controller
            double elapsedTimeMin = elapsedTime / 60;
            double flow_adjustment_p1 = computePID(msg.flow_p1, msg.setpoint_p1, &sumError1, &lastError1, elapsedTimeMin);
            double flow_adjustment_p2 = computePID(msg.flow_p2, msg.setpoint_p2, &sumError2, &lastError2, elapsedTimeMin);
//            debug_double("\tPatient 1 PID Output = %s mL/min", flow_adjustment_p1);
//            debug_double("\tPatient 2 PID Output = %s mL/min", flow_adjustment_p2);
      
            // Map PID output from mL/min to duty cycle
            duty_cycle_p1 = convertToDutyCycle(flow_adjustment_p1, duty_cycle_p1);
            duty_cycle_p2 = convertToDutyCycle(flow_adjustment_p2, duty_cycle_p2);

            // Stay within range - note that these next two lines is typically not allowed for safety-critical code...
            duty_cycle_p1 = (duty_cycle_p1 > 255) ? 255 : (duty_cycle_p1 < 0) ? 0 : duty_cycle_p1;
            duty_cycle_p2 = (duty_cycle_p2 > 255) ? 255 : (duty_cycle_p2 < 0) ? 0 : duty_cycle_p2;            

            // Send signal to valves
            analogWrite(PWM_PATIENT_1_VALVE, duty_cycle_p1);
            analogWrite(PWM_PATIENT_2_VALVE, duty_cycle_p2);
//            debug_double("\tPatient 1 Duty Cycle = %s percent", 100.0 * (duty_cycle_p1 / 255.0));
//            debug_double("\tPatient 2 Duty Cycle = %s percent", 100.0 * (duty_cycle_p2 / 255.0));            
      
            // Send data to desktop app
            comms_send(&msg);

        } else {
//            debug_msg("\tError! Pressure upstream must be less than downstream. Skipping calculations...");
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
double flowMeasurement(double inlet_pressure, double p1)
{
    // Venturi geometry
    const double d1 = 23.0;                   // [mm]
    const double d2 = 4.0;                    // [mm]
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
        delta_current = map(abs(pid_output), 0, MAX_VALVE_FLOW_RATE, MIN_VALVE_CURRENT, MAX_VALVE_CURRENT);
        
        // map from current to duty cycle
        delta = map(delta_current, MIN_VALVE_CURRENT, MAX_VALVE_CURRENT, 0, 255);
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
