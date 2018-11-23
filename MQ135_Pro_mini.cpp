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

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 9600 bps
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val;
  long value;
  int analogPin = 0;  
  val=analogRead(analogPin);    //函数 analogRead 在读取模拟值之后, 将根据参考电压将 模拟值转换到[0,1023]区间
  value = mapping(val, 0, 1024, 10, 1000);

  // OLED 动态显示value的值
  for(int i=1;i<10;i++){
     u8g.firstPage();                // 是显示实现部分
  do {
    // 动态显示10秒延时，进行传感器的预热
    for(int j = 10; j > 0 ; j-- ){
      u8g.setFont(u8g_font_fub30);       //设置字体和自号，目前测试字号有fub14,17,20,30
      u8g.setPrintPos(50, 50);            //显示的位置
      u8g.print(j);                  //显示变量i的值
    }
    delay(10);

    // OLED 动态显示value的值   
    u8g.setFont(u8g_font_fub30);       //设置字体和自号，目前测试字号有fub14,17,20,30
    u8g.setPrintPos(0, 50);            //显示的位置
    if((value>30) && (value < 50){
      u8g.print(value);                  //显示变量i的值
      u8g.setFont(u8g_font_fub14);        //设置字体和自号
      u8g.setPrintPos(0, 80);            //显示的位置
      u8g.print("ppm");                   //显示cm字样,单位。但是不饿能显示中文
      u8g.print(" Grade 1 ");
    }
    else if( value >= 50 && value < 70 ){
      u8g.print(value);                  //显示变量i的值
      u8g.setFont(u8g_font_fub14);        //设置字体和自号
      u8g.setPrintPos(0, 80);            //显示的位置
      u8g.print("ppm");                   //显示cm字样,单位。但是不饿能显示中文
      u8g.print(" Grade 2 ");
    }
    else if( value >= 70 && value < 90 ){
      u8g.print(value);                  //显示变量i的值
      u8g.setFont(u8g_font_fub14);        //设置字体和自号
      u8g.setPrintPos(0, 80);            //显示的位置
      u8g.print("ppm");                   //显示cm字样,单位。但是不饿能显示中文
      u8g.print(" Grade 3 ");
    }
    else if( value >= 90 && value < 120 ){
      u8g.print(value);                  //显示变量i的值
      u8g.setFont(u8g_font_fub14);        //设置字体和自号
      u8g.setPrintPos(0, 80);            //显示的位置
      u8g.print("ppm");                   //显示cm字样,单位。但是不饿能显示中文
      u8g.print(" Grade 4 ");
    }
    else if( value >= 120 ){
      u8g.print(value);                  //显示变量i的值
      u8g.setFont(u8g_font_fub14);        //设置字体和自号
      u8g.setPrintPos(0, 80);            //显示的位置
      u8g.print("ppm");                   //显示cm字样,单位。但是不饿能显示中文
      u8g.print(" Grade 5 ");
    }
    else{
      u8g.print(" Please try again ");
    }
    
  } while( u8g.nextPage() );
  delay(50);                            //显示的时间间隔。    
  }

  // 做一个判断，浓度值的阈值限制
  if( value > 300){
    Serial.print("value = ");
    Serial.print(value);
    Serial.print("  ppm");
    Serial.print("  -->  ");
    Serial.println("Sorroy, You need a doctor.");
    while(1);
  }
  else{
    Serial.print("You are OK.");
    Serial.println("Please contiune to detective");
    delay(100);
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
