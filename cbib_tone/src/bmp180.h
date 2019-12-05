#ifndef BMP180_H
#define BMP180_H

// bmp180初始化的函数
void bmp085Calibration();

// 获取温度值的函数
float bmp085GetTemperature(unsigned int ut);
unsigned int bmp085ReadUT();

// 获取压力值的函数
long bmp085GetPressure(unsigned long up);
unsigned long bmp085ReadUP();
float calcAltitude(float pressure);

#endif