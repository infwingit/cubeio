/*
 * utils.c
 *
 *  Created on: 2010-4-30
 *      Author: Administrator
 */
#include "utils.h"
#include "hal_eeprom.h"

//------------------------------------------
// Caculate Polynominal Value
// y=f(x)=coff[0]+coff[1]*x+coff[2]*x*x...
//------------------------------------------
float adcCalcPolynominalValue(float const* coff, int16_t coffSize, float n)
{
    int16_t i;
    float temp;
    float aN = (coff[coffSize - 1]);
    float aN_1 = (coff[coffSize - 2]);
    float aN_2 = 0;
    temp = aN * n + aN_1;
    for(i = coffSize - 3; i >= 0; i--)
    {
        aN_2 = coff[i];
        temp = temp * n + aN_2;
    }
    return temp;
}

double resolveLineTransform(double x1, double y1, double x2, double y2, double x)
{
    double k, b;
    k = (y2 - y1) / (x2 - x1);
    b = (y1 * x2 - y2 * x1) / (x2 - x1);
    return k * x + b;
}

uint16_t hex2Asc(uint8_t hex_data)
{
    uint16_t asc_data;
    uint8_t temp;
    if((temp = hex_data >> 4) > 9)
        temp += 55;
    else
        temp += 48;
    asc_data = temp * 256;
    if((temp = hex_data & 0xf) > 9)
        temp += 55;
    else
        temp += 48;
    asc_data += temp;
    return asc_data;
}

uint8_t asc2Hex(uint8_t asc_h, uint8_t asc_l)
{
    asc_h -= 0x30;
    if(asc_h > 10)
        asc_h -= 7;
    asc_l -= 0x30;
    if(asc_l > 10)
        asc_l -= 7;
    return asc_l | (asc_h << 4);
}

uint8_t hex2AscForDecimal(uint8_t hex_data)
{
    uint8_t temp;
    if((temp = hex_data & 0xf) > 9)
        temp += 55;
    else
        temp += 48;
    return temp;
}

float Celsius2Fahrenheit(float Celsius)
{
    //摄氏温度转化为华氏温度，转换公式为：f=c*9/5+32。
    float Fahrenheit = Celsius * 1.8 + 32;
    return Fahrenheit;
}

float Celsius2Kelvin(float Celsius)
{
    //摄氏温度转化为开氏温度，转换公式为：k=c+273.15。
    float Kelvin = Celsius + 273.15;
    return Kelvin;
}

void GetCpuID(uint32_t* pBuf)
{
    pBuf[0] = *(uint32_t*)(0x1FFFF7E8);
    pBuf[1] = *(uint32_t*)(0x1FFFF7EC);
    pBuf[2] = *(uint32_t*)(0x1FFFF7F0);
}

//#define EEPROM_CHECK 1

#if defined(EEPROM_CHECK)
uint32_t crc32EepromCheck(uint32_t* pStartAddress, uint32_t byteLengthWithCRC)
{
    uint8_t i;
    uint32_t crc;
    uint32_t buf[20];
    byteLengthWithCRC = byteLengthWithCRC / 4; // byte length to 4byte length

    LL_CRC_ResetCRCCalculationUnit(CRC);
    for(i = 0; i < byteLengthWithCRC; i++)
    {
        buf[i] = eeprom_readint32((__IO uint32_t*)pStartAddress + i);
        LL_CRC_FeedData32(CRC, buf[i]);
    }

    crc = LL_CRC_ReadData32(CRC);
    return crc;
}

void crc32EepromAppend(uint32_t* pStartAddress, uint32_t byteLengthWithCRC)
{
    uint8_t i;
    uint32_t crc;
    uint32_t buf[20];
    byteLengthWithCRC = byteLengthWithCRC / 4; // byte length to 4byte length
    byteLengthWithCRC = byteLengthWithCRC - 1;

    LL_CRC_ResetCRCCalculationUnit(CRC);
    for(i = 0; i < byteLengthWithCRC; i++)
    {
        buf[i] = eeprom_readint32((__IO uint32_t*)pStartAddress + i);
        LL_CRC_FeedData32(CRC, buf[i]);
    }
    crc = LL_CRC_ReadData32(CRC);
    eeprom_writeint32(pStartAddress + byteLengthWithCRC, crc);
}
#else
uint32_t crc32EepromCheck(uint32_t* pStartAddress, uint32_t byteLengthWithCRC)
{
    return 1;
}

void crc32EepromAppend(uint32_t* pStartAddress, uint32_t byteLengthWithCRC)
{
}
#endif

#define CRC8INIT	0x00
#define CRC8POLY	0x18              //0X18 = X^8+X^5+X^4+X^0
uint8_t	crc8(uint8_t* data_in, uint16_t number_of_bytes_to_read)
{
    uint8_t	 crc;
    uint16_t loop_count;
    uint8_t  bit_counter;
    uint8_t  data;
    uint8_t  feedback_bit;

    crc = CRC8INIT;

    for(loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++)
    {
        data = data_in[loop_count];

        bit_counter = 8;
        do
        {
            feedback_bit = (crc ^ data) & 0x01;

            if(feedback_bit == 0x01)
            {
                crc = crc ^ CRC8POLY;
            }
            crc = (crc >> 1) & 0x7F;
            if(feedback_bit == 0x01)
            {
                crc = crc | 0x80;
            }

            data = data >> 1;
            bit_counter--;

        }
        while(bit_counter > 0);
    }

    return crc;
}

uint16_t _crc16_update(uint16_t sum, uint8_t data)
{

    sum ^= data;

    for(int8_t luc = 0; luc < 8; luc++)
    {

        if(sum & 0x0001)
        {
            sum >>= 1;
            sum ^= 0xA001;
        }
        else
        {
            sum >>= 1;
        }
    }

    return sum;
}

uint16_t crc16Check(uint8_t* pStartAddress, uint8_t lengthWithCrc16)
{
    uint8_t i;
    uint16_t sum = 0xffff;
    for(i = 0; i < lengthWithCrc16; i++)
        sum = _crc16_update(sum, pStartAddress[i]);
    return sum;
}

//append crc16 info to a message frame
void crc16Append(uint8_t* pStartAddress, uint8_t dataLength)
{
    int16_t sum = 0xffff;
    uint8_t i;
    //create crc16 value
    for(i = 0; i < dataLength; i++)
    {
        sum = _crc16_update(sum, pStartAddress[i]);
    }
    pStartAddress[dataLength] = LO8_UINT(sum);
    pStartAddress[dataLength + 1] = HI8_UINT(sum);
}
