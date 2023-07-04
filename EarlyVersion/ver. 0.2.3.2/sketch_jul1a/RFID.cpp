#include "RFID.h"

int chipSelectPin = 10;    //SDA引脚片选引脚
int NRSTPD = 5;
 
/*
 * Function：ShowCardID
 * Description：显示卡的ID
 * Input parameter：ID string
 * Return：Null
 */
void ShowCardID(uchar *id)
{
    int IDlen=4;
    for(int i=0; i<IDlen; i++){
        Serial.print(0x0F & (id[i]>>4), HEX);
        Serial.print(0x0F & id[i],HEX);
    }
    Serial.println("");
}

/*
 * Function：ShowCardType
 * Description：显示卡类型
 * Input parameter：Type string
 * Return：Null
 */
void ShowCardType(uchar* type)
{
    Serial.print("Card type: ");
    if(type[0]==0x04&&type[1]==0x00)
        Serial.println("MFOne-S50");
    else if(type[0]==0x02&&type[1]==0x00)
        Serial.println("MFOne-S70");
    else if(type[0]==0x44&&type[1]==0x00)
        Serial.println("MF-UltraLight");
    else if(type[0]==0x08&&type[1]==0x00)
        Serial.println("MF-Pro");
    else if(type[0]==0x44&&type[1]==0x03)
        Serial.println("MF Desire");
    else
        Serial.println("Unknown");
}

/*
 * Function：Write_MFRC5200
 * Description：写一个字节的数据到一个登记的MR RC522
 * Input parameter：addr--register address；val--the value that need to write in
 * Return：Null
 */
void Write_MFRC522(uchar addr, uchar val)
{
    digitalWrite(chipSelectPin, LOW);

    //address format：0XXXXXX0
    SPI.transfer((addr<<1)&0x7E);
    SPI.transfer(val);

    digitalWrite(chipSelectPin, HIGH);
}


/*
 * Function：Read_MFRC522
 * Description：读一个字节的数据到一个登记的MR RC522
 * Input parameter：addr--register address
 * Return：return the read value
 */
uchar Read_MFRC522(uchar addr)
{
    uchar val;

    digitalWrite(chipSelectPin, LOW);

    //地址形式：1XXXXXX0
    SPI.transfer(((addr<<1)&0x7E) | 0x80);
    val =SPI.transfer(0x00);

    digitalWrite(chipSelectPin, HIGH);

    return val;
}

/*
 * Function：SetBitMask
 * Description：设置RC522寄存器位
 * Input parameter：reg--register address;mask--value
 * Return：null
 */
void SetBitMask(uchar reg, uchar mask)
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask); //设置掩码位
}


/*
 * Function：ClearBitMask
 * Description：清除RC522寄存器位
 * Input parameter：reg--register address;mask--value
 * Return：null
 */
void ClearBitMask(uchar reg, uchar mask)
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask)); //清除掩码
}

/*
 * Function：AntennaOn
 * Description：打开天线,每一次打开的天线所需要至少1ms
 * Input parameter：null
 * Return：null
 */
void AntennaOn(void)
{
    uchar temp;

    temp = Read_MFRC522(TxControlReg);
    if (!(temp & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


/*
 * Function：AntennaOff
 * Description：关闭天线,每次关闭天线至少需要1ms
 * Input parameter：null
 * Return：null
 */
void AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

/*
 * Function：ResetMFRC522
 * Description：重置RC522
 * Input parameter：null
 * Return：null
 */
void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);        //重置RC522
}

/*
 * Function：InitMFRC522
 * Description：初始化RC522
 * Input parameter：null
 * Return：null
 */
void MFRC522_Init(void)
{
    digitalWrite(NRSTPD,HIGH);                //复位引脚

    MFRC522_Reset();                //重置RC522

    //定时器: TPrescaler*TreloadVal/6.78MHz = 24ms
    Write_MFRC522(TModeReg, 0x8D);            //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E);        //TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);
    Write_MFRC522(TReloadRegH, 0);

    Write_MFRC522(TxAutoReg, 0x40);            //100%ASK
    Write_MFRC522(ModeReg, 0x3D);            //CRC效验值0x6363 ???

    //ClearBitMask(Status2Reg, 0x08); //MFCrypto1On=0
    //Write_MFRC522(RxSelReg, 0x86); //RxWait = RxSelReg[5..0]
    //Write_MFRC522(RFCfgReg, 0x7F); //RxGain = 48dB

    AntennaOn();                    //打开天线
}

/*
 * Function：MFRC522_Request
 * Description：搜寻卡,读取卡的类型
 * Input parameter：reqMode--搜寻方法，
 *     TagType--返回卡的类型
 *     0x4400 = Mifare_UltraLight
 *     0x0400 = Mifare_One(S50)
 *     0x0200 = Mifare_One(S70)
 *     0x0800 = Mifare_Pro(X)
 *     0x4403 = Mifare_DESFire
 * return：return MI_OK if successed
 */
uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
    uchar status;
    uint backBits;                                //接收到的数据比特

    Write_MFRC522(BitFramingReg, 0x07);         //TxLastBists = BitFramingReg[2..0] ???

    TagType[0] = reqMode;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

    if ((status != MI_OK) || (backBits != 0x10))
    {
        status = MI_ERR;
    }

    return status;
}


