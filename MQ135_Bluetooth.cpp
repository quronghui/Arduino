// /*
// * Title :     MQ135_Bluetooth
// * Keyword :   Arduino; Open Jumper bluetooth; MQ135; OLED IIC
// * Funcation1 : Detection MQ135 analog value; 
// * Funcation2 ：OLED dynamic display MQ135_ppm;
// * Funcation3 : Bluetooth send value 
// */

// #include <Arduino.h>
// #include <U8glib.h>
// #include <SoftwareSerial.h>

// long mapping(long x, long in_min, long in_max, long out_min, long out_max);

// U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); 

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600); // 9600 bps
//   pinMode(13, OUTPUT);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   int val;
//   long value;
//   int analogPin = 0;  
//   val=analogRead(analogPin);    //函数 analogRead 在读取模拟值之后, 将根据参考电压将 模拟值转换到[0,1023]区间
//   value = mapping(val, 0, 1024, 10, 1000);

//   // OLED 动态显示value的值
//   for(int i=1;i<10;i++){
//      u8g.firstPage();                //一下是显示实现部分
//   do {
//     u8g.setFont(u8g_font_fub30);       //设置字体和自号，目前测试字号有fub14,17,20,30
//     u8g.setPrintPos(0, 50);            //显示的位置
//     u8g.print(value);                  //显示变量i的值
//     u8g.setFont(u8g_font_fub14);        //设置字体和自号
//     u8g.setPrintPos(95, 50);            //显示的位置
//     u8g.print("ppm");                   //显示cm字样,单位。但是不饿能显示中文

//   } while( u8g.nextPage() );
//     delay(100);                         //显示的时间间隔。    
//   }

//   // 做一个判断，浓度值的阈值限制
//   if( value > 200){
//     Serial.print("value = ");
//     Serial.print(value);
//     Serial.print("  ppm");
//     Serial.print("  -->  ");
//     Serial.println("Sorroy, You need a doctor.");
//     while(1);
//   }
//   else{
//     Serial.print("You are OK.");
//     Serial.println("Please contiune to detective");
//     delay(100);
//   }
// }
// // mapping 映射函数
// long mapping(long x, long in_min, long in_max, long out_min, long out_max){
//   long ratio;
//   long map_value;
//   ratio = (x - in_min) * (out_max - out_min) / (in_max - in_min) ;   // 通过计算value的值在0-1024的比例后，求出value 在0-255 站的权重值
//   map_value = ratio + out_min;                                          // 权重加上初值
//   return map_value;
//   //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }


/*
* Title :     MQ135_Bluetooth
* Keyword :   Arduino; Open Jumper bluetooth; MQ135; OLED IIC
* Funcation1 : Detection MQ135 analog value; 
* Funcation2 ：OLED dynamic display MQ135_ppm;
* Funcation3 : Bluetooth send value 
*/

#include <Arduino.h>
#include <U8glib.h>
#include <SoftwareSerial.h>

long mapping(long x, long in_min, long in_max, long out_min, long out_max);
void Waiting(int time);
void Store_array(int number);           // 进行采样值的存储
void SerialPrintMQ(int* p, int size);   // 数组可以进行传值，打印数组
int MaxMQ(int* p, int size);           // 选取函数中的最大值
void draw(int temp);                    // 对最大值进行显示


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 9600 bps
  pinMode(13, OUTPUT); 
}

void loop() {

  // 参数的定义
  int number = 5;              // sampling_number 的定义 
  int time = 3;                // delay延时的时间长度，倒计时的显示      

  // 函数框架
  Waiting(time);               // 延时预热的函数
  Store_array(number);         // 对采样数据进行数组的存储
  while(1);                    // 跳出loop循环，一直显示固定值 
}

// 传感器倒计时的预热功能
void Waiting(int time){
  for(int i = time; i > 0; i-- ){
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
  int* p = MQarray;

  // 对采样的数据进行数据存取
  for(int i = 0; i < number; i++ ){
    // 参数定义
    val=analogRead(analogPin);    //函数 analogRead 在读取模拟值之后, 将根据参考电压将 模拟值转换到[0,1023]区间
    Serial.print(val);
    value = mapping(val, 0, 1024, 10, 1000);
    MQarray[i] = value;
    
    // 对每一次数组的采样值进行OLED显示
    u8g.firstPage();                //一下是显示实现部分
    do {
      u8g.setFont(u8g_font_fub30);       //设置字体和自号，目前测试字号有fub14,17,20,30
      u8g.setPrintPos(0, 50);            //显示的位置
      u8g.print(MQarray[i]);                  //显示变量i的值
    } while( u8g.nextPage() );
     delay(1000);
  }
  SerialPrintMQ(MQarray, number);
  temp = MaxMQ(MQarray, number);
  draw(temp);
  //SerialPrintMQ(temp, 1);

}

// serialPrint的一个打印
void SerialPrintMQ(int* p, int size){
  for(int i = 0; i < size; i++){
    Serial.print("value = ");
    Serial.print(p[i]);
    Serial.print("  ppm");
    Serial.print("  -->  ");
    Serial.println("Sorroy, You need a doctor.");
  }
}

// mapping 映射函数
long mapping(long x, long in_min, long in_max, long out_min, long out_max){
  long ratio;
  long map_value;
  ratio = (x - in_min) * (out_max - out_min) / (in_max - in_min) ;   // 通过计算value的值在0-1024的比例后，求出value 在0-255 站的权重值
  map_value = ratio + out_min;                                          // 权重加上初值
  return map_value;
  //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// 取数组的最大值
int MaxMQ(int* p, int size){
  int temp = p[0];
  for(int i = 1; i < size; i++ ){
    if(temp < p[i]){
      temp = p[i];
    }
  }
  return temp;
}

// 屏幕显示最大值
void draw(int temp){
    u8g.firstPage();                //一下是显示实现部分
    do {
      u8g.setFont(u8g_font_fub20);       //设置字体和自号，目前测试字号有fub14,17,20,30
      u8g.setPrintPos(0, 20);            //显示的位置
      u8g.print(temp);                  //显示变量i的值
      u8g.setFont(u8g_font_fub14);        //设置字体和自号
      u8g.setPrintPos(0, 50);            //显示的位置
      u8g.print("ppm");                   //显示cm字样,单位。但是不饿能显示中文
    } while( u8g.nextPage() );
     delay(1000);
}
// 对口气进行分级

