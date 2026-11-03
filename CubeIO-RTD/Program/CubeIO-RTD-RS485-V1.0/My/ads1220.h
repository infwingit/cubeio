#ifndef __ADS1220_H__
#define __ADS1220_H__

#include "main.h"

typedef enum EnumAds1220Command
{
    ADS1220_RESET = 0x06,
    ADS1220_START = 0x08,
    ADS1220_PWRDOWN = 0x02,
    ADS1220_RDATA = 0x10,
    ADS1220_WREG = 0x40,
    ADS1220_RREG = 0x20,
} EnumAds1220Command;

typedef enum EnumAds1220Register
{
    ADS1220_CONF_REG_0 = 0x00,
    ADS1220_CONF_REG_1 = 0x01,
    ADS1220_CONF_REG_2 = 0x02,
    ADS1220_CONF_REG_3 = 0x03,
} EnumAds1220Register;

typedef enum EnumAds1220Mux
{
    ADS1220_MUX_0_1     = 0x00,   //default
    ADS1220_MUX_0_2     = 0x10,
    ADS1220_MUX_0_3     = 0x20,
    ADS1220_MUX_1_2     = 0x30,
    ADS1220_MUX_1_3     = 0x40,
    ADS1220_MUX_2_3     = 0x50,
    ADS1220_MUX_1_0     = 0x60,
    ADS1220_MUX_3_2     = 0x70,
    ADS1220_MUX_0_AVSS  = 0x80,
    ADS1220_MUX_1_AVSS  = 0x90,
    ADS1220_MUX_2_AVSS  = 0xA0,
    ADS1220_MUX_3_AVSS  = 0xB0,
    ADS1220_MUX_REFPX_REFNX_4 = 0xC0,
    ADS1220_MUX_AVDD_M_AVSS_4 = 0xD0,
    ADS1220_MUX_AVDD_P_AVSS_2 = 0xE0
} EnumAds1220Mux;

typedef enum EnumAds1220Gain
{
    ADS1220_GAIN_1   = 0x00,   //default
    ADS1220_GAIN_2   = 0x02,
    ADS1220_GAIN_4   = 0x04,
    ADS1220_GAIN_8   = 0x06,
    ADS1220_GAIN_16  = 0x08,
    ADS1220_GAIN_32  = 0x0A,
    ADS1220_GAIN_64  = 0x0C,
    ADS1220_GAIN_128 = 0x0E
} EnumAds1220Gain;

typedef enum EnumAds1220BypassPGA
{
    ADS1220_BYPASSPGA_ENABLE   = 0x00,   //default
    ADS1220_BYPASSPGA_DISABLE  = 0x01,
} EnumAds1220BypassPGA;

typedef enum EnumAds1220DataRateNormal
{
    ADS1220_DR_NORMAL_20 = 0x00,   // default
    ADS1220_DR_NORMAL_45 = 0x20,
    ADS1220_DR_NORMAL_90 = 0x40,
    ADS1220_DR_NORMAL_175 = 0x60,
    ADS1220_DR_NORMAL_330 = 0x80,
    ADS1220_DR_NORMAL_600 = 0xA0,
    ADS1220_DR_NORMAL_1000 = 0xC0
} EnumAds1220DataRateNormal;

typedef enum EnumAds1220DataRateDutyCycle
{
    ADS1220_DR_DUTYCYCLE_5 = 0x00,   // default
    ADS1220_DR_DUTYCYCLE_11p25 = 0x20,
    ADS1220_DR_DUTYCYCLE_22p5 = 0x40,
    ADS1220_DR_DUTYCYCLE_44 = 0x60,
    ADS1220_DR_DUTYCYCLE_82p5 = 0x80,
    ADS1220_DR_DUTYCYCLE_150 = 0xA0,
    ADS1220_DR_DUTYCYCLE_250 = 0xC0
} EnumAds1220DataRateDutyCycle;

typedef enum EnumAds1220DataRateTurbo
{
    ADS1220_DR_TURBO_40 = 0x00,   // default
    ADS1220_DR_TURBO_90 = 0x20,
    ADS1220_DR_TURBO_180 = 0x40,
    ADS1220_DR_TURBO_350 = 0x60,
    ADS1220_DR_TURBO_660 = 0x80,
    ADS1220_DR_TURBO_1200 = 0xA0,
    ADS1220_DR_TURBO_2000 = 0xC0
} EnumAds1220DataRateTurbo;

typedef enum EnumAds1220OpMode
{
    ADS1220_NORMAL_MODE     = 0x00,  // default
    ADS1220_DUTY_CYCLE_MODE = 0x08,
    ADS1220_TURBO_MODE      = 0x10
} EnumAds1220OpMode;

typedef enum EnumAds1220ConvMode
{
    ADS1220_SINGLE_SHOT     = 0x00,  // default
    ADS1220_CONTINUOUS      = 0x04
} EnumAds1220ConvMode;

typedef enum EnumAds1220TempSensorEn
{
    ADS1220_TEMPSENSOR_DISABLE     = 0x00,  // default
    ADS1220_TEMPSENSOR_ENABLE      = 0x02
} EnumAds1220TempSensorEn;

typedef enum EnumAds1220BurnOutCurrentEn
{
    ADS1220_BURNOUTCURRENT_DISABLE     = 0x00,  // default
    ADS1220_BURNOUTCURRENT_ENABLE      = 0x01
} EnumAds1220BurnOutCurrentEn;

