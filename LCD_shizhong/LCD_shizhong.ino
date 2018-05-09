
#include<LiquidCrystal.h>//LCD
//#include "DHT.h"//温湿度传感器  
#include <DS1302.h>//时钟模块
#include <stdio.h>
#include <string.h>

//温湿度传感器初始化
//#define DHTPIN 13 //定义针脚
//#define DHTTYPE DHT11 //定义类型，DHT11或者其它
//DHT dht(DHTPIN, DHTTYPE); //进行初始设置 
const int inPin=0;//温度传感器模拟引脚
//LCD的初始化设置
const int numRows=2; //LCD的行数
const int numCols=16; ///LCD的列数
int count;
const int degreeSymbol=B11011111;//度
LiquidCrystal lcd(12,11,5,4,3,2); //使用界面引脚的编号初始化库

//DS1302初始化设置
/* 接口定义
CE(DS1302 pin5) -> Arduino D6
 IO(DS1302 pin6) -> Arduino D7
 SCLK(DS1302 pin7) -> Arduino D8
 */
uint8_t CE_PIN   = 6;
uint8_t IO_PIN   = 7;
uint8_t SCLK_PIN = 8;
 
/* 日期变量缓存 */
char buf[50];
char day[10];
/* 串口数据缓存 */
String comdata = "";
int numdata[7] ={0}, j = 0, mark = 0;
/* 创建 DS1302 对象 */
DS1302 rtc(CE_PIN, IO_PIN, SCLK_PIN);
 
void print_time()
 {
     /* 从 DS1302 获取当前时间 */
     Time t = rtc.time();
     /* 将星期从数字转换为名称 */
   /*  memset(day, 0, sizeof(day));
     switch (t.day)
     {
     case 1: strcpy(day, "Sunday"); break;
     case 2: strcpy(day, "Monday"); break;
     case 3: strcpy(day, "Tuesday"); break;
     case 4: strcpy(day, "Wednesday"); break;
     case 5: strcpy(day, "Thursday"); break;
     case 6: strcpy(day, "Friday"); break;
     case 7: strcpy(day, "Saturday"); break;
     }*/
     /* 将日期代码格式化凑成buf等待输出 */
     snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", t.yr, t.mon, t.date, t.hr, t.min, t.sec);
     /* 输出日期到串口 */
     Serial.println(buf);
     
 }

void setup()
 {
     Serial.begin(9600);
     //dht.begin(); //DHT开始工作
     rtc.writeProtect(false);
     rtc.halt(false);
     
     lcd.begin(numCols,numRows);
  lcd.print("Starting in");//
  for(int i=3;i>0;i--)//从3倒数
  {
    //顶行是0行
    lcd.setCursor(12,0);
    lcd.print(i);
    delay(1000);
  }
 }
 
void loop()
 {

int columnWidth=4;//列的间隔
int displayColumns=3;//列的数量

// 两次检测之间，要等几秒钟，这个传感器有点慢。
  //delay(2000);
  // 读温度或湿度要用250毫秒
  //float h = dht.readHumidity();//读湿度
  //float t = dht.readTemperature();//读温度，默认为摄氏度
  int value=analogRead(inPin);
  float millivolts=(value/1024.0)*5000;
  float celsius=millivolts/10;
 // Serial.print("Humidity: ");//湿度
  //Serial.print(h);
  //Serial.println(" %");
  //Serial.print("Temperature: ");//温度
  //Serial.print(t);
  //Serial.println(" C");

  Serial.print(celsius);//
  Serial.println("degrees Celsius");
  Serial.print((celsius*9)/5+32);
  Serial.println("degrees Fahrenheit");
  float h=celsius;
  float t=(celsius*9)/5+32;
  

 
     /* 当串口有数据的时候，将数据拼接到变量comdata */
     while (Serial.available() > 0)
     {
         comdata += char(Serial.read());
         delay(2);
         mark = 1;
     }
     /* 以逗号分隔分解comdata的字符串，分解结果变成转换成数字到numdata[]数组 */
     if(mark == 1)
     {
         Serial.print("You inputed : ");
         Serial.println(comdata);
         for(int i = 0; i < comdata.length() ; i++)
         {
             if(comdata[i] == ',' || comdata[i] == 0x10 || comdata[i] == 0x13)
             {
                 j++;
             }
             else
             {
                 numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
             }
         }
         /* 将转换好的numdata凑成时间格式，写入DS1302 */
         Time t(numdata[0], numdata[1], numdata[2], numdata[3], numdata[4], numdata[5], numdata[6]);
         rtc.time(t);
         mark = 0;j=0;
         /* 清空 comdata 变量，以便等待下一次输入 */
         comdata = String("");
         /* 清空 numdata */
         for(int i = 0; i < 7 ; i++) numdata[i]=0;
     }
      
     /* 打印当前时间 */
     print_time();

     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print(buf);

     if(h<10)
     {
      lcd.setCursor(2,1);
      lcd.print((int)h);
     }
     else if(h<100)
     {
      lcd.setCursor(1,1);
      lcd.print((int)h);
     }
     else
     {
      lcd.setCursor(0,1);
      lcd.print((int)h);
     }
     lcd.setCursor(3,1);
     lcd.write(degreeSymbol);
     lcd.setCursor(4,1);
     lcd.print("C");
     
     if(t<10)
     {
      lcd.setCursor(9,1);
      lcd.print((int)t);
     }
     else if(t<100)
     {
      lcd.setCursor(8,1);
      lcd.print((int)t);
     }
     else
     {
      lcd.setCursor(7,1);
      lcd.print((int)h);
     }
     lcd.setCursor(10,1);
     lcd.write(degreeSymbol);
     lcd.setCursor(11,1);
     lcd.print("F");
     
      delay(1000);
 }
