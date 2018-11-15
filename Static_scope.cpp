/*
* This team is static scope. 
* 让数的值在一个范围内变动
* RandomWalk
* Paul Badger 2007
* RandomWalk函数在两个终点间随机的上下移动
* 在一个循环中最大的移动由参数“stepsize”决定
* 一个静态变量向上和向下移动一个随机量
* 这种技术也被叫做“粉红噪声”或“醉步”
*/

#include <Arduino.h>

#define RandomWalkLowRange -20
#define RandomWalkHighRange 20

int stepsize;
int thisTime;
int total;


int randomWalk(int moveSize){

  static int place;
  Serial.print("moveSize = ");
  Serial.println(moveSize);

  place = place + (random(-moveSize, moveSize + 1 ));
  Serial.print("place = ");
  Serial.println(place);
  
  if(place < RandomWalkLowRange){
    place = place + (RandomWalkLowRange - place);     // 转换数字为正
  }

  else if(place > RandomWalkHighRange){
    place = place - (place - RandomWalkHighRange);
  }
  

  return place;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  stepsize = 5;
  thisTime = randomWalk(stepsize);
  Serial.println(thisTime);
  delay(100);
}