/*
 * Function：MFRC522_ToCard
 * Description：RC522和ISO14443之间通信
 * Input parameter：command--MF522 command bits
 * sendData--send data to card via rc522
 * sendLen--send data length
 * backData--the return data from card
 * backLen--the length of return data
 * return：return MI_OK if successed
 */
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR;
    uchar irqEn = 0x00;
    uchar waitIRq = 0x00;
    uchar lastBits;
    uchar n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT:            //密码验证
        {
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        }
        case PCD_TRANSCEIVE:         //在FIFO发送数据
        {
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        }
        default:
            break;
    }

    Write_MFRC522(CommIEnReg, irqEn|0x80);     //允许中断
    ClearBitMask(CommIrqReg, 0x80);            //清除所有的中断标志位
    SetBitMask(FIFOLevelReg, 0x80);         //FlushBuffer=1, FIFO 初始化大部分

    Write_MFRC522(CommandReg, PCD_IDLE); //没有行动;取消现在的命令

    //把数据持续写入FIFO
    for (i=0; i<sendLen; i++)
    {
        Write_MFRC522(FIFODataReg, sendData[i]);
    }

    //进行下面
    Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80);     //StartSend=1,开始传送数据
    }

    //等待接收数据完成
    i = 2000;         //i 应根据时钟调整,等待的时间最大应该是25毫秒
    do
    {
        //CommIrqReg[7..0]
        //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80); //StartSend=0

    if (i != 0)
    {
        if(!(Read_MFRC522(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR; //??
            }

            if (command == PCD_TRANSCEIVE)
            {
                n = Read_MFRC522(FIFOLevelReg);
                lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {
                    *backLen = (n-1)*8 + lastBits;
                }
                else
                {
                    *backLen = n*8;
                }

                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAX_LEN)
                {
                    n = MAX_LEN;
                }

                //读取FIFO数据
                for (i=0; i<n; i++)
                {
                    backData[i] = Read_MFRC522(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }

    }

    //SetBitMask(ControlReg,0x80);            //关闭定时器
    //Write_MFRC522(CommandReg, PCD_IDLE);

    return status;
}


/*
 * Function：MFRC522_Anticoll
 * Description：防冲撞,读取卡的连续数据
 * Input parameter：serNum--return the 4 bytes card serial number, the 5th byte is recheck byte
 * return：return MI_OK if successed
 */
uchar MFRC522_Anticoll(uchar *serNum)
{
    uchar status;
    uchar i;
    uchar serNumCheck=0;
    uint unLen;

    //ClearBitMask(Status2Reg, 0x08);     //strSensclear
    //ClearBitMask(CollReg,0x80);         //ValuesAfterColl
    Write_MFRC522(BitFramingReg, 0x00); //TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
        //验证卡的连续数据
        for (i=0; i<4; i++)
        {
            serNumCheck ^= serNum[i];
        }
        if (serNumCheck != serNum[i])
        {
            status = MI_ERR;
        }
    }

    //SetBitMask(CollReg, 0x80);        //ValuesAfterColl=1

    return status;
}


/*
 * Function：CalulateCRC
 * Description：使用mf522计算CRC
 * Input parameter：pIndata--the CRC data need to be read，len--data length，pOutData-- the caculated result of CRC
 * return：Null
 */
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;

    ClearBitMask(DivIrqReg, 0x04);         //CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);        //清除FIFO指针
    //Write_MFRC522(CommandReg, PCD_IDLE);

    //Write data into FIFO
    for (i=0; i<len; i++)
    {
        Write_MFRC522(FIFODataReg, *(pIndata+i));
    }
    Write_MFRC522(CommandReg, PCD_CALCCRC);

    //等待CRC计算完成
    i = 0xFF;
    do
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04)); //CRCIrq = 1

    //读出CRC校验结果
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}



/*
 * Function：MFRC522_Write
 * Description：写数据块
 * Input parameters：blockAddr--block address;writeData--Write 16 bytes data into block
 * return：return MI_OK if successed
 */
uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
    uchar status;
    uint recvBits;
    uchar i;
    uchar buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        for (i=0; i<16; i++) //16字节的数据写入到FIFO
        {
            buff[i] = *(writeData+i);
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

        if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}


/*
 * Function：MFRC522_Halt
 * Description：命令卡进入睡眠模式
 * Input parameters：null
 * return：null
 */
void MFRC522_Halt(void)
{
    uchar status;
    uint unLen;
    uchar buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);

    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}
