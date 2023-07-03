#include "Music.h"
//列出所有节拍
int tune[]=        //根据简谱列出各频率
{
  M3,M3,M4,M5,
  M5,M4,M3,M2,
  M1,M1,M2,M3,
  M3,M2,M2,
  M3,M3,M4,M5,
  M5,M4,M3,M2,
  M1,M1,M2,M3,
  M2,M1,M1,
  M2,M2,M3,M1,
  M2,M3,M4,M3,M1,
  M2,M3,M4,M3,M2,
  M1,M2,D5,D0,
  M3,M3,M4,M5,
  M5,M4,M3,M4,M2,
  M1,M1,M2,M3,
  M2,M1,M1
};
float durt[]=       //根据简谱列出各节拍、音乐上为八分音符，四分音符之类的，这里是每个音的延时
{
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1+0.5,0.5,1+1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1+0.5,0.5,1+1,
  1,1,1,1,
  1,0.5,0.5,1,1,
  1,0.5,0.5,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,0.5,0.5,
  1,1,1,1,
  1+0.5,0.5,1+1,
};

int length = sizeof(tune)/sizeof(tune[0]);

int tone_val = 440;
int durt_val = 1;

void Music_Init()
{
  // pinMode(tonePin,OUTPUT);
   //length = sizeof(tune)/sizeof(tune[0]);//这里用了一个sizeof函数，可以查出tone序列里有多少个音符
}
void Play_Music()
{            
   pinMode(tonePin,OUTPUT);   
   for(int x=0;x<length;x++)
   {
     newtone(tune[x],durt[x]*500);
   }
   pinMode(tonePin,INPUT);
}


void newtone(uint16_t frequency, uint32_t duration) {
  int period = 1000000L / frequency;
  int pulse = period / 2;
  for (long i = 0; i < duration * 1000L; i += period) {
    digitalWrite(tonePin, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(tonePin, LOW);
    delayMicroseconds(pulse);
  }
    delay(20);
}
