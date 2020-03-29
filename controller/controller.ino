#include <ArduinoJson.h>

/* PID Gain Values */
double Kp = 1.0;
double Ki = 0.2;
double Kd = 0.1;

/* Other PID variables */
unsigned long currentTime, previousTime;
double elapsedTime;
double error, lastError;
double input, output, setpoint;
double sumError, rateError;
double pollFrequency;
void computePID();

void setup() {

  // Setup serial
  Serial.begin(9600);

  // PID setup
  setpoint = 500.0;
  previousTime  = micros();
  pollFrequency = 100.0; //microseconds per reading
  lastError     = 0.0;

  while (!Serial) {
    ;
  }
}

/* JSON format "p1": ["20"], "v1":[500], "flow1":[8], "p2": ["20"], "v2":[500], "flow2":[8], "setpt1":[10] "setpt2":[10]*/

void loop() {

  currentTime = micros();
  elapsedTime = currentTime - previousTime;

  // Ensure polling at desired frequency
  if (elapsedTime >= pollFrequency) {

  // TODO: Read input from sensors
  // Convert pressure readings to flow

  // TODO: Compute PID output
  output = computePID(input);
  
  // TODO: Map PID output from mL/min to Amps
  

  // TODO: Send signal to valves
  
  }
}

double computePID(double input) {

  // Calculate error variables
  error     = setpoint - input;
  sumError += (error*elapsedTime);
  rateError = (error - lastError)/elapsedTime;

  //Compute PID output
  output       = Kp*error + Ki*sumError + Kd*rateError;
  lastError    = error;
  previousTime = currentTime;

  return output;
}
  
