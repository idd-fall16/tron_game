/* Controller 1 = accelerometer
 * Controller 2 = two force sensitive resistors (FSRs)
 */
 
// do not use the cloud functions - assume programming through Arduino IDE
#if defined(ARDUINO) 
SYSTEM_MODE(MANUAL); 
#endif
#include "Adafruit_LIS3DH.h"
#include "Adafruit_Sensor.h"

// macros for accelerometer thresholds
#define TILT_RIGHT_MIN 6000
#define TILT_LEFT_MAX -6000
#define TILT_BRAKE_MIN 7000
#define LEVEL_X_MIN -1000
#define LEVEL_X_MAX 1000
#define LEVEL_Y_MIN 900
#define LEVEL_Y_MAX 1600
// macros for softpot thresholds
#define SOFTPOT_MIDDLE 2800
#define SOFTPOT_NOT_PRESSED 10
// macros for FSR thresholds
#define FSR_PRESS 1000
#define FSR_STATIONARY 4000

// counters prevent controllers from doing continuous left/right input 
// (which would cause the player to crash)
int leftAccelCounter = 0;
int rightAccelCounter = 0;
int brakeAccelCounter = 0;
int fsrCounter = 0;
int softpotCounter = 0;

// First FSR (force sensitive resistor) is connected to analog A0
int fsrAnalogPin0 = A3; 
// the analog reading from the first FSR resistor divider
int fsrReading0; 
// Second FSR (force sensitive resistor) is connected to analog A1
int fsrAnalogPin1 = A2; 
// the analog reading from the second FSR resistor divider
int fsrReading1;      
// softpot connected to analog A4
int softpot = A4;
// analog reading from softpot
int softpotReading;

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup(void) {
  Serial.begin(9600);
  Serial.println("LIS3DH test!");
  
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  
  //Serial.print("Range = "); Serial.print(2 << lis.getRange());  
  //Serial.println("G");
}

void loop() {
  
  // Softpot section
  /*softpotReading = analogRead(softpot);
  if (softpotReading > SOFTPOT_MIDDLE && softpotCounter==0) { // left side pressed
    Serial.println(4);
    softpotCounter = 1;
  } else if (softpotReading > SOFTPOT_NOT_PRESSED && softpotReading <=SOFTPOT_MIDDLE && softpotCounter==0) { // right side pressed
    Serial.println(5);
    softpotCounter = 1;
  } else if (softpotReading <= SOFTPOT_NOT_PRESSED && softpotCounter==1){ // softpot not pressed
    softpotCounter = 0;
  }*/
  
  // FSR section
  fsrReading0 = analogRead(fsrAnalogPin0);
  fsrReading1 = analogRead(fsrAnalogPin1);
  // left sensor pressed, right sensor free, turn left
  if (fsrReading0 < FSR_PRESS && fsrReading1 > FSR_STATIONARY) {
    if (fsrCounter==0){
     Serial.println(4);
     fsrCounter=1; 
    }
  } else if (fsrReading1 < FSR_PRESS && fsrReading0 > FSR_STATIONARY) { // right sensor pressed, left sensor free, turn right
    if (fsrCounter==0){
     Serial.println(5);
     fsrCounter=1; 
    }
  } else if (fsrReading1 < FSR_PRESS && fsrReading0 < FSR_PRESS) { // both sensors pressed, brake
    if (fsrCounter==0){
     Serial.println(6);
     fsrCounter=1; 
    }
  } else if (fsrReading1 > FSR_STATIONARY && fsrReading0 > FSR_STATIONARY) { // both sensors free, reset counters
    if (fsrCounter==1) {
      fsrCounter = 0;
    }
  }

  // Accelerometer Section
  lis.read();      // get X Y and Z data at once
  
  // accelerometer tilting right - move player right
  if (lis.x >= TILT_RIGHT_MIN) {
    // ensures that you must go level again before it accepts more right input
    if (rightAccelCounter==0) { 
       Serial.println(2);
       rightAccelCounter = 1;
    }
  }
  // accelerometer tilting left - move player left
  else if (lis.x <= TILT_LEFT_MAX) {
    // ensures that you must go level again before it accepts more left input
    if (leftAccelCounter==0) { 
      Serial.println(1);
      leftAccelCounter = 1;
    }
  }
  // accelerometer level again
  else if (lis.x <= LEVEL_X_MAX && lis.x >= LEVEL_X_MIN && lis.y <= LEVEL_Y_MAX && lis.y >= LEVEL_Y_MIN) {
    // if the board was previously tilted left or right, reset counters so future tilts are recognized
    if (leftAccelCounter==1) {
      leftAccelCounter = 0;
    }
    if (rightAccelCounter==1) {
      rightAccelCounter = 0;
    }
    if (brakeAccelCounter==1) {
      brakeAccelCounter = 0;
    }
  } 
  // accelerometer tilting down towards user - brake (continuous)
  else if (lis.y >= TILT_BRAKE_MIN) { 
    if (brakeAccelCounter==0){
      Serial.println(3);
      brakeAccelCounter = 1;
    }
  }
}
