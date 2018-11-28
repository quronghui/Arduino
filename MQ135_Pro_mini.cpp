/*
* Title :     MQ135_Bluetooth
* Keyword :   Arduino; Open Jumper bluetooth; MQ135; OLED IIC
* Funcation1 : Detection MQ135 analog value; 
* Funcation2 ：OLED dynamic display MQ135_ppm;
* Funcation3 : Bluetooth send value 
*/

#include <Arduino.h>
#include <U8glib.h>

long mapping(long x, long in_min, long in_max, long out_min, long out_max); // 函数的映射
void Waiting(int delay_time);
void Store_array(int number);           // 进行采样值的存储
void SerialPrintMQ(int* p, int size);   // 数组可以进行传值，打印数组
int  MQgrade(int temp);                 // 进行等级的评定

// 多次函数的调用，占用了太大的空间，所以去掉这部分函数的调用
// void MaxMQ(int* p, int size);           // 选取函数中的最大值
// void draw(int temp);                    // 对最大值进行显示


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 9600 bps
  
  // pinMode(13, OUTPUT);               // 为了显示有输出
}

void loop() {

  // 参数的定义
  int number = 20;              // sampling_number 的定义 
  int delay_time = 3;                // delay延时的时间长度，倒计时的显示      
  // 函数框架
  Waiting(delay_time);               // 延时预热的函数
  Store_array(number);         // 对采样数据进行数组的存储
  while(1);                    // 跳出loop循环，一直显示固定值 
}

// 传感器倒计时的预热功能
void Waiting(int delay_time){
  for(int i = delay_time; i > 0; i-- ){
    u8g.firstPage();                //一下是显示实现部分
    do {
      u8g.setFont(u8g_font_fub30);       //设置字体和自号，目前测试字号有fub14,17,20,30
      u8g.setPrintPos(0, 50);            //显示的位置
      u8g.print(i);                  //显示变量i的值
    } while( u8g.nextPage() );
     delay(1000);
  }
}

// 对采样值进行数组的存储
void Store_array(int number){

  // 变量定义
  int MQarray[number + 1];
  int val;
  long value;
  int analogPin = 0; 
  int temp;
  int grade;

  // 对采样的数据进行数据存取
  for(int i = 0; i < number; i++ ){

    // 参数定义
    val=analogRead(analogPin);                       //函数 analogRead 在读取模拟值之后, 将根据参考电压将 模拟值转换到[0,1023]区间
    // Serial.println(val);                         //打印采样值
    value = mapping(val, 0, 1024, 10, 1000);       //映射函数
    MQarray[i] = value;
    /*************************************/
    // 对每一次数组的采样值进行OLED显示
    // 这个不影响占有空间的大小
    u8g.firstPage();                     //一下是显示实现部分
    do {
      u8g.setFont(u8g_font_fub30);       //设置字体和自号，目前测试字号有fub14,17,20,30
      u8g.setPrintPos(0, 50);            //显示的位置
      u8g.print(MQarray[i]);             //显示变量i的值
    } while( u8g.nextPage() );
     delay(1000);
  }
  /****************************************/
  // 取最大值进行显示，不加返回值会减小函数调用,取采样值中的最大值
  temp = MQarray[0];
  for(int i = 1; i < number; i++ ){
    if(temp < MQarray[i]){
      temp = MQarray[i];
    }
  }
  /****************************************/
  // 调用函数-->对最大值进行等级分配
  grade = MQgrade(temp);
  /****************************************/
  // 对最大浓度值和等级进行显示
  u8g.firstPage();                          //一下是显示实现部分
  do {
    u8g.setFont(u8g_font_fub14);           //设置字体和自号，目前测试字号有fub14,17,20,30
                                          // 字体为20，占有的字节数会超了
    u8g.setPrintPos(10, 20);              //显示的位置
    u8g.print(temp);                      //显示变量i的值
    u8g.setPrintPos(60, 20);              //显示的位置
    u8g.println("ppm");                   //显示ppm字样,单位。但是不能显示中文
    u8g.setPrintPos(10, 50);              //显示的位置
    u8g.print("grade = ");                //显示cm字样,单位。但是不饿能显示中文
    u8g.println(grade);       
  } while( u8g.nextPage() );
  delay(1000);
  /******************************/
  // 串口打印显示
  SerialPrintMQ(MQarray, number);                     
}
/***********************************************/
// serialPrint的一个打印
void SerialPrintMQ(int* p, int size){
  for(int i = 0; i < size; i++){
    Serial.print("value = ");
    Serial.print(p[i]);
    Serial.print("  ppm");
    Serial.print("  -->  ");
    Serial.println("Hello, please refer to specification.");
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
// 对浓度最大值进行等级分配
int MQgrade(int temp){
  int grade;
  if( temp < 40 ){
    grade = 0;
  }
  else if( temp >= 40 && temp < 80 ){
    grade = 1;
  }
  else if( temp >= 80 && temp < 120 ){
    grade = 2;
  }
  else if( temp >= 120 && temp < 160 ){
    grade = 3;
  }
  
  else
    grade = 4;
  
  return grade;
}   