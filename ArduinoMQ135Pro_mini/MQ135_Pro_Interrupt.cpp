/*
  调代码一步步的调
1. 全局变量/数组，可以实现loop函数和Interrupt函数值的传递；
2. 实现检测次数的累加，和每次检测采样的次数sampling；
3. 将需要的值写入EEPROM，进行存储； 
4. 中断0：检测key1，Key Down采样口气值，Key UP显示本次采样的最大值；
5. 中断1：检测key2,Key Down 显示第几次采样，最大浓度值，等级
6. 按键算法,还未成功移入?
*/

#include "Arduino.h"
#include <EEPROM.h>
#include "U8glib.h"

/***********************************************************/
// Interrupt0函数声明
void MemoryData();              // 采样数据的存储，存储到数据；
void blink();                   // 中断函数，检测到RISING，便进入中断函数进行执行
void SerialprintMQarray();      // 串口打印，Debug 函数

// Interrupt1函数声明
void ShowMaxArray();            // 将中断1触发后的数据，在loop中进行显示，延时时间为10ms

// EEPROM 存储的函数声明
void EepromWriteMaxArray(int EEP_data);        // 写入每次检测到的最大浓度值EEPROM存储器,
void EepromWriteGrade(int grade_data);        // 写入每次检测的等级值
void EepromRead();                            // 从EEPROM存储器读出数据
void EepromClear();                           // 清除EEPROM里面的数据

// I2C 显示MQ135的采样函数
void please();                        // I2C显示 Please press the key
long mapping(long x, long in_min, long in_max, long out_min, long out_max); // 函数的映射
int MQgrade(int temp);                 // 进行等级的评定

/***********************************************************/
// Interrupt0  全局变量的定义
int pinInterrupt = 2;           //连接震动开关到中断0，也就是数字引脚2
int array[100];                 //定义全局数组,验证1
int sampling;                   //定义一个全局变量,为了传递采样了多少个数据，验证2;
int number = 0;                 //总的进行了多少次检测
int Initarray;                  //每次检测中array的最大浓度值
int grade;                      //定义每次检测的最大浓度值

// Interrupt1  全局变量的定义
int pinInterrupt1 = 3;           //连接震动开关到中断1，也就是数字引脚3
int MaxArray[100];              // 定义全局数组，用于保存每次检测的最大值
int GradeArray[100];              // 用于保存每次检测最大浓度值的等级Grade
int DetectionArray[100];        // 定义检测的次数

// EEPROM全局变量申明
int addr = 0;                          // EEPROM 写入的地址
int GradeAddr = 100;                  //  EERROM 写入等级值
byte value;                           // EEPROM读取的最大浓度值
byte GradeValue;                      // EEPROM读取的最大等级

// I2C的显示
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C communication

/***********************************************************/
void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);     // 9600 bps
  while (!Serial) {      // wait for serial port to connect. Needed for native USB port only
    ;                    
  }
  // Interrupt0 初始化
  pinMode(pinInterrupt, INPUT_PULLUP);        // 初始化pinInterrupt0震动开关为输入模式
  attachInterrupt( digitalPinToInterrupt(pinInterrupt), blink, RISING);
  // Interrupt1 初始化
  pinMode(pinInterrupt1, INPUT_PULLUP);        // 初始化pinInterrupt1震动开关为输入模式
  attachInterrupt( digitalPinToInterrupt(pinInterrupt1), EepromRead, FALLING);
}

void loop() {
  // please();                                 // I2C显示 Please press the key
  MemoryData();                               /// *MemoryData() (1)保存采样数据到数组; (2)跳出循环*/
  ShowMaxArray();                           // 将中断1触发后的数据，在loop中进行显示，延时时间为10ms
  // Serial.println("Please press the key");
  delay(100);                             // 这个必须的保持着动态检测，这样才能实时的进行显示；因为我的中断是检测上升沿
                                
}

