/*******************************************/
//  Analog single
/* This is MQ-135 analog read and debug */
/* Analog voltage 0.3V -- 4V */
/*******************************************/

#include <Arduino.h>
#include <U8glib.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); 

long mapping(long x, long in_min, long in_max, long out_min, long out_max);
void setup() 
{ 
  Serial.begin(9600); // 9600 bps
}

void loop() 
{
  int val;
  long value, voltage;
  int analogPin = 0;  
  val=analogRead(analogPin);  //函数 analogRead 在读取模拟值之后, 将根据参考电压将 模拟值转换到[0,1023]区间
  Serial.print('\n');
  Serial.print("AD sampling = ");
  Serial.print(val ,DEC);   // debug value

  // 电压值和AD值的映射
  voltage = mapping(val, 0, 1024, 1, 4);
  Serial.print("  -->  ");
  Serial.print("voltage = ");
  Serial.print(voltage);
  Serial.print(" V");

  // Mapping 进行值的转换,输出打印值，Debug
  value = mapping(val, 0, 1024, 10, 1000);
  Serial.print("  -->  ");
  Serial.print("value = ");
  Serial.print(value);
  Serial.print("  ppm");
  Serial.print('\n');
  delay(100);

  for(int i=1;i<10;i++){
     u8g.firstPage();  //一下是显示实现部分
  do {
  u8g.setFont(u8g_font_fub30);//设置字体和自号，目前测试字号有fub14,17,20,30
  u8g.setPrintPos(0, 50); //显示的位置
  u8g.print(value);//显示变量i的值
  u8g.setFont(u8g_font_fub14);//设置字体和自号
  u8g.setPrintPos(95, 50); //显示的位置
  u8g.print("ppm");//显示cm字样
  } while( u8g.nextPage() );
  delay(100);//显示的时间间隔。    
  }

}

long mapping(long x, long in_min, long in_max, long out_min, long out_max){

  long ratio;
  long map_value;
  ratio = (x - in_min) * (out_max - out_min) / (in_max - in_min) ;   // 通过计算value的值在0-1024的比例后，求出value 在0-255 站的权重值
  map_value = ratio + out_min;                                          // 权重加上初值
  return map_value;
  //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
