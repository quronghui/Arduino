// 项目：实现整体采集装置的控制

#include <Arduino.h>
#include "bmp180.h"

//  5v控制
#define Buzzer  2                                             // 提示模式二的开始和结束
#define  Air_pump_second 4                        // 气泵2 -- 数字引脚5
#define Air_pump_third 5                      // 气泵3 -- 数字引脚6
#define Air_pump_third_two  6         // 气泵3 排气的控制：通过arduino UNO 给一个高电平的控制信号

// 12v控制
#define  Direct_connection_second 7         //  直通阀2 -- 数字引脚 4
#define Direct_connection_first 8                    // 直通阀1 -- 数字引脚 8
#define Air_pump_first 9           // 直通阀1 -- 数字引脚 9

// 工作模式设置
#define Mode_first  10          // 工作模式一： 排气模式
#define Mode_second 11  // 工作模式二： 储气模式～人体呼出气体
#define Mode_third  12    // 工作模式三： 储气模式～易挥发性气体
 
// Gas Sensors
int Gas[15];
int sendTime = 0;

 // bmp180气压传感器的限制
 struct delay_time
 {
   int  Ventilation_delay = 5 ;   // 通气延时时间
   float high_pressure = 103500;    //  储气接近上限时的bmp180的值
   float low_pressure = 101000;

 }Time ;
 
 // 函数声明
float Test_Pressure();


