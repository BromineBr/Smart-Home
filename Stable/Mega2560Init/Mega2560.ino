#include <Servo.h>
#include "FTM1637.h"
#include <IRremote.h>

#define Trig 15
#define Echo 14
#define Touch 2
#define Bell 3
#define Infrared 7
#define windowPin 13 //定义窗舵机引脚
#define Green 19  //定义引脚D3为绿灯
#define Yellow 20  //定义引脚D5为黄灯
#define Red 21    //定义引脚D6为红灯
#define pwmPin 46 //D5引脚风扇控制速度大小
#define dirPin 47 //D6引脚风扇控制方向

/*#define IR_OFF 0xFFA25D
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
#define IR_FFFFFFFF 0xFFFFFFFF*/

#define rainSensor  A1//定义雨滴传感器引脚
#define sensor A2


byte servoAngle = 80;//舵机初始角度
int sensorValue;//光敏传感器返回值
int lightValue;//亮度值
int rainValue;//雨水值

//long IRValue = 0; //获得红外遥控键值
//IRrecv irrecv(23);//定义红外引脚
//decode_results results;


Servo servo_11;
Servo servo_13;

int distance;


void setup() {
  Serial.begin(9600);
  pinMode(Touch, INPUT);
  pinMode(Bell, OUTPUT);
  pinMode(Infrared, INPUT);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(pwmPin, OUTPUT); //风扇引脚输出模式
  pinMode(dirPin, OUTPUT);

  servo_11.attach(11);
  servo_11.write(90);

  digitalWrite(pwmPin, LOW);//上电关闭风扇
  digitalWrite(dirPin, LOW);

  /*pinMode(Green, OUTPUT);  //设置LED灯输出模式
  pinMode(Yellow, OUTPUT); //设置LED灯输出模式
  pinMode(Red, OUTPUT);    //设置LED灯输出模式
  irrecv.enableIRIn();*/
  servo_13.attach(13);//舵机初始化
  servo_13.write(servoAngle);//舵机运行到初始角度
  /*delay(1000);
  Serial.println("reset");
  servo_13.detach();//关闭舵机*/

  
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

  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  distance = (int)(pulseIn(Echo, HIGH) / 58.00);
  if (digitalRead(Infrared) == LOW || distance < 10) {
    servo_11.write(0); //开门3秒
    delay(3000);
  } else {
    servo_11.write(90); //关门
  }

  sensorValue = analogRead(sensor);//获取光敏传感器返回值
  lightValue = map(sensorValue, 0, 1023, 1000, 0); //将获取光敏传感器返回值映射成亮度值
  rainValue =  analogRead(rainSensor);//雨水值
  Serial.print("lightValue = ");
  Serial.println(lightValue);
  Serial.print("rainValue = ");
  Serial.println(rainValue);
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
  
  /*if (irrecv.decode(&results)) {
    IRValue = results.value;
    Serial.println(IRValue, HEX);
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
      servo_13.attach(13);
      while (servoAngle > 2) { //缓慢开窗
        servoAngle -= 1;
        servo_13.write(servoAngle);
        delay(20);
      }
      servo_13.detach();
    } else if (IRValue == IR_DOWN) {
      servo_13.attach(13);
      while (servoAngle < 80) {
        servoAngle += 1;
        servo_13.write(servoAngle);
        delay(20);
      }
      servo_13.detach();
    }
    irrecv.resume();
  }*/
  
  delay(500);
  
}
