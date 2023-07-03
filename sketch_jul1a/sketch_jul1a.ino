#include <Servo.h>
#include <IRremote.h>
#include "Temperature_Sensor.h"
#include "TM1637.h"
#include "BMP180.h"
#include "DS1302.h"
#include "FTM1637.h"

#define Trig 15
#define Echo 14
#define Touch 2
#define Bell 3
#define PwmPin 5
#define DirPin 6
#define Infrared 7
#define RST 16
#define DAT 17
#define CLK 18
#define Green 19  //定义引脚D3为绿灯
#define Yellow 20  //定义引脚D5为黄灯
#define Red 21    //定义引脚D6为红灯

#define Rain A1
#define Light A2

#define IR_OFF 0xFFA25D
#define IR_MENU 0xFFE21D
#define IR_TEST 0xFF22DD
#define IR_UP 0xFF02FD
#define IR_QIEHUAN 0xFFC23D
#define IR_LEFT 0xFFE01F
#define IR_START 0xFFA857
#define IR_RIGHT 0xFF906F
#define IR_0 0xFF6897
#define IR_DOWN 0xFF9867
#define IR_C 0xFFB04F
#define IR_1 0xFF30CF
#define IR_2 0xFF18E7
#define IR_3 0xFF7A85
#define IR_4 0xFF10EF
#define IR_5 0xFF38C7
#define IR_6 0xFF5AA5
#define IR_7 0xFF42BD
#define IR_8 0xFF4AB5
#define IR_9 0xFF52AD
#define IR_FFFFFFFF 0xFFFFFFFF

DHT DHTData(A3, 11);
Servo servo_11;
Servo servo_13;
TM1637 tm1637(4,8);
DS1302 rtc(RST, DAT, CLK);
IRrecv irrecv(22);//定义红外引脚

int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
int secPre = -1;
int distance;
int sensorValue;//光敏传感器返回值
int lightValue;//亮度值
int rainValue;//雨水值
long IRValue = 0; //获得红外遥控键值

boolean ClockPoint = true;//数码管中间两个点,0关闭,1开启
byte servoAngle = 80;//舵机初始角度
decode_results results;

void FanControl(int speedValue)
{
  if (speedValue >= 0)
  {
    digitalWrite(DirPin, LOW);
    analogWrite(PwmPin, speedValue);
  }
  else
  {
    digitalWrite(DirPin, HIGH);
    analogWrite(PwmPin, speedValue+255);
  }
}

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
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
  // 输出日期到串口
  //Serial.println(buf);
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

void show()
{
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);                            //10 hexadecimal
  Serial.println("℃");
  Serial.print("Pressure: ");
  Serial.print(pressure, 0);                               //10 hexadecimal
  Serial.println("Pa");
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println("m");
}

void setup() {

  Serial.begin(9600);
  DHTData.begin();
  tm1637.set();//数码管初始化
  tm1637.init();
  tm1637.point(POINT_ON);
  //Wire.begin();
  //OSS = 2;  // Oversampling Setting  0: single    1: 2 times    2: 4 times   3: 8 times 
  //BMP180start();
  
  pinMode(Touch, INPUT);
  pinMode(Bell, OUTPUT);
  pinMode(PwmPin, OUTPUT);
  pinMode(DirPin, OUTPUT);
  pinMode(Infrared, INPUT);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(Green, OUTPUT);  //设置LED灯输出模式
  pinMode(Yellow, OUTPUT); //设置LED灯输出模式
  pinMode(Red, OUTPUT);    //设置LED灯输出模式
  
  servo_11.attach(11);
  servo_13.attach(13);//舵机初始化

  digitalWrite(PwmPin, LOW);//上电关闭风扇
  digitalWrite(DirPin, LOW);

  servo_11.write(90);
  servo_13.write(servoAngle);//舵机运行到初始角度

  rtc.writeProtect(false);
  rtc.halt(false);
  Time t(2022, 1, 1, 00, 00, 00, Time::kSaturday);  // 年月日时分秒 星期  第一次使用时设置
  rtc.time(t);

  irrecv.enableIRIn();

  
  //delay(20);
  //servo_13.detach();
}

void loop() {
  
  if (digitalRead(Touch) == HIGH)
  {
    tone(Bell, 1230);
    delay(500);
    tone(Bell, 680);
    delay(500);
    noTone(Bell);
  } 
  else {
    noTone(Bell);
  }

  float tValue = DHTData.readTemperature();
  //Serial.println(String("Temperature:") + String(tValue));
  //Serial.println(distance);
  if(tValue >= 28){//测得温度大于或等于28摄氏度
    FanControl(100); //风扇速度100
  }else{
    FanControl(0);//关闭风扇
  }
  
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  distance = (int)(pulseIn(Echo, HIGH) / 58.00);
  //tm1637.TimeUpdate(0,distance);
  if (digitalRead(Infrared) == LOW || distance < 10) {
    servo_11.write(0); //开门3秒
    delay(3000);
  } else {
    servo_11.write(90); //关门
  }

  printTime();//打印日期
  delay(1000);

  /*if (distance < 10) {
    servo_11.write(0);
    delay(3000);
  } else {
    servo_11.write(90);
  }*/

  //calculate();
  //show();
  //delay(1000);

  sensorValue = analogRead(Light);//获取光敏传感器返回值
  lightValue = map(sensorValue, 0, 1023, 1000, 0); //将获取光敏传感器返回值映射成亮度值
  rainValue =  analogRead(Rain);//雨水值
  //Serial.print("lightValue = ");
  //Serial.print(lightValue);
  //Serial.print("   rainValue = ");
  //Serial.println(rainValue);
  //tm1637.TimeUpdate(ClockPoint, lightValue); //数码管显示亮度值
  if (lightValue > 400 && rainValue < 300) {    //亮度值小于400（环境亮）的同时雨滴值小于300（没有雨）,开窗.
    while (servoAngle > 2) { //亮度值小于400.缓慢开窗
      servoAngle -= 1;
      servo_13.write(servoAngle);
      delay(20);
    }
  } else {            //缓慢关窗.有雨或者阳光不足时关窗
    while (servoAngle < 80) {
      servoAngle += 1;
      servo_13.write(servoAngle);
      delay(20);
    }
  }  

  if (irrecv.decode(&results)) {
    IRValue = results.value;
    //Serial.println(IRValue, HEX);
    if (IRValue == IR_1) {
      digitalWrite(Green, HIGH); //点亮绿灯
    } else if (IRValue == IR_4) {
      digitalWrite(Green, LOW);  //熄灭绿灯
    } else if (IRValue == IR_2) {
      digitalWrite(Yellow, HIGH);  //点亮黄灯
    } else if (IRValue == IR_5) {
      digitalWrite(Yellow, LOW);  //熄灭黄灯
    } else if (IRValue == IR_3) {
      digitalWrite(Red, HIGH);  //点亮红灯
    } else if (IRValue == IR_6) {
      digitalWrite(Red, LOW);  //熄灭红灯
    } else if (IRValue == IR_UP) {
      //servo_13.attach(13);
      while (servoAngle > 2) { //缓慢开窗
        servoAngle -= 1;
        servo_13.write(servoAngle);
        delay(20);
      }
      //servo_13.detach();
    } else if (IRValue == IR_DOWN) {
      servo_13.attach(13);
      while (servoAngle < 80) {
        servoAngle += 1;
        servo_13.write(servoAngle);
        delay(20);
      }
      //servo_13.detach();
    }
    irrecv.resume();
  }  
}
