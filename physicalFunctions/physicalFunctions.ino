void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  
}

/****** Physical Model Functions ******/

/* Flow calculation: Bernoulli's eqn and Flow = Area*Velocity */
double flowCalc(double p1, p2){

  double flowRate = 0.0;      // [m^3/s]
  
  // Venturi Geometry
  static double d1 = 0.01905; // [m]
  static double d2 = 0.001;   // [m]

  // air density
  static double rho = 1.225;  // [kg/m^3]

  // Catch error if p2 > p1 
  if (p1 > p2) {
    flowRate = sqrt( (2.0/rho)*(p1-p2)/( (16.0/(M_PI*M_PI))*(1.0/pow(d2,4.0) - 1.0/pow(d1,4.0))));
  }

  return flowRate;
}