typedef enum EnumAds1220Vref
{
    ADS1220_VREF_INT            = 0x00,  // default
    ADS1220_VREF_REFP0_REFN0    = 0x40,
    ADS1220_VREF_REFP1_REFN1    = 0x80,
    ADS1220_VREF_AVDD_AVSS      = 0xC0
} EnumAds1220Vref;

typedef enum EnumAds1220FIR
{
    ADS1220_NONE        = 0x00,   // default
    ADS1220_50HZ_60HZ   = 0x10,
    ADS1220_50HZ        = 0x20,
    ADS1220_60HZ        = 0x30
} EnumAds1220FIR;

typedef enum EnumAds1220PSW
{
    ADS1220_ALWAYS_OPEN = 0x00,  // default
    ADS1220_SWITCH      = 0x08
} EnumAds1220PSW;

typedef enum EnumAds1220IdacCurrent
{
    ADS1220_IDAC_OFF        = 0x00,  // defaulr
    ADS1220_IDAC_10_MU_A    = 0x01,
    ADS1220_IDAC_50_MU_A    = 0x02,
    ADS1220_IDAC_100_MU_A   = 0x03,
    ADS1220_IDAC_250_MU_A   = 0x04,
    ADS1220_IDAC_500_MU_A   = 0x05,
    ADS1220_IDAC_1000_MU_A  = 0x06,
    ADS1220_IDAC_1500_MU_A  = 0x07
} EnumAds1220IdacCurrent;

typedef enum EnumAds1220IdacRouting
{
    ADS1220_IDAC_NONE       = 0x00,  // default
    ADS1220_IDAC_AIN0_REFP1 = 0x01,
    ADS1220_IDAC_AIN1       = 0x02,
    ADS1220_IDAC_AIN2       = 0x03,
    ADS1220_IDAC_AIN3_REFN1 = 0x04,
    ADS1220_IDAC_REFP0      = 0x05,
    ADS1220_IDAC_REFN0      = 0x06,
} EnumAds1220IdacRouting;

typedef enum EnumAds1220DrdyMode
{
    ADS1220_DRDY      = 0x00,   // default
    ADS1220_DOUT_DRDY = 0x02
} EnumAds1220DrdyMode;


#define RAWADC_BUFFER_INITIAL_VALUE     	0x7FFFFFFF //2147483647
#define RAWADC_BUFFER_SIZE 								128  // 数据缓冲区大小
#define RAWADC_WINDOW_SIZE 								10   // 滑动窗口大小（最近的N个数据）

int16_t ADS1220_Init(uint8_t gain, uint8_t dataRate);
int16_t ADS1220_CheckHealth(void);
int16_t ADS1220_Deinit(void);
void ADS1220_Start(void);
void ADS1220_Reset(void);
void ADS1220_PowerDown(void);
int16_t ADS1220_ReadRawAd_ByRData_CheckingPinDoutDrdy(uint8_t* pIsConversionDone, int32_t* pData);

//Reg 0
void ADS1220_SetReg0_Mux_Gain_BypassPGA(EnumAds1220Mux mux, EnumAds1220Gain gain, EnumAds1220BypassPGA bypassPGA);
void ADS1220_SetReg0Mux(EnumAds1220Mux mux);
void ADS1220_SetReg0Gain(EnumAds1220Gain gain);
void ADS1220_SetReg0BypassPGA(uint8_t bypassPGA);

//Reg 1
void ADS1220_SetReg1_DR_OpMode_ConvMode_EnTempSensor_EnBurnOutCurrent(EnumAds1220DataRateNormal dataRate, EnumAds1220OpMode opMode, EnumAds1220ConvMode convMode, EnumAds1220TempSensorEn tempSensorEn, EnumAds1220BurnOutCurrentEn burnOutCurrentEn);
void ADS1220_SetReg1DataRate(EnumAds1220DataRateNormal dataRate);
void ADS1220_SetReg1OperatingMode(EnumAds1220OpMode opMode);
void ADS1220_SetReg1ConversionMode(EnumAds1220ConvMode convMode);
void ADS1220_SetReg1EnableTemperatureSensor(EnumAds1220TempSensorEn tempSensorEn);
void ADS1220_SetReg1EnableBurnOutCurrentSources(EnumAds1220BurnOutCurrentEn burnOutCurrentEn);

//Reg 2
void ADS1220_SetReg2_VRef_Fir_Psw_Idac(EnumAds1220Vref vRefSource, EnumAds1220FIR fir, EnumAds1220PSW psw, EnumAds1220IdacCurrent current);
void ADS1220_SetReg2VRefSource(EnumAds1220Vref vRefSource);
void ADS1220_SetReg2FIRFilter(EnumAds1220FIR fir);
void ADS1220_SetReg2LowSidePowerSwitch(EnumAds1220PSW psw);
void ADS1220_SetReg2IdacCurrent(EnumAds1220IdacCurrent current);

//Reg 3
void ADS1220_SetReg3_I1Mux_I2Mux_DrdyMode(EnumAds1220IdacRouting i1Mux, EnumAds1220IdacRouting i2Mux, EnumAds1220DrdyMode drdyMode);
void ADS1220_SetReg3Idac1Routing(EnumAds1220IdacRouting i1Mux);
void ADS1220_SetReg3Idac2Routing(EnumAds1220IdacRouting i2Mux);
void ADS1220_SetReg3DrdyMode(EnumAds1220DrdyMode drdyMode);

#endif
