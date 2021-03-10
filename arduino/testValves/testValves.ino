/*
 * Main loop of ventilator controller
 */

#include <SPI.h>
#include <math.h>
#include "config.h"
#include "comms.hpp"
#include "pressure_sensor.hpp"


// Pressure sensor object globals
static PressureSensor* inlet_sensor;
static PressureSensor* patient1_sensor;
static PressureSensor* patient2_sensor;


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

    // Setup valve pins
    pinMode(PWM_PATIENT_1_VALVE, OUTPUT);
    pinMode(PWM_PATIENT_2_VALVE, OUTPUT);
    
}

void loop()
{

    // Start the testing sequence
    Serial.println("TEST HAS BEGUN. Start with both valves OPEN");
    Serial.println("*******************************************");

    // Open the valves
    analogWrite(PWM_PATIENT_1_VALVE, 255);
    analogWrite(PWM_PATIENT_2_VALVE, 255);
    delay(500);
    Serial.println("Valves opened");
    delay(5000);
    digitalWrite(PWM_PATIENT_1_VALVE, HIGH);
    digitalWrite(PWM_PATIENT_2_VALVE, HIGH);
    Serial.println("Valves DIGITAL opened");
    delay(5000);
    digitalWrite(PWM_PATIENT_1_VALVE, LOW);
    digitalWrite(PWM_PATIENT_2_VALVE, LOW);
    Serial.println("Valves DIGITAL CLOSED");
    
    // Read pressure [PSI] from sensors
    double inlet_pressure = inlet_sensor->getPressure();
    double p1 = patient1_sensor->getPressure();
    double p2 = patient2_sensor->getPressure();
    
    debug_double("\tInlet Pressure = %s PSI", inlet_pressure);
    debug_double("\tPatient 1 Pressure Sensor = %s PSI", p1);
    debug_double("\tPatient 2 Pressure Sensor = %s PSI", p2);

    // Wait 5s and try to close first valve only
    delay(10000);
    analogWrite(PWM_PATIENT_1_VALVE, 0);
    delay(500);
    Serial.println("First Valve CLOSED, 2nd Valve OPEN");

    // Read pressure [PSI] from sensors
    inlet_pressure = inlet_sensor->getPressure();
    p1 = patient1_sensor->getPressure();
    p2 = patient2_sensor->getPressure();
    
    debug_double("\tInlet Pressure = %s PSI", inlet_pressure);
    debug_double("\tPatient 1 Pressure Sensor = %s PSI", p1);
    debug_double("\tPatient 2 Pressure Sensor = %s PSI", p2);


    // Wait 5s and try to open first valve and close 2nd valve
    delay(10000);
    analogWrite(PWM_PATIENT_1_VALVE, 255);
    analogWrite(PWM_PATIENT_2_VALVE, 0);
    delay(500);
    Serial.println("First Valve OPEN, 2nd Valve CLOSED");

    // Read pressure [PSI] from sensors
    inlet_pressure = inlet_sensor->getPressure();
    p1 = patient1_sensor->getPressure();
    p2 = patient2_sensor->getPressure();
    
    debug_double("\tInlet Pressure = %s PSI", inlet_pressure);
    debug_double("\tPatient 1 Pressure Sensor = %s PSI", p1);
    debug_double("\tPatient 2 Pressure Sensor = %s PSI", p2);
    
    
    // Wait 5s and try 1st valve OPEN and  2nd valve 50%
    delay(10000);
    analogWrite(PWM_PATIENT_1_VALVE, 255);
    analogWrite(PWM_PATIENT_2_VALVE, 127);
    delay(500);
    Serial.println("First Valve OPEN, 2nd Valve HALF");

    // Read pressure [PSI] from sensors
    inlet_pressure = inlet_sensor->getPressure();
    p1 = patient1_sensor->getPressure();
    p2 = patient2_sensor->getPressure();
    
    debug_double("\tInlet Pressure = %s PSI", inlet_pressure);
    debug_double("\tPatient 1 Pressure Sensor = %s PSI", p1);
    debug_double("\tPatient 2 Pressure Sensor = %s PSI", p2);
 
}
