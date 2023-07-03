#include <Servo.h>
#include <IRremote.h>
#include <Wire.h>
#include "Temperature_Sensor.h"
#include "TM1637.h"
#include "BMP180.h"
#include "DS1302.h"
#include "FTM1637.h"
#include "LiquidCrystal_I2C.h"
#include "KEYBOARD.h"
#include "RFID.h"
#include "Music.h"

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
#define flamePin 22  //火焰传感器
#define SCLPin 23 //键盘引脚
#define SDOPin 24 //键盘引脚
#define chipSelectPin 25
#define PIN_L9110S 26 //L9110S电机驱动模块引脚

#define Rain A1
#define Light A2
#define gasPin A4  //气体传感器

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
TM1637 tm1637(4,8);
DS1302 rtc(RST, DAT, CLK);
IRrecv irrecv(22);//定义红外引脚
LiquidCrystal_I2C lcd(0x27, 16, 2);

int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
int secPre = -1;
int distance;
int sensorValue;//光敏传感器返回值
int lightValue;//亮度值
int rainValue;//雨水值
long IRValue = 0; //获得红外遥控键值
int gas = 0; //气体模拟值
int flame = 0; //火焰数字量 高电平或低电平
int timing[]={0, 1, 10};
int atiming[]={0, 0, 30};

boolean ClockPoint = true;//数码管中间两个点,0关闭,1开启
byte servoAngle = 80;//舵机初始角度
decode_results results;
String month;
String day;
String hour;
String minute;
String second;

boolean inputDone = false; //输入密码是否结束标志
char ch = ' '; //按下的按键值
byte count = 0; //输入的密码第几位
char password[] = {'3', '6', '9'}; //设置正确密码
char inputCode[] = {' ', ' ', ' '}; //用户输入的密码
keyBoard keybo;

byte state_pre;
//4字节序列号卡，5字节字节是核准
uchar serNum[5];

Servo servo_11;
Servo servo_12;
Servo servo_13;

