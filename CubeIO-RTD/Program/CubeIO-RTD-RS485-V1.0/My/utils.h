#include "main.h"

#ifndef UTILS_H_
#define UTILS_H_

//----------------------------------------
//Basic Data Types
//----------------------------------------
#define TRUE  0x01
#define FALSE 0x00

typedef union
{
    uint16_t uint16Value;
    uint8_t uint8Value[sizeof(uint16_t) / sizeof(uint8_t)];
} UNION_INT_VALUE;

typedef union
{
    float floatValue;
    uint32_t uint32Value;
    uint16_t uint16Value[sizeof(uint16_t) / sizeof(uint8_t)];
    uint8_t uint8Value[sizeof(uint32_t) / sizeof(uint8_t)];
} UNION_LONG_VALUE;

//----------------------------------------
//Bit Operation
//----------------------------------------
#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define is_bit_set(p,m) (((p) & (m))==0?0:1)
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

//----------------------------------------
//Commons
//----------------------------------------
#define   LO8_UINT(ui_digit)              ((uint8_t)((ui_digit)& 0x00ff))         //得到一个16位数字的低8位
#define   HI8_UINT(ui_digit)              ((uint8_t)((ui_digit) >> 8))            //得到一个16位数字的高8位

#define   LO16_UINT32(ui32_digit)         ((uint16_t)((ui32_digit)& 0x0000ffff))         //得到一个32位数字的低16位
#define   HI16_UINT32(ui32_digit)         ((uint16_t)(((ui32_digit)>>16)&0x0000ffff))    //得到一个32位数字的高16位

#define   MAKE_UINT(uchHi,uchLo)          (((uchHi) << 8) | (uchLo))		 //将两个8位合成一个16位数
#define   MAKE_LONG(x, y)                 ((((uint32_t)((x) & 0x0000FFFF)) << 16) | ((uint32_t)((y) & 0x0000FFFF)))

#define   MAKE_16BIT(hi8,lo8)             (((hi8) << 8) | (lo8))
#define   MAKE_32BIT(hi16,lo16)           ((((uint32_t)((hi16) & 0x0000FFFF)) << 16) | ((uint32_t)((lo16) & 0x0000FFFF)))

#define   LO_UCHAR(uc_digit)              (uint8_t)((uc_digit)&0x0f)           //得到一个8位数的低4位
#define   HI_UCHAR(uc_digit)              (uint8_t)((uc_digit)>>4)             //得到一个8位数的高4位

//----------------------------------------
//Functions
//----------------------------------------
float adcCalcPolynominalValue(float const* coff, int16_t coffSize, float n);
double resolveLineTransform(double x1, double y1, double x2, double y2, double x);
void GetCpuID(uint32_t* pBuf);
float Celsius2Fahrenheit(float Celsius);
float Celsius2Kelvin(float Celsius);

//EEPROM & RAM R/W
uint32_t crc32EepromCheck(uint32_t* pStartAddress, uint32_t byteLengthWithCRC);
void crc32EepromAppend(uint32_t* pStartAddress, uint32_t byteLengthWithCRC);
uint8_t	crc8(uint8_t* data_in, uint16_t number_of_bytes_to_read);
uint16_t crc16Check(uint8_t* pStartAddress, uint8_t lengthWithCrc16);
void crc16Append(uint8_t* pStartAddress, uint8_t dataLength);
uint16_t _crc16_update(uint16_t sum, uint8_t data);

#endif /* UTILS_H_ */