/***************************************************************/
// /*（1）一直显示 Please press on the key；太占program,取消编程*/
// void please(){
//   u8g.firstPage();                          //一下是显示实现部分
//   do {
//     u8g.setFont(u8g_font_fub14);           //设置字体和自号，目前测试字号有fub14,17,20,30
//                                           // 字体为20，占有的字节数会超了
//     u8g.setPrintPos(10, 20);              //显示的位置
//     u8g.println("Please press");          //显示ppm字样,单位。但是不能显示中文
//     u8g.setPrintPos(10, 50);              //显示的位置
//     u8g.print("the key1 ");                //显示cm字样,单位。但是不饿能显示中文    
//   } while( u8g.nextPage() );
//   delay(100);
// }

/***************************************************************/
/*MemoryData() (1)保存采样数据到数组; （2）IIC动态显示(2)跳出循环*/
void MemoryData(){
  // 变量定义
  int AnalogPin = 0;                                // 定义模拟的采样通道 
  int val;                                          // 定义模拟通道0，采集到的由电压采样的到的0-1024的值 
  sampling = 1;                                     // 定义初始采样次数 sampling =1,数组的size = (sampling + 1)
  
  if ( digitalRead(pinInterrupt) == LOW ){          // 检测按键是否按下
    Serial.println("Key Down");                     // 打印一个按下的标志位
    for(int i=0; i<100; i++){                       // 对数组进行数据的采集
      val=analogRead(AnalogPin);                    //函数 analogRead 在读取模拟值之后, 将根据参考电压将 模拟值转换到[0,1023]区间
      array[i]=mapping(val, 0, 1024, 10, 1000);     //映射函数;

      // 串口调试
      // Serial.print("The detection value is : ");
      // Serial.print(array[i]);
      // 对每一次数组的采样值进行OLED显示
      u8g.firstPage();                              //一下是显示实现部分
      do {
        u8g.setFont(u8g_font_fub30);                //设置字体和自号，目前测试字号有fub14,17,20,30
        u8g.setPrintPos(0, 50);                     //显示的位置
        u8g.print(array[i]);                        //显示变量i的值
      } while( u8g.nextPage() );
      delay(500);
      sampling++;                                 // 采样数sampling 进行加1
      if ( digitalRead(pinInterrupt) == HIGH )   // 每500ms检测一次按键，按键是否抬起，抬起的话就跳出循环
      {
        // Serial.print("The detection Maxvalue is : ");     // 为了测试全局变量：中断函数中的值能返回到主函数中。
        // Serial.println(Initarray);
        u8g.firstPage();                          //一下是显示实现部分
        do {
          u8g.setFont(u8g_font_fub14);           //设置字体和自号，目前测试字号有fub14,17,20,30
          u8g.setPrintPos(10, 20);              //显示的位置
          u8g.print(Initarray);                      //显示变量i的值
          u8g.setPrintPos(60, 20);              //显示的位置
          u8g.println("ppm");                   //显示ppm字样,单位。但是不能显示中文
          u8g.setPrintPos(10, 50);              //显示的位置
          u8g.print("grade = ");                //显示cm字样,单位。但是不饿能显示中文
          u8g.println(grade);       
        } while( u8g.nextPage() );
        //delay(5000);
        break;                                   // continue 当执行完中断后，会保留原来的值；用的是break;
      }  
    }
  }                                  
}

/************************************/
/*Blink 中断0函数，检测到按键Rising时，执行函数*/
void blink() {                
  if( digitalRead(pinInterrupt) == LOW){
  }
  else{
    SerialprintMQarray();     // 对排序得到数组array的最大值
    grade = MQgrade(Initarray);       // 对数组中的最大值浓度进行等级评定
  }
}

