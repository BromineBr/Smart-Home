#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <Arduino.h>
#define KEY_MAX 17
typedef struct
{
  char keyname;
  uint16_t keycode;
} StructMap;

typedef enum {
  MatrixCode_1 = 0,
  MatrixCode_2,
  MatrixCode_3,
  MatrixCode_4,
  MatrixCode_5,
  MatrixCode_6,
  MatrixCode_7,
  MatrixCode_8,
  MatrixCode_9,
  MatrixCode_0,
  MatrixCode_A,
  MatrixCode_B,
  MatrixCode_C,
  MatrixCode_D,
  MatrixCode_Xing,
  MatrixCode_Jin,
  MatrixCode_Null,
} MatrixKeyCode;
extern StructMap keyMap[];

class keyBoard {
  private:
    uint8_t SCLPin, SDOPin;
  public:
    void keyBoardInit(uint8_t SclPin, uint8_t SdoPin);
    uint16_t GetKeyCode(void);
    char GetKeyMap(void);
    uint16_t GetKey(void);
};
#endif  /* _KEYMAY_H_ */
