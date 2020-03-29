#include <math.h> 

/* Function declarations */
double flowCalc(double p1, double p2);
double flowRate(double tidal_volume, double respiration_rate);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Serial comms established");
}

void loop() {
  // Testing values 
  double p1 = 101.350;
  double p2 = 100.000;

  double q = flowMeasurement(p1,p2);
  Serial.println(q);

  double Vt = 500.0;
  double rr = 8.0;

  double v_dot = flowRate(Vt,rr);
  Serial.println(v_dot);
}

/****** Physical Model Functions ******/

/* 1. Flow measurement from pressure sensors: 
 * Bernoulli's eqn and Flow = Area*Velocity (uniform). input p1, p2 [MPa], output [mL/min] 
 * Must convert Psi from sensor to MPa
 * Must include math.h library
*/
double flowMeasurement(double p1, double p2){

  double flowRate = 0.0;
  
  // Venturi Geometry
  static double d1 = 11.05;                 // [mm]
  static double d2 = 5.80;                  // [mm]
  static double A1 = (M_PI/4.0)*square(d1); // [mm^2]
  static double A2 = (M_PI/4.0)*square(d2); // [mm^2]

  // air density
  static double rho = 1.225;                // [kg/m^3]

  // Catch error if p2 > p1 
  if (p1 > p2) {
    flowRate = 60.0*sqrt( (2.0/rho)*(p1-p2)/( (1.0/square(A2))-(1.0/square(A1)) ) ); // [mL/min]
  }

  return flowRate;
}

/* 2. Converting Flow rate from tidal volume: 
 * Input: tidal volume [mL], respiration rate (RR) [breaths/min]
 * Output: flow rate [mL/min]
*/
double flowRate(double tidal_volume, double respiration_rate) {
  return tidal_volume*respiration_rate;
}