/************************************/
/*(1)传递数据和采样次数;(2)排序得到最大值(3)记录检测了多少次*/
void SerialprintMQarray(){
  // 定义变量
  Serial.println("Key UP");                      // 检测按键是否抬起
  Serial.print("The sampling is sampling :");    // 输出采样的次数 sampling
  Serial.println(sampling);          

  Initarray = array[0];                       // 定义数组array的初值  
  // Serial.print("The Initarray is :");    // 输出采样的次数 sampling
  // Serial.println(Initarray);               
  for(int i=0; i<sampling; i++){                // 输出采样数组里面的值 array[]
    if(Initarray < array[i]){
      Initarray = array[i];   
    }               
  }
  number++;                                    //总的进行了多少次检测
  EepromWriteMaxArray(Initarray);               // 调用EepromWrite函数，将数据存入EEPROM
}

/**********************************************/
// EEPROM_WRITER 写入每次检测的最大浓度值
void EepromWriteMaxArray(int EEP_data){
  // put your main code here, to run repeatedly:
  //每次写入1个字节，注意不要溢出。
  EEPROM.write(addr, EEP_data);               // 写入数据      
  addr = addr + 1;                            // addr地址加1
  if (addr == EEPROM.length()) {              // 判断地址是否大于EEPROM最大字节
    addr = 0;
  }
  delay(100);
}
// EEPROM_WRITE 写入每次的等级值
void EepromWriteGrade(int grade_data){
  EEPROM.write(GradeAddr, grade_data);               // 写入数据      
  GradeAddr = GradeAddr + 1;                            // addr地址加1
  if (addr == EEPROM.length()) {              // 判断地址是否大于EEPROM最大字节
    addr = GradeAddr;
  }
  delay(100);
}

/**********************************************/
/*********中断1函数*******************/
// EEPROM_READ 读出检测的次数，每次检测的最大值，保存在两个数组里面
void EepromRead(){
  // 读一个字节
  int address = 0;                              // 设置为局部变量，这样每次都是从addr的第一位开始读取
  Serial.print("Detecte time are : ");          // 检测的次数 
  Serial.println(number);   
  for(int i=0; i<addr; i++){
    value = EEPROM.read(address);              // 读出address里面的数据，放在MaxArray,返回loop
    MaxArray[i] = value;                      // 第address = 0 保存了第一个值
    GradeValue = EEPROM.read(address+100);              // 读出address里面的数据，放在MaxArray,返回loop    
    GradeArray[i] =GradeValue;
    DetectionArray[i] = address+1;            // 表示第一次检测
    
    //串口打印检测的函数 
    // value = EEPROM.read(address);                 // 读出address里面的数据，赋值给value
    // Serial.println(value);
    // Serial.print(DetectionArray[i]);             // 打印出地址数，后面可以用作第几次检测的标志位
    // Serial.print("\t");                          // 输出几个空格
    // Serial.print(MaxArray[i], DEC);             // 将value 的值 转换为decimal 十进制的
    // Serial.println();  
    // Serial.print("EEPROM read the Grade : ");        // 打印Grade
    // Serial.println(GradeArray[i]);                

    //用address的次数，表示检测次数
    address = address + 1;                      // READ adress 加1
  }
  if (address == EEPROM.length()) {           // 判断地址是否大于EEPROM最大字节
    address = 1;
  }
}

