// see: https://github.com/adafruit/Adafruit_LIS3DH
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <i2c_t3.h>
// see: http://playground.arduino.cc/Main/runningAverage
#include "RunningAverage.h"

#define LED_PIN    13
#define SWITCH_PIN 23
#define CLICKTHRESHHOLD 80

#define DELTA      400

boolean blinkOn = false;
uint32_t blinkDelta = 0;
uint32_t blinkInterval = 800; 
uint32_t blinkNow;

uint8_t  btnState = LOW;

RunningAverage RA_x(100);
RunningAverage RA_y(100);
RunningAverage RA_z(100);

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup() {
  RA_x.clear(); RA_y.clear(); RA_z.clear();

  Serial.begin(115200);
  pinMode(SWITCH_PIN, INPUT); 
  pinMode(LED_PIN, OUTPUT); 

  Serial.println("start");
  if (! lis.begin(0x18)) {
    Serial.println("Couldnt start");
    while (1) {Serial.println("stall");};
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  lis.setClick(2, CLICKTHRESHHOLD);
}

// this method of blinking does not slow down the loop with delay()

void loop() {

  lis.read();      // get X Y and Z data at once

  // print out the raw data
  // comment these out if you want to speed up loop
  Serial.print("X:  "); Serial.print(lis.x); 
  Serial.print("  \tY:  "); Serial.print(lis.y); 
  Serial.print("  \tZ:  "); Serial.println(lis.z); 

  // acceleration values are contain in lis.x_g, lis.y_g, lis.z_g...
  // Serial.print("X:  "); Serial.print(lis.x_g); 
  // Serial.print("Y:  "); Serial.print(lis.y_g); 
  // Serial.print("Z:  "); Serial.println(lis.z_g); 

  // gathering a running average is really useful
  // add values to the array with
  RA_x.addValue(lis.x);
  RA_y.addValue(lis.y);
  RA_z.addValue(lis.z);

  btnState = digitalRead(SWITCH_PIN);
  if (btnState == HIGH) {
    // do calibration here
    Serial.println("switch ON");
  } 

  blinkInterval = 800; 
  if (abs(RA_x.getAverage()) < DELTA && abs(RA_y.getAverage()) < DELTA ) {
    blinkInterval = 100; 
  }

  blinkNow = millis();
  if ((blinkNow - blinkDelta) > blinkInterval) {
    blinkOn = !blinkOn;
    digitalWrite(LED_PIN, blinkOn);
    blinkDelta = blinkNow;
  }
}
