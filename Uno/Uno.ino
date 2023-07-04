#include "DS1302.h"
#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#include "TM1637.h"
#include "Temperature_Sensor.h"
LiquidCrystal_I2C lcd(0x27,16,2); 
const int RST   = 2;  // Chip Enable
const int DAT   = 3;  // Input/Output
const int CLK = 4;  // Serial Clock

const int PIN_L9110S = 9;  // L9110S电机驱动模块引脚

// 创建 DS1302 对象
DS1302 rtc(RST, DAT, CLK);
int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};
int secPre = -1;
boolean ClockPoint = true;//数码管中间两个点,0关闭,1开启
TM1637 tm1637(7, 8);
DHT DHTData(A3, 11);

String month;
String hour;
String minute;
String second;
String dayAsString(const Time::Day day) {
  switch (day) { 
    case Time::kSunday: return "Sun";
    case Time::kMonday: return "Mon";
    case Time::kTuesday: return "Tue";
    case Time::kWednesday: return "Wed";
    case Time::kThursday: return "Thu";
    case Time::kFriday: return "Fri";
    case Time::kSaturday: return "Sat";
  }
  return "(unknown day)";
}

void printTime() {
  // 从芯片中获取当前时间和日期。
  Time t = rtc.time();

  //将星期从数字转换为名称
  const String day = dayAsString(t.day);

  // 将日期代码格式化凑成buf等待输出 
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
  if(t.mon<10){month = String("0")+String(t.mon);}else{month = String(t.mon);}//判断月份是否满10，否则前面加0占位
  if(t.date<10){day = String("0")+String(t.date);}else{day = String(t.date);}//判断日是否满10，否则前面加0占位
  if(t.hr<10){hour = String("0")+String(t.hr);}else{hour = String(t.hr);}//判断小时是否满10，否则前面加0占位
  if(t.min<10){minute = String("0")+String(t.min);}else{minute = String(t.min);}//判断分钟是否满10，否则前面加0占位
  if(t.sec<10){second = String("0")+String(t.sec);}else{second = String(t.sec);}//判断秒是否满10，否则前面加0占位
  // 输出日期到串口
  // Serial.println(hour);
  lcd.setCursor(0, 0);
  lcd.print(" " + String(t.yr) + "-" +  month + "-"+  day + " " + dayAsString(t.day));
  //lcd.setCursor(0, 1);
  //lcd.print(String("Time ") +  hour + ":" +  minute + ":"+  second);

  Serial.println(buf);
  if (secPre != t.sec) {
    if (ClockPoint) {
      tm1637.point(POINT_ON);
      ClockPoint = false;
    }
    else {
      tm1637.point(POINT_OFF);
      ClockPoint = true;
    }
  }
  TimeDisp[0] = t.hr / 10;     //小时的十位
  TimeDisp[1] = t.hr % 10; //小时的个位
  TimeDisp[2] = t.min / 10;   //分钟的十位
  TimeDisp[3] = t.min % 10;       //分钟的个位
  tm1637.showPosition(4);  //显示四位数
  tm1637.display(TimeDisp); //数码管显示
  secPre = t.sec;
}



void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  tm1637.set();//数码管初始化
  tm1637.init();
  tm1637.point(POINT_ON);
  DHTData.begin();
  pinMode(PIN_L9110S,OUTPUT);
  digitalWrite(PIN_L9110S,LOW);
  Serial.println("start");
  
// 设置时间后, 需要注释掉下面四行设置时间的代码,并重新烧录一次. 以免掉电重新执行setup中的时间设置函数.
  //rtc.writeProtect(false);
  //rtc.halt(false);
  //Time t(2023, 7, 4, 17, 37, 30, Time::kTuesday);  // 年月日时分秒 星期  第一次使用时设置
  //rtc.time(t);
  
}

// Loop and print the time every second.
void loop() {
  printTime();//打印日期
  lcd.setCursor(0, 1);
  lcd.print(String("T:") + String(DHTData.readTemperature())+"  ");
  //lcd.setCursor(0, 1);
  lcd.print(String("H:") + String(DHTData.readHumidity()));
  delay(100);
}