void FanControl(int speedValue) {
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

void show() {
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

void openTheDoor() {
  while (servoAngle < 180) {
    servoAngle += 2;
    servo_12.write(servoAngle);
    delay(50);
  }
  delay(2000);
  while (servoAngle > 90) { //缓慢开门，两秒后，关门
    servoAngle -= 2;
    servo_12.write(servoAngle);
    delay(50);
  }
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
  lcd.init();
  lcd.backlight();
  MFRC522_Init();
  
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
  pinMode(flamePin, INPUT);
  pinMode(chipSelectPin, OUTPUT); 
  pinMode(PIN_L9110S,OUTPUT);
  
  servo_11.attach(11);
  servo_13.attach(13);//舵机初始化

  digitalWrite(PwmPin, LOW);//上电关闭风扇
  digitalWrite(DirPin, LOW);
  digitalWrite(chipSelectPin, LOW);
  digitalWrite(PIN_L9110S,LOW);

  servo_11.write(90);
  servo_13.write(servoAngle);//舵机运行到初始角度

  // 设置时间后, 需要注释掉下面四行设置时间的代码,并重新烧录一次. 以免掉电重新执行setup中的时间设置函数.
  //rtc.writeProtect(false);
  //rtc.halt(false);
  //Time t(2022, 1, 1, 00, 00, 00, Time::kSaturday);  // 年月日时分秒 星期  第一次使用时设置
  //rtc.time(t);

  irrecv.enableIRIn();

  keybo.keyBoardInit(SCLPin, SDOPin);//键盘初始化
  servo_12.attach(12);//舵机初始化
  servo_12.write(servoAngle);//舵机运行到初始角度
  //delay(20);
  //Serial.print("Please input a three character password：");//请用户输入三位数的密码

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

  /*float tValue = DHTData.readTemperature();
  Serial.println(String("Temperature:") + String(tValue));
  Serial.println(distance);
  if(tValue >= 28){//测得温度大于或等于28摄氏度
    FanControl(100); //风扇速度100
  }else{
    FanControl(0);//关闭风扇
  }*/
  
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

  /*sensorValue = analogRead(Light);//获取光敏传感器返回值
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

  gas = analogRead(gasPin); // 把连接在A4接口的气体传感器的模拟值赋给gas变量
  flame = digitalRead(flamePin); // 把连接在D22号接口的火焰传感器的值赋给flame变量
  //Serial.print(String("gas:") + String(gas));
  //Serial.print(",");
  //Serial.println(String("flame:") + String(flame));
  //delay(200);
  // 判断烟雾模拟值是否大于500或者火焰传感器是否感应到火焰
  if (gas > 500 || flame == LOW) {
    FanControl(100); // 打开风扇，风扇速度100
    // 蜂鸣器报警
    tone(Bell, 146);
    delay(800);
    tone(Bell, 293);
    delay(800);
  } else {
    FanControl(0);//关闭风扇
    noTone(3);//关闭蜂鸣器
  }  
  
  lcd.setCursor(0, 0);
  lcd.print(String("Temp:") + String(DHTData.readTemperature()));
  lcd.setCursor(0, 1);
  lcd.print(String("Humi:") + String(DHTData.readHumidity()));
  if (DHTData.readTemperature() > 35 || DHTData.readHumidity() > 80) {
    FanControl(100); //风扇速度值100
  } else {
    FanControl(0);//关闭风扇
  }

  ch = keybo.GetKeyMap(); //检索用户按下的键值
  if (ch != ' ') {
    count++;
    if ( count == 1 ) {
      //Serial.print(ch); //用户输入的第一位
      inputCode[0] = ch;
    } else if ( count == 2 ) {
      //Serial.print(ch); //用户输入的第二位
      inputCode[1] = ch;
    } else if ( count == 3 ) {
      //Serial.println(ch);//用户输入的第三位
      inputCode[2] = ch;
      count = 0;
      inputDone = true; //密码输入完成
    }
  }
  if (inputDone) {
    inputDone = false;
    if ((inputCode[0] ==  password[0]) && (inputCode[1] ==  password[1]) && (inputCode[2] ==  password[2]) ) {   //密码输入正确
      //Serial.println("Welcome to my home");
      openTheDoor();
    } else {
      //Serial.println("Wrong password");   //密码输入错误
      tone(Bell, 293);
      delay(300);
    }
    inputCode[0] = ' ';   //清空用户输入的密码
    inputCode[1] = ' ';
    inputCode[2] = ' ';
    //Serial.println(" ");
    //Serial.print("Please input a three character password：");//继续下一次输入
  }

  uchar status;
  uchar str[MAX_LEN];
  //搜索卡，返回卡类型
  status = MFRC522_Request(PICC_REQIDL, str);
  if (status == MI_OK && state_pre != MI_OK) {
    //查看卡的类型
    //ShowCardType(str);
    //防止冲突返回网卡的4字节序列号
    status = MFRC522_Anticoll(str);
    // str[0..3]: 卡片序列号
    // str[4]: XOR checksum of the SN.(SN的异或校验。)
    if (status == MI_OK) {
      Serial.print("The card's number is: ");
      memcpy(serNum, str, 5);
      ShowCardID(serNum);
      //卡的相关身份验证
      uchar* id = serNum;
      //卡的id需要自己修改下面的if esle判断里的具体id
      //IC Key:2C073F31
      if ( id[0] == 0x2C && id[1] == 0x07 && id[2] == 0x3F && id[3] == 0x31) {
        Serial.println("IC Key!"); openTheDoor();
      } else if (id[0] == 0xA3 && id[1] == 0x00 && id[2] == 0xD8 && id[3] == 0x18) {
        //IC White Card:A300D818
        Serial.println("IC White Card!"); openTheDoor();
      } else {
        Serial.println("unkown !");
      }
    }
  }
  state_pre = status;
  MFRC522_Halt(); //命令卡进入休眠模式

  if(t.hr==timing[0] && t.min==timing[1] &&t.sec==timing[2] ){
    //Serial.println("watering");
    //lcd.clear();
    //lcd.setCursor(0, 0);
    //lcd.print("Watering");
    //lcd.setCursor(0, 1);
    //lcd.print(" flowers");
    digitalWrite(PIN_L9110S,HIGH);
    delay(3000);
    digitalWrite(PIN_L9110S,LOW);
  }

  if(t.hr==atiming[0] && t.min==atiming[1] &&t.sec==atiming[2] ){
    //Serial.println("The alarm clock rings.tering");
    //lcd.clear();
    //lcd.setCursor(0, 0);
    //lcd.print("The alarm");
    //lcd.setCursor(0, 1);
    //lcd.print("clock rings.");
    Play_Music();
  }*/
}
