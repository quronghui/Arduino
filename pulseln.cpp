/*
* Pulseln 用于检测脉冲,高低转换的时间长短
* unsigned long pulseIn (uint8_t pin, uint8_t state, unsigned long timeout) 
*/

#include <Arduino.h>

int pin = 7;
unsigned long duration;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  // digitalWrite(pin, HIGH); 
  // delay(1000);
  // digitalWrite(pin, LOW);   
  
  duration = pulseIn(pin, HIGH);
  Serial.print("duration = ");
  Serial.println(duration);
}