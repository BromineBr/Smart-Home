#include <Servo.h>
#include "DHT.h"

String item;

Servo servo_11;
Servo servo_12;

DHT dhtA3(A3, 11);

void setMotor8833(int speedpin,int dirpin, int speed)
{
  if (speed == 0)
  {
    digitalWrite(dirpin, LOW);
    analogWrite(speedpin, 0);
  }
  else if (speed > 0)
  {
    digitalWrite(dirpin, LOW);
    analogWrite(speedpin, speed);
  }
  else
  {
    digitalWrite(dirpin, HIGH);
    analogWrite(speedpin, -speed);
  }
}

void setup(){
  Serial.begin(9600);
  item = "";
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(47, OUTPUT);
  digitalWrite(46, LOW);
  digitalWrite(47, LOW);
  servo_11.attach(11);
  servo_12.attach(12);
  servo_11.write(90);
  servo_12.write(80);
  dhtA3.begin();
  
}

void loop(){
  Serial.println(String("Temp：") + String(String(dhtA3.readTemperature()) + String("℃")) + " " + String("Humi：") + String(String(dhtA3.readHumidity()) + String("%")) + " " + String("light：") + String(String(analogRead(A2)) + String("lx")));
  delay(500);
  if (Serial.available() > 0) {
    item = Serial.readStringUntil('.');
    if (String(item).equals(String("OPEN"))) {
      Serial.println("OPEN");
      // 打开连接D4号管脚的黄灯
      digitalWrite(43,HIGH);
      digitalWrite(44,HIGH);
      digitalWrite(45,HIGH);
    } 
    if (String(item).equals(String("CLOSE"))) {
      Serial.println("CLOSE");
      // 关闭连接D4号管脚的黄灯
      digitalWrite(43,LOW);
      digitalWrite(44,LOW);
      digitalWrite(45,LOW);
    }
    if (String(item).equals(String("OPEN Fan"))) {
      Serial.println("OPEN Fan");
      setMotor8833(46, 47, 100);
    } 
    if (String(item).equals(String("CLOSE Fan"))) {
      Serial.println("CLOSE Fan");
      setMotor8833(46, 47, 0);
    }
    if (String(item).equals(String("OPEN DOOR"))) {
      Serial.println("OPEN DOOR");
      servo_11.write(0);
      delay(20);
    } 
    if (String(item).equals(String("CLOSE DOOR"))) {
      Serial.println("CLOSE DOOR");
      servo_11.write(90);
      delay(20);
    }
    if (String(item).equals(String("OPEN Window"))) {
      Serial.println("OPEN Window");
      servo_12.write(0);
      delay(20);

    } 
    if (String(item).equals(String("CLOSE Window"))) {
      Serial.println("CLOSE Window");
      servo_12.write(80);
      delay(20);
    }
  }

}
