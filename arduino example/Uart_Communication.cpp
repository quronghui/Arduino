/*
* Title :     Uart_Communication
* Keyword :   Arduino; 
* Funcation : Serial.available()获取串口上可读取的数据的字节数。
*/

#include <Arduino.h>

int incomingByte = 0;       // for incoming serial data; 数字端口的RXD口，不是模拟口

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);       // Opening serial port, setting data rate to 9600 bps
}

void loop() {
  // put your main code here, to run repeatedly:
  // Send data only when you resive data:
  if(Serial.available() > 0){
    // Reading the incoming byte:
    incomingByte = Serial.read();

    // Println the receive data
    Serial.print("I receive: ");
    Serial.println(incomingByte, DEC);    // DEC--十进制数
  }
}

