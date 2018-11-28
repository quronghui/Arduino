# Arduino
This is arduino project.
## Hello word 
+ Title :     Hello_word
+ Keyword :   Arduino; IIC OLED; Uglib
+ Funcation : Dynamic display variable in the OLED.
## Led_gradual_change
1. Title :     Led_gradual_change
2. Keyword :   Arduino; LED; Analog
3. Funcation1 ：实现LED灯的渐变；
4. Note : 项目还未完成，没有调通，需要产生模拟信号的东西，是否可以通过示波器
## Usigned long
1. Title :     Usigned long
2. Keyword :   System Time 
3. Funcation1 ：Gain Arduino sysytem time and println
4. Notes: int（-2^15 -- 2^15-1） and usigned int(2^16-1) size
## Static_Scope
1. Title :     Static_Scope
2. Keyword :   限制数的范围 
3. Funcation1 ：限制数在某一个范围内随机生成
4. Notes:       RandomWalk函数在两个终点间随机的上下移动，在一个循环中最大的移动由参数“stepsize”决定，一个静态变量向上和向下移动一个随机量。这种技术也被叫做“粉红噪声”或“醉步”
## Pulseln
1. Title :      Pulseln
2. Keyword :    检测跳变沿 
3. Funcation1 ：用于检测脉冲,高低转换的时间长短
4. Notes:       unsigned long pulseIn (uint8_t pin, uint8_t state, unsigned long timeout) 
## Mathlib
1. Title :      Mathlib
2. Keyword :    mapping randomseed 
3. Funcation1 ：mathlib funcation
4. Notes:       
## Interrupt 
1. Title :     Interrupt 
2. Keyword :   Interrupt Funcation
3. Funcation1 : Test Interrupt Funcation
4. Notes : 
## Uart_Communication 
1. Title :     Uart_Communication 
2. Keyword :   Arduino UNO
3. Funcation1 : Serial.available()获取串口上可读取的数据的字节数。
4. Notes : 
## MQ135_Bluetooth
1. Title :     MQ135_Bluetooth
2. Keyword :   Arduino UNO ; Open Jumper bluetooth; MQ135; OLED IIC
3. Funcation1 : Detection MQ135 analog value; 
4. Funcation2 ：OLED dynamic display MQ135_ppm or TP410T;
5. Funcation3 : Bluetooth send value 
6. Funcation4 : Uart downloading
## MQ135_Pro_nimi
1. Title :     MQ135_Pro_nimi
2. Keyword :   Arduino pRO MINI; Open Jumper bluetooth; MQ135; OLED IIC
3. Funcation1 : Detection MQ135 analog value; 
4. Funcation2 ：OLED dynamic display MQ135_ppm or TP410T;
5. Funcation3 : Bluetooth send value 
6. Funcation4 : Uart downloading
7. Notes: TP410T model grade need change code. Because it have't electric circuit. 
 
