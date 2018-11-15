/*
*
* usigned long  is the test chars sizes.
*
* Notes: int（-2^15 -- 2^15-1） and usigned int(2^16-1) size
*/

#include <Arduino.h>

unsigned long time;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //

  Serial.print("Time = ");
  time = millis();          // Program is print time;
  Serial.println(time);     // including ：  Serial.print('\n');
  delay(100);
}