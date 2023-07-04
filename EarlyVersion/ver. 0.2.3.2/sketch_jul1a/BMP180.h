#ifndef BMP180__H_
#define BMP180__H_
#include <Arduino.h>
#include <Wire.h>
#define BMP180ADD 0xEE>>1  // I2C address of BMP180  
                           //write is (0xEE)     read is (0xEF) 

                                 
extern unsigned char OSS;                            
/**********************MSB      LSB******/
extern int ac1;           // 0xAA     0xAB
extern int ac2;           // 0xAC     0xAD
extern int ac3;           // 0xAE     0xAE
extern unsigned int ac4;  // 0xB0     0xB1
extern unsigned int ac5;  // 0xB2     0xB3
extern unsigned int ac6;  // 0xB4     0xB5
extern int b1;            // 0xB6     0xB7
extern int b2;            // 0xB8     0xB9
extern int mb;            // 0xBA     0xBB
extern int mc;            // 0xBC     0xBD
extern int md;            // 0xBE     0xBF
extern float temperature;  
extern double pressure;   
extern double pressure2;
extern long b5;          
extern double altitude;


void calculate();
void show();
void BMP180start();
short bmp180GetTemperature(unsigned int ut);
long bmp180GetPressure(unsigned long up);
int bmp180Read(unsigned char address);
int bmp180ReadDate(unsigned char address);
unsigned int bmp180ReadUT();
unsigned long bmp180ReadUP();
float GetTemperatureData();
double GetPressureData();
double GetAltitudeData();
#endif
