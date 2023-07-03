#include <Servo.h>
#include "Temperature_Sensor.h"
#include "TM1637.h"
#include "BMP180.h"

#define Trig 15
#define Echo 14
#define Touch 2
#define Bell 3
#define PwmPin 5
#define DirPin 6
#define Infrared 7

DHT DHTData(A3, 11);
Servo servo_D11;
TM1637 tm1637(4,8);

int distance;

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

void setup() {

  Serial.begin(9600);
  DHTData.begin();
  tm1637.set();//数码管初始化
  tm1637.init();
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
  
  servo_D11.attach(11);
  digitalWrite(PwmPin, LOW);//上电关闭风扇
  digitalWrite(DirPin, LOW);

  servo_D11.write(90);

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
  
  delay(300);
  
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  distance = (int)(pulseIn(Echo, HIGH) / 58.00);
  tm1637.TimeUpdate(0,distance);
  if (digitalRead(Infrared) == LOW || distance < 10) {
    servo_D11.write(0); //开门3秒
    delay(3000);
  } else {
    servo_D11.write(90); //关门
  }

  /*if (distance < 10) {
    servo_D11.write(0);
    delay(3000);
  } else {
    servo_D11.write(90);
  }*/

  //calculate();
  //show();
  //delay(1000);
}
