#include <Arduino.h>

#include <SPI.h>
//#include "ad.h"
//spi_include
//wifi_include
#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

//pwm_define
// use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0     0

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  13

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     1000

// fade LED PIN (replace with LED_BUILTIN constant for built-in LED)
#define LED_PIN         16  

void ledcAnalogWrite(uint8_t channel, uint32_t duty) {//pwm control 
  // calculate duty, 8191 from 2 ^ 13 - 1
  //uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}
//wifi define 
const char *ssid = "AirSensor";
const char *password = "AirSensor";
float Air_data[12];
WiFiServer server(8090);

//ad define
#define ad_start 22
#define ad_drdy 17

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
static const int spiClk = 1000000; // 4 MHz
SPIClass * vspi = NULL;
//command
#define NOP 0x00
#define WAKEUP 0x02
#define POWERDOWN 0x04
#define RESET 0x06
#define START 0x08
#define STOP 0x0a
#define RDATA 0x12
#define RREG(i) (0x20 | i)
#define WREG(i) (0x40 | i)

//address
#define ID 0x00
#define STATUS 0x01
#define INPMUX 0x02
#define PGA 0x03
#define DATARATE 0x04
#define REF 0x05
#define IDACMAG 0x06
#define IDACMUX 0x07
#define VBIAS 0x08
#define SYS 0x09
#define OFCAL0 0x0b
#define OFCAL1 0x0c
#define FSCAL0 0x0e
#define FSCAL1 0x0f

//reg
#define PGA_(i) (1<<3 | i)
    //gain list: 0-1, 1-2, 2-4, 3-8, 4-16, 5-32, 6-64, 7-128
#define MUX_(i) (i<<4 | (0xc)) 
    //[7:4]-i for single-end port to positive 
    //[3:0]-1100 for negative port connect to AINCOM
#define DATARATE_ (0x3e) 
    //8'b00111110; [6]-0 for internal clock, 
    //[5]-1 for single-shot mode,
    //[4]-1 reserved
    //[3:0]-1110 for 4000SPS
#define REF_ (0x0a)
    //[5:4]-00 for disable reference buffer bypass
    //[3:2]-10 for internal 2.5v reference
    //[1:0]-10 for internal always on
#define IDACMAG_(i) (i)
#define IDACMUX_(i,j) (i<<4 | j)
#define VBIAS_(i) (i<<6)//connect vbias to AINCOM
#define SYS_(i) (i<<5 || 1<<4) //[7:5]-i for system monitor,
    //default 8 samples, default disable timeout
#define MON_DISABLE 0
#define MON_VBIAS 1
#define MON_TEMP 2
#define MON_AVDDtoAVSSdiv4 3
#define MON_DVDDdiv4 4
#define MON_p2uA 5
#define MON_1uA 6
#define MON_10uA 7
//rest regs are useless or reserved
u8 volt=53;

void wait()
{
    while (digitalRead(ad_drdy));
}

void write(u8 cmd)
{
    vspi->transfer(cmd);
}

u8 read(void)
{
    return vspi->transfer(0x0);
}
u8 read_reg(u8 address)//read 1 register for once
{
  //wait();
  write(1<<5 | address);
  write(0x00);
  return read();
}

void write_reg(u8 address, u8 reg)
{
//  wait();
  write(1<<6 | address);
  write(0x00);
  write(reg);
}

void set_pga(u8 g)
{
  //wait();
  write_reg(PGA,PGA_(g));
}

void set_mux(u8 i)
{
  write_reg(INPMUX,MUX_(i));
}

s16 read_data(void)
{
    s16 a;
  digitalWrite(ad_start, 1);
  wait();
  write(RDATA);
  a= (read()<<8);
  a|=read();
  digitalWrite(ad_start, 0);
  return a;
}

float cal_res(u8 gain, s32 div_res,s16 data)
{
    //s32 temp= div_res*data;
  return  ((float)(1<<(15+gain)) / (float)data * (float)(div_res) -div_res) + 200000 ;
}

s32 dat[12];
int gain[12];
void sweep_read()
{
  s16 temp;
  s8 i,j;
  for(i=0;i<12;i++)
  {
    set_mux(i);
    for(j=7;j>=0;j--)
    {
      set_pga(j);
      temp = read_data();
            //printf("%d\r\n",temp);
      if (temp <= 32764 && temp >= -32764)
      {
                dat[i]=temp;
                gain[i]=j;
                Air_data[i] = -cal_res(j, 100000, temp);
        break;
      } 
            //dat[i][]=temp;
      //res[i]=cal_res(j, 1e5 ,temp);
            //gain[i]=1<<j;                
            dat[i]=temp;
            gain[i]=j;
            Air_data[i] = -cal_res(j, 100000, temp);
    }
  }
}

void ad_init(void)
{
    //SPI2_Init();
  //io_init();
    //ad_start=0;
  //wait();
  pinMode(ad_start, OUTPUT);
  pinMode(ad_drdy, INPUT);
  digitalWrite(ad_start, 0);
  vspi = new SPIClass(VSPI);
  vspi->begin();
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE1));
  write(RESET);
    delay(2);
//    wait();
  write_reg(REF,REF_);
  //  write_reg(VBIAS,VBIAS_(0));
  write_reg(DATARATE,DATARATE_);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
  
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
  
  Serial.println("pwm init done");
  
  ad_init();
  Serial.println("ad init done");
  Serial.println(read_reg(0x00));
  
  pinMode(33, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(33,1);
//  ledcAnalogWrite(LEDC_CHANNEL_0, 8191);
}

char flag=0;

void loop() {
  delay(15000);
  while(1){
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();   // listen for incoming clients
  sweep_read();
//  float ftemp;
//  ftemp = Air_data[4];
//  Serial.println(ftemp);
  ledcAnalogWrite(LEDC_CHANNEL_0, 8191/5*(49-48));//占空比调节1-5
  while (client.connected()) 
  {            // loop while the client's connected
      digitalWrite(32, 1);
      sweep_read();
//      for(int i = 0; i<6; i++) 
//      {
//        Serial.println(i);
//        Serial.println(Air_data[i]);
//      }
        float ftemp1[12];
        ftemp1[0] = Air_data[0];  //Serial.println(Air_data[4]);
        ftemp1[1] = Air_data[1];
        ftemp1[2] = Air_data[2];
        ftemp1[3] = Air_data[3];
        ftemp1[4] = Air_data[4];
        ftemp1[5] = Air_data[5];
        ftemp1[6] = Air_data[6];

      client.write((uint8_t*)&ftemp1, 4*12); //4*10
//      if(ftemp1[0]>50000000){
//        digitalWrite(25,1);        
//        }
//       else digitalWrite(25,0);
      if (client.available()) {             // if there's bytes to read from the client,
        volt = client.read();
        Serial.println(volt);    
        ledcAnalogWrite(LEDC_CHANNEL_0, 8191/5*(volt-48));//占空比调节1-5V
      }
      delay(500);
    //}    
  digitalWrite(32, 0);  
  }
  digitalWrite(32, 0);
  
//        if(ftemp>50000000){
//        digitalWrite(25,1);        
//        }
//        else digitalWrite(25,0);
  /*sweep_read();
  for(int i = 0; i<12 ;i++)
  {
    Serial.print(i);
    Serial.print(":  ");
    Serial.println(res[i]);
    delay(1000);
  }*/
  }
}