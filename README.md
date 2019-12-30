# Arduino example
### Hello word 

+ Title :     Hello_word
+ Keyword :   Arduino; IIC OLED; Uglib
+ Funcation : Dynamic display variable in the OLED.
### Led_gradual_change

1. Title :     Led_gradual_change
2. Keyword :   Arduino; LED; Analog
3. Funcation1 ：实现LED灯的渐变；
4. Note : 项目还未完成，没有调通，需要产生模拟信号的东西，是否可以通过示波器
### Usigned long

1. Title :     Usigned long
2. Keyword :   System Time 
3. Funcation1 ：Gain Arduino sysytem time and println
4. Notes: int（-2^15 -- 2^15-1） and usigned int(2^16-1) size
### Static_Scope

1. Title :     Static_Scope
2. Keyword :   限制数的范围 
3. Funcation1 ：限制数在某一个范围内随机生成
4. Notes:       RandomWalk函数在两个终点间随机的上下移动，在一个循环中最大的移动由参数“stepsize”决定，一个静态变量向上和向下移动一个随机量。这种技术也被叫做“粉红噪声”或“醉步”
### Pulseln

1. Title :      Pulseln
2. Keyword :    检测跳变沿 
3. Funcation1 ：用于检测脉冲,高低转换的时间长短
4. Notes:       unsigned long pulseIn (uint8_t pin, uint8_t state, unsigned long timeout) 
### Mathlib

1. Title :      Mathlib
2. Keyword :    mapping randomseed 
3. Funcation1 ：mathlib funcation
4. Notes:       
### Interrupt 

1. Title :     Interrupt 
2. Keyword :   Interrupt Funcation
3. Funcation1 : Test Interrupt Funcation
4. Notes : 
### Uart_Communication 

1. Title :     Uart_Communication 
2. Keyword :   Arduino UNO
3. Funcation1 : Serial.available()获取串口上可读取的数据的字节数。
4. Notes : 
### Interrupt_parameter

1. 调代码一步步的调
2. 验证数组可以传递到中断中
3. 如果变量的值一直在变动,这个如何传递. for循环中的判断值,不能是动态变量的
4. 验证全局变量 sampling， 可以传递到中断中
5. 验证检测了第几次口气 number ，可以传递到中断中 ?  函数都是loop进行循环的，想要保存的话，是放在哪里呢？

# Project MQ135

## Arduino Uno MQ135

### MQ135InterruptOriginal.cpp

+ 调代码一步步的调

1. 以实现loop函数和Interrupt函数值的传递；
2. 实现检测次数的累加，和每次检测采样的次数sampling；
3. 将需要的值写入EEPROM，进行存储； 
4. 中断0：检测key1，Key Down采样口气值，Key UP显示本次采样的最大值；
5. 中断1：检测key2,Key Down 显示第几次采样，最大浓度值，等级
6. 按键算法,还未成功移入?

7.  Library

   + board = uno

     framework = arduino

     lib_deps = U8glib_ID7

     lib_deps = SoftwareSerial_ID2728

     lib_deps = MQ135

     lib_deps = clickButton_ID1105

### MQ135_Interrupt. cpp

6. 刚加入加入按键算法,还未成功?

### timer.cpp

1.定时器中断：实现成功
2.但是在中断中的定时器：每隔一个中断的时间，执行一次中断，达不到效果

### Interrupt.cpp

1. 中断中加入定时器延时：每隔一个中断的时间，执行一次中断

### ClickButton.cpp

1. github 查找到的

## ArduinoMQ135Pro_mini

+ 知识迁移了一下平台，代码基本上没有变

### MQ135_Pro_Original.cpp

1. 以实现loop函数和Interrupt函数值的传递；
2. 实现检测次数的累加，和每次检测采样的次数sampling；
3. 将需要的值写入EEPROM，进行存储； 
4. 中断0：检测key1，Key Down采样口气值，Key UP显示本次采样的最大值；
5. 中断1：检测key2,Key Down 显示第几次采样，最大浓度值，等级
6. 按键算法,还未成功移入?

### MQ135_Pro_Interrupt.cpp

# Project cbib_tone

+ 项目中使用了两块bmp180，由于其IIC通信时address是相同的，我在mega上没有找到区别的方式；因此加了一块Arduino mega
+ 注意：
  + Judge_mode ：排气模式中，第三部分传感器阵列排气的气压控制信号是由Arduino UNO 提供的；
  + Judge_mode：检测模式中，需要保证在气体输送的同时，PC端就开始接收数据，这里存在一个打开的顺序问题？
    + PC端先打开串口 -->  嵌入式端按下检测模式按钮 -->  PC段发送采集次数；

## cbib_tone_mega

1. main.cpp
   + 实现电子鼻项目的采集气路装置；
   +  Judge_mode() ：实现气路装置三种模式的控制；
   + Gas_Detection()：实现传感器整列数据的采集和串口传输；
2. bmp180.cpp bmp180.h
   + 实现储气带的气压采集；

## cbib_tone

1. 为Arduino mega 主控制板的Judge_mode()，排气模式中第三部分传感器阵列排气的控制，提供气压控制信号。

2. 代码逻辑修改：
   + 1）传感器整列先采集数据，然后在发送数据
   + 2）当检测到