
/*
1. Title :     Led_gradual_change
2. Keyword :   Arduino; LED; Analog
3. Funcation1 ：实现LED灯的渐变；
4. Note : 项目还未完成，没有调通，需要产生模拟信号的东西，是否可以通过示波器
*/


#include <Arduino.h>

int ledPin = 13;
int analogPin = 3;
int val = 0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
   int x = 1;
   for (int i = 0; i > -1; i = i + x){

      analogWrite(analogPin, i);
      analogWrite(ledPin, analogPin / 4); 

      //digitalWrite(ledPin, i);
      if (i == 255) x = -1;             // 在峰值切换方向
      delay(10);
   } 
}
/*
* Led gradual change
*
* By simple for recycling adn analog output pin
* 1. Nead analog pin 
* 估计得是模拟电压的输出，才会出现LED的渐变

void loop()
{
   int x = 1;
   for (int i = 0; i > -1; i = i + x){
      analogWrite(PWMpin, i);
      if (i == 255) x = -1;             // 在峰值切换方向
      delay(10);
   } 
}
*/