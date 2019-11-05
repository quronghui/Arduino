// 项目：实现电磁阀的整体控制；

#include <Arduino.h>
#include <Stepper.h>


//  这里设置步进电机旋转一圈是多少步
const int stepsPerRevolution = 100;
// 初始化点击使用的Arduino的引脚，分别接四个IO；需要将10和9对调一下
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);    

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);             // 要设置串口工作
  pinMode(13, OUTPUT);    // （1）LED工作指示灯
  pinMode(2, OUTPUT);     //  （2）用作电磁阀的控制信号，低电平有效
  pinMode(3, OUTPUT);     //  （3）用作气泵的控制信号，低电平有效

  //（4） 设置电机转速，单位r/min  
  myStepper.setSpeed(90);
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.print("Turning ON!");
    digitalWrite(13, HIGH);  
    digitalWrite(2, LOW);       //  电磁阀的控制信号
    digitalWrite(3, LOW);     // 气泵的控制信号
  
  // 电机：顺时针旋转一周
    myStepper.step(2048);     // 4  步模式下旋转一周用2048 步。
    delay(1000);
    digitalWrite(13, LOW);
  // 电机的反转：需要将10和9对调一下，才能控制
    myStepper.step(-1024);
    delay(1000);
    Serial.print("Turning OFF!");
}