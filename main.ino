//LIBRARIES
#include <HIH61xx.h>
#include <AsyncDelay.h>
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

//CONSTANTS
#define TSLPIN A1
 
//VARIABLES
int chk;
int sensorValue = 0;
int temp = 0;
int amb = 0;
int accelx = 0;
int accely = 0;
int accelz = 0;
bool printed = true;
int toleranceMode = 0;

//INTIALISATION
HIH61xx<TwoWire> hih(Wire);
Adafruit_MMA8451 mma = Adafruit_MMA8451();
AsyncDelay samplingInterval;
void powerUpErrorHandler(HIH61xx<TwoWire>& hih)
{
  Serial.println("Error powering up HIH61xx device");
}
void readErrorHandler(HIH61xx<TwoWire>& hih)
{
  Serial.println("Error reading from HIH61xx device");
}

void setup() { 
  //Starts Serial Output
  Serial.begin(9600);
  
  //HIH6120 setup
  Wire.begin();
  hih.setPowerUpErrorHandler(powerUpErrorHandler);
  hih.setReadErrorHandler(readErrorHandler);
  hih.initialise();
  samplingInterval.start(1000, AsyncDelay::MILLIS); 
  
  //MMA8451 setup
  Serial.println("Adafruit MMA8451 test!");
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!"); 
  mma.setRange(MMA8451_RANGE_2_G);
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");
  Serial.println("");
 
  //LED pin Setup
  pinMode(11, OUTPUT);  
  pinMode(12, OUTPUT);  
  pinMode(13, OUTPUT);
}
//TSL25 Function
void TSL257(){
  sensorValue = analogRead(TSLPIN);
  Serial.println("SENSOR TSL257");
  Serial.println(sensorValue);
  Serial.println();
}
//MMA8451 Function
void MMA8451(){
  Serial.println("SENSOR MMA8451");
  mma.begin();
  mma.read(); 
  sensors_event_t event; 
  mma.getEvent(&event);
  
  Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("m/s^2 ");
  accelx = event.acceleration.x;
  accely = event.acceleration.y;
  accelz = event.acceleration.z;  
  
  //Prints the orientation of the 
  uint8_t ori = mma.getOrientation();  
  switch (ori) {
    case MMA8451_PL_PUF: 
      Serial.println("Portrait Up Front");
      break;
    case MMA8451_PL_PUB: 
      Serial.println("Portrait Up Back");
      break;    
    case MMA8451_PL_PDF: 
      Serial.println("Portrait Down Front");
      break;
    case MMA8451_PL_PDB: 
      Serial.println("Portrait Down Back");
      break;
    case MMA8451_PL_LRF: 
      Serial.println("Landscape Right Front");
      break;
    case MMA8451_PL_LRB: 
      Serial.println("Landscape Right Back");
      break;
    case MMA8451_PL_LLF: 
      Serial.println("Landscape Left Front");
      break;
    case MMA8451_PL_LLB: 
      Serial.println("Landscape Left Back");
      break;
    }
  Serial.println();
  return;  
}
//HIH6120 Function
void HIH6120(){
  
  while(1){
  if (samplingInterval.isExpired() && !hih.isSampling()) {
    hih.start(); 
    printed = false;
    samplingInterval.repeat();
    Serial.println("Sampling started:");
  } 
  hih.process();
  
  if (hih.isFinished() && !printed) {
    printed = true;
    Serial.println("SENSOR HIH6120");
    Serial.print("RH: ");
    Serial.print(hih.getRelHumidity() / 100.0);
    amb = hih.getRelHumidity() / 100.0; //Stores ambient humidity
    Serial.println(" %");
    Serial.print("Ambient: ");
    Serial.print(hih.getAmbientTemp() / 100.0);
    temp = (hih.getAmbientTemp())/100;//Stores ambient temperature
    Serial.println(" deg C");
    Serial.println();
    return;
  }
  }
}
//Tolerance Function
void ToleranceCheck(){
  if (temp < -5 || temp > 100){ //Tolerance check for temperature
    toleranceMode = 1;
  }
  else if (amb < 10 || amb > 60){ //Tolerance check for moisture
    toleranceMode = 2;
  }
  else if (accelx < -15 || accelx > 15 || accely < -15 || accely > 15 || accelz < -15 || accelz > 15){ //Tolerance check for accelration
    toleranceMode = 3;
  }
  else if (sensorValue < 500){ //Tolerance check for light level
    toleranceMode = 4;
  }
  else{ //Default State
    toleranceMode = 0;
  }
  return;
}
//MAIN CODE
void loop(void) {
  TSL257();    //Calls all the separate functions
  MMA8451();
  HIH6120();
  ToleranceCheck();
  
  //LED Lighting according to there corresponding tolerance mode
  if (toleranceMode == 0) {
    delay(1000);
    digitalWrite(11, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
  }
  else if (toleranceMode == 1) {
    delay(1000);
    digitalWrite(13, HIGH);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
  }
  else if (toleranceMode == 2) {
    delay(1000);
    digitalWrite(13, !digitalRead(13));
    digitalWrite(12, HIGH);
    digitalWrite(11, LOW);
  }
  else if (toleranceMode == 3) {
    delay(1000);
    digitalWrite(12, !digitalRead(12));
    digitalWrite(13, HIGH);
    digitalWrite(11, LOW);
  }
  else if (toleranceMode == 4) {
    delay(1000);
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
  }
}

//Made by Noah Johnson @University of Bath 5th December 2022
