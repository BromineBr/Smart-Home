#include "KEYBOARD.h"

StructMap keyMap[17] = {
  {'1', 0x0001},
  {'2', 0x0002},
  {'3', 0x0004},
  {'4', 0x0008},
  {'5', 0x0010},
  {'6', 0x0020},
  {'7', 0x0040},
  {'8', 0x0080},
  {'9', 0x0100},
  {'A', 0x0200},
  {'B', 0x0400},
  {'C', 0x0800},
  {'D', 0x1000},
  {'E', 0x2000},
  {'F', 0x4000},
  {'G', 0x8000},
  {' ', 0}
};

void keyBoard::keyBoardInit(uint8_t SclPin, uint8_t SdoPin) {
  SCLPin = SclPin;
  SDOPin = SdoPin;
  pinMode(SCLPin , OUTPUT);
  pinMode(SDOPin , INPUT);
}

uint16_t keyBoard::GetKeyCode(void) {
  unsigned int DATA = 0;
  pinMode(SDOPin , OUTPUT);
  digitalWrite(SDOPin, HIGH);
  delayMicroseconds(93);
  digitalWrite(SDOPin, LOW);
  delayMicroseconds(10);
  pinMode(SDOPin , INPUT);
  for (int i = 0; i < 16; i++)
  {
    digitalWrite(SCLPin, HIGH);
    digitalWrite(SCLPin, LOW);
    DATA |= digitalRead(SDOPin) << i;
  }
  delay(4);
  return DATA & 0xFFFF;
}
char keyBoard::GetKeyMap(void) {
  byte i;
  uint16_t keycode = GetKeyCode();
  //Serial.println  (keycode);
  StructMap *irkeymap = keyMap;
  for (i = 0; i < KEY_MAX; i++) {
    //    Serial.println(irkeymap[i].keycode);
    if (irkeymap[i].keycode == keycode)
      return irkeymap[i].keyname;
  }
  return ' ';
}

uint16_t keyBoard::GetKey(void) {
  byte i;
  uint16_t keycode = GetKeyCode();
  StructMap *irkeymap = keyMap;
  for (i = 0; i < KEY_MAX; i++) {
    //    Serial.println(irkeymap[i].keycode);
    if (irkeymap[i].keycode == keycode)
      return i;
  }
  return 0xff;
}