/***********************Init ******************************/
// （1）设置arduino引脚的Output、input:
void InitPin()
{ 
  pinMode(Direct_connection_first, OUTPUT); 
  pinMode(Direct_connection_second, OUTPUT);
  pinMode(Air_pump_first, OUTPUT);
  pinMode(Air_pump_second, OUTPUT);
  pinMode(Air_pump_third, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  // Air_pump_third_two input
  pinMode(Air_pump_third_two, INPUT);

  //Mode  Input
  pinMode(Mode_first, INPUT);
  pinMode(Mode_second, INPUT);
  pinMode(Mode_third, INPUT);
  
}

//   （2）气泵和三通阀，一开始全为高电平才是暂停；
void StopAll()
{
  digitalWrite(Direct_connection_first, HIGH);
  digitalWrite(Direct_connection_second, HIGH);
  digitalWrite(Air_pump_first, HIGH);
  digitalWrite(Air_pump_second, HIGH);
  digitalWrite(Air_pump_third, HIGH);
  digitalWrite(Buzzer, LOW);
}

/*******************************工作模式设置******************************/
// 工作模式一 排气模式： (1)直通1开,气1开,气2开,气3开,通气延时; (2)直1关, 气1关,气2开,气3开,储气袋排气;
void  Action_first()
{
  // （1）整体装置排气体
  digitalWrite(Direct_connection_first, HIGH);
  digitalWrite(Direct_connection_second, LOW);
  digitalWrite(Air_pump_first, LOW);     
  digitalWrite(Air_pump_second, LOW);
  digitalWrite(Air_pump_third, LOW);
  delay(1000 * Time.Ventilation_delay) ;       // 通气延时时间

  // （2）储气袋排气
    digitalWrite(Direct_connection_first, HIGH);
    digitalWrite(Air_pump_first, HIGH);     

  digitalWrite(Air_pump_third, HIGH);
  float pressure = Test_Pressure() ;
  while ( pressure > Time.low_pressure )
  {
    pressure = Test_Pressure();
    // Serial.print("Pressure: ");
    // Serial.print(pressure, 0); //whole number only.
    // Serial.println(" Pa");
      delay(100);
  }
  digitalWrite(Air_pump_second, HIGH);    // 关闭第二个气泵
  delay(100);
  digitalWrite(Air_pump_third, LOW);        // 打开第三个气泵

  // （3）传感器阵列排气：  Arduino UNO会给mega一个控制信号，停止抽气
  while (digitalRead(Air_pump_third_two) == 0)  
  {
    // Serial.print("Air_pump_third_two: ");
    digitalWrite(Air_pump_third, LOW);
    delay(100);
  }

  StopAll();    // 工作模式1停止
  delay(500);
}

//  工作模式二 储气模式——人体呼出气体采集：  直1开，气1开，气2关，气3关;
void Action_second()
{
  digitalWrite(Direct_connection_first, LOW);
  digitalWrite(Direct_connection_second, HIGH);
  digitalWrite(Air_pump_first, HIGH);
  digitalWrite(Air_pump_second, HIGH);
  digitalWrite(Air_pump_third, HIGH);
  // delay(500 * Time.Ventilation_delay);
  
  float pressure = Test_Pressure() ;
  while ( pressure < Time.high_pressure )
  {
    pressure = Test_Pressure();
    // Serial.print("Pressure: ");
    // Serial.print(pressure, 0); //whole number only.
    // Serial.println(" Pa");
      delay(100);
  } 
  StopAll();    // 停止储气；
  delay(500);
}

//  工作模式二 储气模式——挥发性气体采集

void Action_second_vol()
{
  digitalWrite(Direct_connection_first, HIGH);
  digitalWrite(Direct_connection_second, HIGH);
  digitalWrite(Air_pump_first, LOW);
  digitalWrite(Air_pump_second, HIGH);
  digitalWrite(Air_pump_third, HIGH);
  // delay(500 * Time.Ventilation_delay);
  
  float pressure = Test_Pressure() ;
  while ( pressure < Time.high_pressure )
  {
    pressure = Test_Pressure();
    // Serial.print("Pressure: ");
    // Serial.print(pressure, 0); //whole number only.
    // Serial.println(" Pa");
      delay(100);
  } 
  StopAll();    // 停止储气；
  delay(500);
}


// 工作模式三 检测模式：  直1关，气1关，气2开，气3关;
void Action_third()
{
  digitalWrite(Direct_connection_first, HIGH);
  digitalWrite(Direct_connection_second, LOW);
  digitalWrite(Air_pump_first, HIGH);
  digitalWrite(Air_pump_second, LOW);
  digitalWrite(Air_pump_third, HIGH);


  // 放在Gas_Detection（）函数里面了，现在模式三的控制必须是由Gas_Detection关闭
  // float pressure = Test_Pressure() ;
  // while ( pressure > Time.low_pressure )
  // {
  //   pressure = Test_Pressure();
  //   // Serial.print("Pressure: ");
  //   // Serial.print(pressure, 0); //whole number only.
  //   // Serial.println(" Pa");
  //   delay(100);
  // }
  // StopAll();  // 停止检测

  delay(500); 
}

//  工作模式四：抽气模式
void Action_four()
{
  // digitalWrite(Direct_connection_first, HIGH);
  // digitalWrite(Direct_connection_second, HIGH);
  // digitalWrite(Air_pump_first, HIGH);
  // digitalWrite(Air_pump_second, HIGH);
  digitalWrite(Air_pump_third, LOW);

  // 传感器阵列排气： 当传感器阵列气体低于LOW_pressure,  Arduino UNO会给mega一个HIGH高的控制信号;
  // while (digitalRead(Air_pump_third_two) == 0)  
  // {
  //   // Serial.print("Air_pump_third_two: ");
  //   digitalWrite(Air_pump_third, LOW);
  //   delay(100);
  // }
  
  delay(500);

}



/************ ****工作模式的判断：*********************/
void Judge_mode()
{
  // 工作模式一: 排气模式
  int i = 0;
  if(digitalRead(Mode_first) == 0 ){
    for ( ;  i < 2; i++)
    {
      Action_first();
    }
    for(int j =0 ; j < 3 ; j++){
      digitalWrite(Buzzer, HIGH);
      delay(100);
       digitalWrite(Buzzer, LOW);
      delay(100);
   }
    
  }
  
  // 工作模式二: 储气模式~通过人体呼出气体
  if(digitalRead(Mode_second) == 0){
    // 加了一个蜂鸣器进行提示；
      digitalWrite(Buzzer, HIGH);
      delay(500);
      digitalWrite(Buzzer, LOW);
      
      Action_second();

      digitalWrite(Buzzer, HIGH);
      delay(500);
      digitalWrite(Buzzer, LOW);
    }
      
  // 工作模式二: 储气模式~通过吸气采集易挥发气体
  if(digitalRead(Mode_third) == 0)
    Action_second_vol();

  delay(100);
}

/*****************传感器检测，电脑端口读取******************/
void Gas_Detection()
{
  int readNum = 0;
  int not_readValue = 15;   // 设置预先采集的次数
  int model_one_time = 0;   // 自动进行换气的判断

  if (Serial.available()){
    readNum = Serial.read();
    if(readNum != '\r'){
      sendTime = sendTime * 10 + (readNum - 48);
    }
    else{     
      for(int j = 0; j < sendTime; j ++){
        // 设置预先采集的次数
       if(j == not_readValue)
          Action_third();   //  检测模式，将气体通入到传感器阵列中
        // 设置采集气体抽出的时间点
        if(j == sendTime - 50)
          Action_four();
        for(int i = 0; i < 15; i ++){
          Gas[i] = analogRead(i);
        }

        // 控制气泵二吸气装置停止
        float pressure = Test_Pressure() ;
        if ( pressure < Time.low_pressure )
            digitalWrite(Air_pump_second, HIGH);
        // 控制抽气装置停止
        if(digitalRead(Air_pump_third_two) == 1)
          digitalWrite(Air_pump_third, HIGH);

        //float smoke_float = wet_int / 204.6;
        Serial.print("GAS_A0=");Serial.print(Gas[0]);Serial.print(" ");
        Serial.print("GAS_A1=");Serial.print(Gas[1]);Serial.print(" ");
        Serial.print("GAS_A2=");Serial.print(Gas[2]);Serial.print(" ");
        Serial.print("GAS_A3=");Serial.print(Gas[3]);Serial.print(" ");
        Serial.print("GAS_A4=");Serial.print(Gas[4]);Serial.print(" ");
        Serial.print("GAS_A5=");Serial.print(Gas[5]);Serial.print(" ");
        Serial.print("GAS_A6=");Serial.print(Gas[6]);Serial.print(" ");
        Serial.print("GAS_A7=");Serial.print(Gas[7]);Serial.print(" ");
        Serial.print("GAS_A8=");Serial.print(Gas[8]);Serial.print(" ");
        Serial.print("GAS_A9=");Serial.print(Gas[9]);Serial.print(" ");
        Serial.print("GAS_A10=");Serial.print(Gas[10]);Serial.print(" ");
        Serial.print("GAS_A11=");Serial.print(Gas[11]);Serial.print(" ");
        Serial.print("GAS_A12=");Serial.print(Gas[12]);Serial.print(" ");
        Serial.print("GAS_A13=");Serial.print(Gas[13]);Serial.print(" ");
        Serial.print("GAS_A14=");Serial.print(Gas[14]);Serial.print(" \r\n");
        delay(100);
      }
      sendTime = 0;  
      model_one_time++;  
      delay(1000);
      if(sendTime == 0 && model_one_time > 0){

      // 第二次开始后，自动进行换气
        Action_first();
        for(int j =0 ; j < 3 ; j++){
          digitalWrite(Buzzer, HIGH);
          delay(100);
          digitalWrite(Buzzer, LOW);
          delay(100);
        }
      }
    }

    // 控制气泵二吸气装置停止
    float pressure = Test_Pressure() ;
    if ( pressure < Time.low_pressure )
      digitalWrite(Air_pump_second, HIGH);


  }
  // 控制抽气装置停止
    if(digitalRead(Air_pump_third_two) == 1)
      digitalWrite(Air_pump_third, HIGH);
}

/*************bmp180气压传感器测量************/ 
float Test_Pressure()
{
 float temperature = bmp085GetTemperature(bmp085ReadUT()); //MUST be called first
  float pressure = bmp085GetPressure(bmp085ReadUP());
  float atm = pressure / 101325; // "standard atmosphere"
  float altitude = calcAltitude(pressure); //Uncompensated caculation - in Meters 

  // Serial.print("Temperature: ");
  // Serial.print(temperature, 2); //display 2 decimal places
  // Serial.println("deg C");

  // Serial.print("Pressure: ");
  // Serial.print(pressure);
  // // Serial.print(pressure, 0); //whole number only.
  // Serial.println(" Pa");

  // Serial.print("Standard Atmosphere: ");
  // Serial.println(atm, 4); //display 4 decimal places

  // Serial.print("Altitude: ");
  // Serial.print(altitude, 2); //display 2 decimal places
  // Serial.println(" M");

  // Serial.println();//line break

  delay(100); //wait a second and get values again.

  return pressure;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);      // 要设置串口工作；
  InitPin();                        // 初始化引脚的工作模式；
  StopAll();                      // 初始化为高；
  
  // bmp 180气压模块
  Wire.begin();
  bmp085Calibration();    // bmp180的初始化;  
  
}

void loop() {
  Judge_mode();       // 三个工作模式的判断
  Gas_Detection();  // 传感器采集数据
}

