# include <SPI.h>

const int inletPin = 10;
const int p1_pin   = 9;
const int p2_pin   = 2;
const int valvePin = 5;

#define SPI_CLOCK_SPEED 800000 // hz

// Constants for sensor transfer function
#define OUTPUT_MIN  0x19999A
#define OUTPUT_MAX  0xE66666
#define P_MAX       25.0 //PSI
#define P_MIN       0.0 // PSI

void setup() {

  // Setup serial
  Serial.begin(57600);

  // Setup control pin for valve
  pinMode(valvePin, OUTPUT);

  // Set slaveSelectPin as OUTPUT (when slave is HIGH it ignores master, when it is LOW it communicates
  pinMode(inletPin, OUTPUT);
  pinMode(p1_pin, OUTPUT);
  pinMode(p2_pin, OUTPUT);

  // De-assert slave pins
  digitalWrite(inletPin, HIGH);
  digitalWrite(p1_pin, HIGH);
  digitalWrite(p2_pin, HIGH);

  // reset sensors
  reset_sensors();
  
  delay(1);

  // Initialize SPI
  SPI.begin();

}

void loop() {

  // Get inlet pressure reading
  double p_inlet = read_pressure(inletPin);
  double p1      = read_pressure(p1_pin);
  double p2      = read_pressure(p2_pin);

  // Print pressure readings
  print_pressure_reading(p_inlet);
  print_pressure_reading(p1);
  print_pressure_reading(p2);

  // Open and close the valve
  digitalWrite(valvePin, HIGH);
  Serial.println("opened valve");
  delay(2000);
  digitalWrite(valvePin, LOW);
  Serial.println("closed valve");
  delay(2000);

 
}

/* Call before any spi transfer sequence */
void initiate_SPI_transfer(int slave_pin)
{
  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0));
  digitalWrite(slave_pin, LOW);
}

/* Handles SPI transfer between pressure sensor at the slave_pin and master. */
double read_pressure(int slave_pin)
{

  // Step 1 and 2: Check pressure sensor status
  uint8_t stat = connect_to_sensor(slave_pin);

  // Step 3: Get pressure reading [24 bit count] from sensor
  uint32_t reading = 0;
  for(uint8_t i = 0; i<3; i++)
  {
    reading |= SPI.transfer(0x00);
    if (i != 2) reading = reading << 8;
  }

  // De-select pin and end transaction
  digitalWrite(slave_pin, HIGH);
  SPI.endTransaction();

  // Wait to clear bus before reading next sensor
  delay(5);

   // convert to pressure with transfer function
  return ((reading - OUTPUT_MIN)*(P_MAX - P_MIN))/(OUTPUT_MAX - OUTPUT_MIN) + P_MIN;
   
}

uint8_t connect_to_sensor(int slave_pin)
{
  // Start SPI transaction with pin
  initiate_SPI_transfer(slave_pin);

  // Step 1 - Datasheet SPI commands pp 18
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
  uint8_t stat = SPI.transfer(0xF0);
  Serial.print("status = ");
  Serial.println(stat);

  return stat;
}

void print_pressure_reading(double pressure)
{

  Serial.print("pressure is ");
  Serial.print(pressure);
  Serial.println("PSI");
}

void reset_sensors(void)
{
  // Assert then de-assert pins to clear out any junk data
  digitalWrite(inletPin, LOW);
  digitalWrite(inletPin, HIGH);
  
  delay(5);
  
  digitalWrite(p1_pin, LOW);
  digitalWrite(p1_pin, HIGH);

  delay(5);
  
  digitalWrite(p2_pin, LOW);
  digitalWrite(p2_pin, HIGH);
}
