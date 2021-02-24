# include <SPI.h>

const int inletPin = 10;
const int p1_pin   = 9;

#define SPI_CLOCK_SPEED 800000 // hz

// Constants for sensor transfer function
#define OUTPUT_MIN  0x19999A
#define OUTPUT_MAX  0xE66666
#define P_MAX       25.0 //PSI
#define P_MIN       0.0 // PSI

void setup() {

  // Setup serial
  Serial.begin(57600);

  // Set slave pins as OUTPUT (when slave is HIGH it ignores master, when it is LOW it communicates)
  pinMode(inletPin, OUTPUT);
  pinMode(p1_pin, OUTPUT);

  // De-assert slave pins
  digitalWrite(inletPin, HIGH);
  digitalWrite(p1_pin, HIGH);

  delay(1);

  // Initialize SPI
  SPI.begin();

}

void loop() {


  // Get pressure readings
  double p_inlet = read_pressure(inletPin);
  double p1      = read_pressure(p1_pin);

  // print preasure readings
  print_pressure_reading(p_inlet);
  print_pressure_reading(p1);

 
}

/* Function to initiate any SPI sequence to a given slave device
 * Clock speed, MSB, SPI_MODE all given in sensor datasheet.
 */
void initiate_SPI_transfer(int slave_pin)
{
  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0));
  digitalWrite(slave_pin, LOW);
}

/* Handles reading pressure count from given sensor
 * Uses SPI based on datasheet commands
 * Sensor sends back 24-bit pressure count
 * Use transfer function A from datasheet to conver to PSI 
 */
double read_pressure(int slave_pin)
{

  // Start SPI transaction with pin
  initiate_SPI_transfer(slave_pin);

  // Step 1 From datasheet pp 18
  SPI.transfer(0xAA);
  SPI.transfer(0x00);
  SPI.transfer(0x00);

  // End SPI sequence
  digitalWrite(slave_pin, HIGH);
  SPI.endTransaction();

  // Step 2
  delay(5);

  // Step 3
  initiate_SPI_transfer(slave_pin);
  

  uint8_t stat     = SPI.transfer(0xF0);
  Serial.print("status = ");
  Serial.println(stat);

  // Get pressure count reading [3x8bits = 24 bit reading]
  uint32_t reading = 0;
  for(uint8_t i = 0; i<3; i++)
  {
    reading |= SPI.transfer(0x00);
    if (i != 2) reading = reading << 8;
    Serial.println(reading);
  }

  // De-select pin and end transaction
  digitalWrite(slave_pin, HIGH);
  SPI.endTransaction();

  // Wait to clear bus before reading next sensor
  delay(10);

   // convert to pressure with transfer function
  return ((reading - OUTPUT_MIN)*(P_MAX - P_MIN))/(OUTPUT_MAX - OUTPUT_MIN) + P_MIN;
   
}

/* Prints pressure reading to serial in a nice way */
void print_pressure_reading(double pressure)
{

  Serial.print("pressure is ");
  Serial.print(pressure);
  Serial.println("PSI");
}
