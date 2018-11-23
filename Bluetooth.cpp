/*
* Title :     Bluetooth
* Keyword :   Arduino; bluetooth_v1.0; 
* Funcation : AT command setting； Bluetooth communication with Phone
*/

#include <Arduino.h>
#include <SoftwareSerial.h>


/**********************************************
* Setting AT command
* But don't success
***********************************************/

// // SoftwareSerial mySerial= SoftwareSerial(rxPin, txPin)
// SoftwareSerial BT(10, 11);
// char val;

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
//   Serial.println(" BT is ready! ");
//   // HC-05 默认的波特率
//   BT.begin(38400);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   if(Serial.available()){
//     val = Serial.read();
//     BT.print(val);
//   }
//   if(BT.available()){
//     val = BT.read();
//     Serial.print(val);
//   }
// }


/**********************************************
* Android _ HC-05 and Phone
* 与手机的蓝牙通信
***********************************************/
void setup()
{ 
  Serial.begin(9600);         // 之前代码的波特率设置错了，不是 38400，而是9600
  pinMode(13, OUTPUT);
}


void loop()
{
  while(Serial.available())
  {
    char c=Serial.read();
      if(c=='1')
      {
        Serial.println("BT is ready!");
        // 返回到手机调试程序上
        Serial.write("Serial--13--high");
        digitalWrite(13, HIGH);
      }
     if(c=='2')
     {
       Serial.write("Serial--13--low");
       digitalWrite(13, LOW);
     }
  }
}