/**********************************************/
// 将中断1触发后的数据，在loop中进行显示，延时时间为10ms
void ShowMaxArray(){
  // 打印出最近三次的数据就行
  if(digitalRead(pinInterrupt1) == LOW){
      // for(int i = addr-1; i>= (addr-3) && i>=0 ; i--){
        int i = addr-1;
        u8g.firstPage();                          //一下是显示实现部分
        do {
          // 显示标签值
          u8g.setFont(u8g_font_fub14);           //设置字体和自号，目前测试字号有fub14,17,20,30
          u8g.setPrintPos(0, 15);                //显示的位置
          u8g.print("T");                       //显示变量i的值
          u8g.setPrintPos(40, 15);              //显示的位置
          u8g.println("ppm");                   //显示ppm字样,单位。但是不能显示中文
          u8g.setPrintPos(110, 15);              //显示的位置
          u8g.println("G");                      //显示cm字样,单位。但是不饿能显示中文
          // 显示最后一次采样对应的值
          u8g.setPrintPos(0, 40);              //显示第几次检测Detection
          u8g.print(DetectionArray[i]);        
          u8g.setPrintPos(40, 40);             // 显示每次最大浓度值ppm
          u8g.print(MaxArray[i]);
          u8g.setPrintPos(110, 40);            // 显示等级grade
          u8g.println(GradeArray[i]);
          // 显示倒数第二次采样值
          u8g.setPrintPos(0, 60);              //显示第几次检测Detection
          u8g.print(DetectionArray[i-1]);        
          u8g.setPrintPos(40, 60);             // 显示每次最大浓度值ppm
          u8g.print(MaxArray[i-1]);
          u8g.setPrintPos(110, 60);            // 显示等级grade
          u8g.println(GradeArray[i-1]);
        } while( u8g.nextPage() );
        delay(1000);
      // 串口打印检测
      for(int i = addr-1; i>= (addr-3) && i>=0 ; i--){
        Serial.print(DetectionArray[i]);
        Serial.print("\t");
        Serial.print(MaxArray[i]);
        Serial.print("\t");
        Serial.println(GradeArray[i]);
      }
    //delay(8000);
  }
  
}


/***********************************************/
// mapping 映射函数
long mapping(long x, long in_min, long in_max, long out_min, long out_max){
  long ratio;
  long map_value;
  ratio = (x - in_min) * (out_max - out_min) / (in_max - in_min) ;       // 通过计算value的值在0-1024的比例后，求出value 在0-255 站的权重值
  map_value = ratio + out_min;                                          // 权重加上初值
  return map_value;
  //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/***********************************************/
/********针对于第一块传感器**********************/
// 对浓度最大值进行等级分配
int MQgrade(int temp){
  // temp = mapping(temp, 150, 400, 50, 100);       //对于买的MQ135模块，预热成功后，只用调节浓度分配的初值就行
  int start_value = 46;                             //针对 单MQ135传感器的话，在上面加一层映射，主要是为了保证实时显示
  int n = 15;
  int score[n];
  for(int i=0; i<n; i++){
    score[i] = start_value;
    start_value += 5; 
  }
  // int score[] = {30,                            // 0 未检测
  //                 40,45,50,                     // 1-3 轻微
  //                 55,65,70,                     // 4-6 中度
  //                 75,85,90,                     // 7-9 重度
  //                 100                           // 100 检测其他物体/或者传感器没有正常连接     
  //                 };       
  if(temp <= score[3] )
  {
    if( temp < score[0] ){
      grade = 0;                       // 你没有口气，不要提示重新测试
    }
    else if( temp >= score[0] && temp < score[1] ){
      grade = 1;                      // 轻微的口气，正常的范围
    }
    else if( temp >= score[1] && temp < score[2] ){
      grade = 2;
    }
    else if( temp >= score[2] && temp < score[3] ){
      grade = 3;
    }
  }
  else if (temp >= score[3] && temp < score[6] )
  {
    if( temp >= score[3] && temp < score[4] ){
      grade = 4;                      // 轻微的口气，正常的范围
    }
    else if( temp >= score[4] && temp < score[5] ){
      grade = 5;
    }
    else if( temp >= score[5] && temp < score[6] ){
      grade = 6;
    }
  }
  else if (temp >= score[6] && temp < score[9])
  {
    if( temp >= score[6] && temp < score[7] ){
      grade = 7;                      // 轻微的口气，正常的范围
    }
    else if( temp >= score[7] && temp < score[8] ){
      grade = 8;
    }
    else if( temp >= score[8] && temp < score[9] ){
      grade = 9;
    }
  }
  else if (temp > score[10] )
    grade = 10;
  EepromWriteGrade(grade);
  return grade;
} 