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
int accelx = 0;
int accely = 0;
int accelz = 0;
bool printed = true;

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
  #if F_CPU >= 12000000UL
    Serial.begin(9600);
  #else
    Serial.begin(9600);
  #endif
  
  //HIH6120
  Wire.begin();
  hih.setPowerUpErrorHandler(powerUpErrorHandler);
  hih.setReadErrorHandler(readErrorHandler);
  hih.initialise();
  samplingInterval.start(1000, AsyncDelay::MILLIS); 

  //MMA8451
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

//MMA8451 Function
void MMA8451(){
  mma.begin();
  mma.read();
  Serial.print("X:\t"); Serial.print(mma.x); 
  Serial.print("\tY:\t"); Serial.print(mma.y); 
  Serial.print("\tZ:\t"); Serial.print(mma.z); 
  Serial.println();
 
  sensors_event_t event; 
  mma.getEvent(&event);
  
  Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("m/s^2 ");
  accelx = event.acceleration.x;
  accely = event.acceleration.y;
  accelz = event.acceleration.z;
  
  uint8_t o = mma.getOrientation();
  
  switch (o) {
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
void HIH6120setup(){
  hih.initialise();
  return;
}

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
    Serial.println(" %");
    Serial.print("Ambient: ");
    Serial.print(hih.getAmbientTemp() / 100.0);
    temp = (hih.getAmbientTemp())/100;
    Serial.println(" deg C");
    Serial.print("Status: ");
    Serial.println(hih.getStatus());
    return;
  }
  }
}
//LED Function
void LED(){
   
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  
  if (temp < -5 || temp > 100){
    digitalWrite(13, HIGH);
  }
  else if (accelx < -15 || accelx > 15 || accely < -15 || accely > 15 || accelz < -15 || accelz > 15){
    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
  }
  else if (sensorValue < 800){
    digitalWrite(13, HIGH);
    digitalWrite(11, HIGH);
  }
  else{
    digitalWrite(11, HIGH);
  }
  return;
}

//MAIN CODE
void loop(void) {
  sensorValue = analogRead(TSLPIN);
  Serial.println("SENSOR TSL257");
  Serial.println(sensorValue);
  Serial.println("");
  Serial.println("SENSOR MMA8451");
  MMA8451();
  Serial.println("SENSOR HIH6120");
  HIH6120();
  Serial.println("");
  LED();
}

//Made by Noah Johnson @University of Bath 5th December 2022
