#ifndef MUSIC_H_
#define MUSIC_H_
extern int tune[];
extern float durt[];
extern int length;
#include <stdio.h>
#include <Arduino.h>
extern int tone_val;
extern int durt_val;
#define tonePin  5   //蜂鸣器端口
void Music_Init();
void Play_Music();
void newtone(uint16_t frequency, uint32_t duration) ;
#define D0 -1
#define D1 262   //do re mi fa sol la xi
#define D2 293
#define D3 329
#define D4 349
#define D5 392
#define D6 440
#define D7 494
 
#define M1 523  //高八度的do re mi fa sol la xi
#define M2 586
#define M3 658
#define M4 697
#define M5 783
#define M6 879
#define M7 987
 
#define H1 1045  
#define H2 1171
#define H3 1316
#define H4 1393
#define H5 1563
#define H6 1755
#define H7 1971

#define WHOLE 1
#define HALF 0.5
#define QUARTER 0.25
#define EIGHTH 0.25
#define SIXTEENTH 0.625
#endif
