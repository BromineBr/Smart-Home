#ifndef RFID__H_
#define RFID__H_
#include <SPI.h>
extern int chipSelectPin ;    //SDA引脚片选引脚
extern int NRSTPD;
//MF522 命令位
#define PCD_IDLE 0x00             //取消当前命令不行动
#define PCD_AUTHENT 0x0E         //验证密码的钥匙
#define PCD_RECEIVE 0x08         //接收数据
#define PCD_TRANSMIT 0x04        //发送数据
#define PCD_TRANSCEIVE 0x0C        //发送和接收数据
#define PCD_RESETPHASE 0x0F        //重置
#define PCD_CALCCRC 0x03        //CRC校验和的计算

//Mifare_One卡的命令位
#define PICC_REQIDL 0x26        //在天线区域搜索不进入睡眠模式的卡
#define PICC_REQALL 0x52         //搜索天线区域中的所有卡
#define PICC_ANTICOLL 0x93        //防止冲突
#define PICC_SElECTTAG 0x93     //选择卡
#define PICC_AUTHENT1A 0x60     //验证A密码密钥
#define PICC_AUTHENT1B 0x61     //验证B密码密钥
#define PICC_READ 0x30            //读
#define PICC_WRITE 0xA0            //写
#define PICC_DECREMENT 0xC0     //扣除值
#define PICC_INCREMENT 0xC1     //装载值
#define PICC_RESTORE 0xC2         //还原数据到缓冲区
#define PICC_TRANSFER 0xB0        //保存数据到缓冲区
#define PICC_HALT 0x50             //睡眠模式


//通信时MF522的返回值
#define MI_OK 0
#define MI_NOTAGERR 1
#define MI_ERR 2

//------------------MFRC522 注册 ---------------
//Page 0:命令和状态
#define Reserved00 0x00             //保留将来之用
#define CommandReg 0x01             //启动和停止命令的执行
#define CommIEnReg 0x02             //中断请求传递的使能和禁能控制位。
#define DivlEnReg 0x03                 //中断请求传递的使能和禁能控制位。
#define CommIrqReg 0x04             //包含中断请求标志
#define DivIrqReg 0x05                //包含中断请求标志
#define ErrorReg 0x06                 //错误标志，指示执行行的上个命令的错误状态
#define Status1Reg 0x07             //包含通信的状态标志
#define Status2Reg 0x08             //包含接收器和发射器的状态标志
#define FIFODataReg 0x09            //64字节FIFO缓冲器的输入输出
#define FIFOLevelReg 0x0A            //指示FIFO中存储的字节数
#define WaterLevelReg 0x0B            //定义FIFO下溢和上溢报警的FIFO深度。
#define ControlReg 0x0C                //不同的控制寄存器
#define BitFramingReg 0x0D            //面向位的帧调节
#define CollReg 0x0E                //RF接口上检测到的第一个位冲突的位的位置
#define Reserved01 0x0F                //保留将来之用
//Page 1:控制
#define Reserved10 0x10                //保留将来之用
#define ModeReg 0x11                //定义发射和接收的常用模式
#define TxModeReg 0x12                //定义发射过程的数据传输速率
#define RxModeReg 0x13                //定义接收过程中的数据传输速率
#define TxControlReg 0x14            //控制天线驱动管脚TX1和TX2的逻辑特征
#define TxAutoReg 0x15                //控制天线驱动的设置
#define TxSelReg 0x16                //选择天线驱动器的内部源
#define RxSelReg 0x17                //选着内部的接收器设置
#define RxThresholdReg 0x18            //选择位译码器的阀值
#define DemodReg 0x19                //定义解调器的设置
#define Reserved11 0x1A                //保留将来之用
#define Reserved12 0x1B                //保留将来之用
#define MifareReg 0x1C                //控制ISO 14443/MIFARE 模式106kbit/s的通信
#define Reserved13 0x1D                //保留将来之用
#define Reserved14 0x1E                //保留将来之用
#define SerialSpeedReg 0x1F            //选择串行UART接口的速率
//Page 2:发生器
#define Reserved20 0x20                //保留将来之用
#define CRCResultRegM 0x21            //显示CRC计算的实际MSB和LSB值（MSB）
#define CRCResultRegL 0x22            //显示CRC计算的实际MSB和LSB值（LSB）
#define Reserved21 0x23                //保留将来之用
#define ModWidthReg 0x24            //控制ModWidth的设置
#define Reserved22 0x25                //保留将来之用
#define RFCfgReg 0x26                //配置接受器增益
#define GsNReg 0x27                    //选择天线驱动器管脚TX1和TX2的调制电导
#define CWGsPReg 0x28                //选择天线驱动器管脚TX1和TX2的调制电导
#define ModGsPReg 0x29                //选择天线驱动器管脚TX1和TX2的调制电导
#define TModeReg 0x2A                //定义内部定时器的设置A
#define TPrescalerReg 0x2B            //定义内部定时器的设置B
#define TReloadRegH 0x2C            //描述16位长的定时器重装值（C）
#define TReloadRegL 0x2D            //描述16位长的定时器重装值（D）
#define TCounterValueRegH 0x2E        //显示16位长的实际定时器值（E）
#define TCounterValueRegL 0x2F        //显示16位长的实际定时器值（F）
//Page 3:记录
#define Reserved30 0x30                //保留将来之用
#define TestSel1Reg 0x31            //常用测试信号的配置
#define TestSel2Reg 0x32            //常用测试信号的配置和PRBS控制
#define TestPinEnReg 0x33            //D1—D7输出驱动器的使能管脚（注：仅用于串行接口）
#define TestPinValueReg 0x34        //定义D1-D7用做I/O总线的值
#define TestBusReg 0x35                //显示内部测试总线的状态
#define AutoTestReg 0x36            //控制数字自测试
#define VersionReg 0x37                //显示版本
#define AnalogTestReg 0x38            //控制管脚AUX1和AUX2
#define TestDAC1Reg 0x39             //定义 TestDAC1的测试值
#define TestDAC2Reg 0x3A             //定义 TestDAC2的测试值
#define TestADCReg 0x3B             //显示 ADC I和 Q通道的实际值
#define Reserved31 0x3C             //保留用于产品测试
#define Reserved32 0x3D             //保留用于产品测试
#define Reserved33 0x3E             //保留用于产品测试
#define Reserved34 0x3F                //保留用于产品测试
//-----------------------------------------------
#define uchar unsigned char
#define uint unsigned int
//数据数组的最大长度
#define MAX_LEN 16

void MFRC522_Halt(void);
uchar MFRC522_Write(uchar blockAddr, uchar *writeData);
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData);
uchar MFRC522_Anticoll(uchar *serNum);
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen);
uchar MFRC522_Request(uchar reqMode, uchar *TagType);
void MFRC522_Init(void);
void MFRC522_Reset(void);
void AntennaOff(void);
void AntennaOn(void);
void ClearBitMask(uchar reg, uchar mask);
void SetBitMask(uchar reg, uchar mask);
uchar Read_MFRC522(uchar addr);
void Write_MFRC522(uchar addr, uchar val);
void ShowCardType(uchar* type);
void ShowCardID(uchar *id);
#endif 
