/*
* Title: Interrupt Funcation
* 
*/

#include <Arduino.h>

void blink();

int pin = 13;
volatile int state = LOW;       // 中断函数中使用的变量需要定义为 volatile 类型.

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
  attachInterrupt(0, blink, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(pin, state);
}

void blink(){
  state = !state;
}