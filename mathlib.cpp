/*
* Title : Mathlib
* 1. Test constrain() funcation.
*/

#include <Arduino.h>

void Con();
//int Constrain(int amt, int low, int high);      // 这个函数没有在主函数中，因此可以不声明：函数名不能和数学函数式同名
void Mapping();
long mapping(long x, long in_min, long in_max, long out_min, long out_max);
void Randomseed();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  /************************************************/
  // Funcation
  int selection = 3;
  switch(selection){
    case 1 :
      Con();
      break;
    case 2 :
      Mapping();
      break;
    case 3 :
      Randomseed();
      break;
    default : 
      break;
  }

}
/************************************************/
// 1. constrain funcation 
// Constrain Funcation: Using to limit value, belong to range.
void Con(){
  int value;
  value = constrain(random(0,300), 50, 200);
  Serial.print("value = ");
  Serial.println(value);
  delay(10);
} 
int Constrain(int amt, int low, int high){
  int sensval; 
  sensval = constrain(amt, low, high);
  return sensval;
}

/************************************************/
// 2. Map funcation 
// Map Funcation: 将 value 从0-1024 映射到 0-255. 以后就不用计算数学方程式了，直接使用map进行转换。
void Mapping(){

  long value;
  value = mapping(random(0, 1024), 0, 1024, 0, 255);
  Serial.print("value = ");
  Serial.println(value);
  delay(100);
}

long mapping(long x, long in_min, long in_max, long out_min, long out_max){

  long ratio;
  long map_value;
  ratio = (x - in_min) * (out_max - out_min) / (in_max - in_min) ;   // 通过计算value的值在0-1024的比例后，求出value 在0-255 站的权重值
  map_value = ratio + out_min;                                          // 权重加上初值
  return map_value;
  //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/************************************************/
// 3. Randomseed funcation 
// Randomseed Funcation: 生成随机数
/***********没有成功************/
// void Randomseed(){
//   randomSeed(unsigned int seed);
//   Serial.print("seed = ");
//   Serial.println(randomSeed());
//   delay(100);
// }
