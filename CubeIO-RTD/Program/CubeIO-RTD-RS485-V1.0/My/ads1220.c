#include "globals.h"
#include "hal_gpio.h"
#include "hal_spi.h"
#include "ads1220.h"
#include "delay_utils.h"
#include "sensor.h"

static uint8_t ADS1220_ReadRegister(EnumAds1220Register reg);
static void ADS1220_WriteRegister(EnumAds1220Register reg, uint8_t val);

static uint8_t _ads1220_dataRate = ADS1220_DR_NORMAL_20;

int16_t ADS1220_Init(uint8_t gain, uint8_t dataRate)
{
    int16_t error = 0;
    ADS1220_Reset();

    _ads1220_dataRate = dataRate;

    //配置寄存器
    uint8_t reg0_write = ADS1220_MUX_1_2 | gain | ADS1220_BYPASSPGA_ENABLE;
    ADS1220_WriteRegister(ADS1220_CONF_REG_0, reg0_write);
    uint8_t reg0_read = ADS1220_ReadRegister(ADS1220_CONF_REG_0);

    uint8_t reg1_write = dataRate | ADS1220_NORMAL_MODE | ADS1220_CONTINUOUS | ADS1220_TEMPSENSOR_DISABLE | ADS1220_BURNOUTCURRENT_DISABLE;
    ADS1220_WriteRegister(ADS1220_CONF_REG_1, reg1_write);
    uint8_t reg1_read = ADS1220_ReadRegister(ADS1220_CONF_REG_1);

    uint8_t reg2_write = ADS1220_VREF_REFP1_REFN1 | ADS1220_50HZ_60HZ | ADS1220_SWITCH | ADS1220_IDAC_OFF;
    ADS1220_WriteRegister(ADS1220_CONF_REG_2, reg2_write);
    uint8_t reg2_read = ADS1220_ReadRegister(ADS1220_CONF_REG_2);

    uint8_t reg3_write = ADS1220_IDAC_NONE | ADS1220_IDAC_NONE | ADS1220_DOUT_DRDY ;
    ADS1220_WriteRegister(ADS1220_CONF_REG_3, reg3_write);
    uint8_t reg3_read = ADS1220_ReadRegister(ADS1220_CONF_REG_3);

    if((reg0_write == reg0_read) && (reg1_write == reg1_read) && (reg2_write == reg2_read) && (reg3_write == reg3_read))
    {
        //配置正确
        error = 0;
    }
    else
    {
        //配置错误
        error += 1;
    }

    return error;
}

int16_t ADS1220_CheckHealth(void)
{
    int16_t error = 0;

    uint8_t reg2_write = ADS1220_VREF_REFP1_REFN1 | ADS1220_50HZ_60HZ | ADS1220_SWITCH | ADS1220_IDAC_OFF;
    uint8_t reg2_read = ADS1220_ReadRegister(ADS1220_CONF_REG_2);

    uint8_t reg3_write = ADS1220_IDAC_NONE | ADS1220_IDAC_NONE | ADS1220_DOUT_DRDY ;
    uint8_t reg3_read = ADS1220_ReadRegister(ADS1220_CONF_REG_3);

    if((reg2_write == reg2_read) && (reg3_write == reg3_read))
    {
        //配置正确
        error = 0;
    }
    else
    {
        //配置错误
        error += 1;
    }

    return error;
}

int16_t ADS1220_Deinit(void)
{
    int16_t error = 0;
    ADS1220_PowerDown();
    return error;
}

static uint8_t ADS1220_ReadRegister(EnumAds1220Register reg)
{
    uint8_t regValue = 0;

    SPI_CS_LOW();
    SPI_Soft_SwapByte(ADS1220_RREG | (reg << 2));
    regValue = SPI_Soft_SwapByte(0x00);
    SPI_CS_HIGH();

    return regValue;
}

static void ADS1220_WriteRegister(EnumAds1220Register reg, uint8_t val)
{
    SPI_CS_LOW();
    SPI_Soft_SwapByte(ADS1220_WREG | (reg << 2));
    SPI_Soft_SwapByte(val);
    SPI_CS_HIGH();
}

void ADS1220_Command(EnumAds1220Command cmd)
{
    SPI_CS_LOW();
    SPI_Soft_SwapByte(cmd);
    SPI_CS_HIGH();
}

//In single-shot mode, the START/SYNC command is used to start a single conversion, or (when sent during an
//ongoing conversion) to reset the digital filter, and then restarts a single new conversion. When the device is set
//to continuous conversion mode, the START/SYNC command must be issued one time to start converting
//continuously. Sending the START/SYNC command while converting in continuous conversion mode resets the
//digital filter and restarts continuous conversions.
//发送时如果AD正在转换，则重置digital filter并重新启动转换

//ADS1220的启动时间（内部4.096MHZ振荡器）
//手册8.3.5 Output Data Rate
//The first conversion starts 210*t(CLK) (normal mode, duty-cycle mode) or 114 · t(CLK) (turbo mode) after the last SCLK falling edge of the START/SYNC command.
//连续模式转换时间：NORMAL MODE, 204768*t(CLK) @20 SPS
//单次模式转换时间：NORMAL MODE, 204850*t(CLK) @20 SPS
//连续模式启动时间：210*t(CLK) + 204768*t(CLK) = 204768/4096000 = 0.04999 = 50ms
//单次模式启动时间：210*t(CLK) + 204850*t(CLK) = 204850/4096000 = 0.05001 = 50ms
void ADS1220_Start(void)
{
    ADS1220_Command(ADS1220_START);
    _delay_ms(70);
}

//ADS1220的复位时间（内部4.096MHZ振荡器）
//手册8.5.3.1 RESET (0000 011x)
//Resets the device to the default values. Wait at least (50 μs + 32*t(CLK)) after the RESET command is sent before sending any other command.
//复位时间：50 μs + 32*t(CLK)  = 50us + 32/4096000 = 50us + 7.8125us = 57.8125us
void ADS1220_Reset(void)
{
    ADS1220_Command(ADS1220_RESET);
    _delay_ms(1);
}

//The POWERDOWN command places the device into power-down mode. This command shuts down all internal
//analog components, opens the low-side switch, turns off both IDACs, but holds all register values. In case the
//POWERDOWN command is issued while a conversion is ongoing, the conversion completes before the
//ADS1220 enters power-down mode. As soon as a START/SYNC command is issued, all analog components
//return to their previous states.
//进入低功耗后，自动关闭所有模拟部件，打开low-side开关，关闭IDAC，并保持寄存器不变
//使用START/SYNC退出低功耗后，按寄存器配置自动设置为原来的设置。
void ADS1220_PowerDown(void)
{
    ADS1220_Command(ADS1220_PWRDOWN);
}

//8.5.1.6 SPI Timeout
//The ADS1220 offers an SPI timeout feature that can be used to recover communication when a serial interface
//transmission is interrupted. This feature is especially useful in applications where CS is permanently tied low and
//is not used to frame a communication sequence. Whenever a complete command is not sent within 13955 ·
//t(MOD) (normal mode, duty-cycle mode) or 27910 · t(MOD) (turbo mode), the serial interface resets and the next
//SCLK pulse starts a new communication cycle. See the Modulator section for details on the modulator frequency
//(f(MOD) = 1 / t(MOD)) in the different operating modes. For the RREG and WREG commands, a complete command
//includes the command byte itself plus the register bytes that are read or written.
//SPI总线错误处理函数，这里f(MOD)对于NormalMode来说是256KHz，13955/256KHz = 54.5ms
void ADS1220_SpiTimeout()
{
    //60ms后SPI总线由ADS1220复位
    _delay_ms(60);
}

//ADS1220 datasheet 8.5.6 Interfacing with Multiple Devices
int16_t ADS1220_ReadRawAd_ByRData_CheckingPinDoutDrdy(uint8_t* pIsConversionDone, int32_t* pData)
{
    int16_t errorCode = 0;
    uint32_t data = 0;
    uint8_t buf[3];

    SPI_CS_LOW();
    _delay_us(20);
    uint32_t spiDoutDrdyPin = SPI_MISO_READ();//CS变低后检查引脚状态，此引脚标志转换是否完成
    if(spiDoutDrdyPin != 0)
    {
        //高电平表示转换未完成
        *pIsConversionDone = 0;
    }
    else
    {
        //低电平表示转换完成
        *pIsConversionDone = 1;
        buf[0] = SPI_Soft_SwapByte(0x00);
        buf[1] = SPI_Soft_SwapByte(0x00);
        buf[2] = SPI_Soft_SwapByte(0x00);
        //To make sure DOUT/DRDY is taken high, send 8 additional SCLKs with DIN held low after each data read operation.
        //这下面必须多发一个字节，让DOUT线变成高电平，以等待下次CS拉低时进行转换完成的检测
        SPI_Soft_SwapByte(0x00);
        SPI_CS_HIGH();

        data = 0;
        data = (data << 8) | buf[0];
        data = (data << 8) | buf[1];
        data = (data << 8) | buf[2];
        *pData = data;
    }
    SPI_CS_HIGH();

    return errorCode;
}

//此函数应在连续转换下被调用
int16_t ADS1220_ReadAd(int32_t* pAdValue)
{
    int16_t errorCode = 0;
    int32_t rawAD = 0;
    uint8_t isConversionDone = 0;
    uint8_t timeOutCounter = 100;

    while((timeOutCounter--) > 0)
    {
        _delay_ms(1);
        errorCode += ADS1220_ReadRawAd_ByRData_CheckingPinDoutDrdy(&isConversionDone, &rawAD);
        if(isConversionDone != 0)
        {
            break;
        }
    }

    if(timeOutCounter == 0)
    {
        errorCode = 1;
    }

    if(errorCode == 0)
    {
        *pAdValue = rawAD;
    }
    return errorCode;
}

//ADS1220 Reg 0
void ADS1220_SetReg0_Mux_Gain_BypassPGA(EnumAds1220Mux mux, EnumAds1220Gain gain, EnumAds1220BypassPGA bypassPGA)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_0);
    regValue = mux | gain | bypassPGA;
    ADS1220_WriteRegister(ADS1220_CONF_REG_0, regValue);
}

void ADS1220_SetReg0Mux(EnumAds1220Mux mux)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_0);
    regValue &= ~0xF0;
    regValue |= mux;
    ADS1220_WriteRegister(ADS1220_CONF_REG_0, regValue);
}

void ADS1220_SetReg0Gain(EnumAds1220Gain gain)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_0);
    regValue &= ~0x0E;
    regValue |= gain;
    ADS1220_WriteRegister(ADS1220_CONF_REG_0, regValue);
}

void ADS1220_SetReg0BypassPGA(uint8_t bypassPGA)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_0);
    regValue &= ~0x01;
    regValue |= bypassPGA;
    ADS1220_WriteRegister(ADS1220_CONF_REG_0, regValue);
}

//ADS1220 Reg 1
void ADS1220_SetReg1_DR_OpMode_ConvMode_EnTempSensor_EnBurnOutCurrent(EnumAds1220DataRateNormal dataRate, EnumAds1220OpMode opMode, EnumAds1220ConvMode convMode, EnumAds1220TempSensorEn tempSensorEn, EnumAds1220BurnOutCurrentEn burnOutCurrentEn)
{
    uint8_t regValue;
    regValue = dataRate | opMode | convMode | tempSensorEn | burnOutCurrentEn;
    ADS1220_WriteRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS1220_SetReg1DataRate(EnumAds1220DataRateNormal dataRate)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_1);
    regValue &= ~0xE0;
    regValue |= dataRate;
    ADS1220_WriteRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS1220_SetReg1OperatingMode(EnumAds1220OpMode opMode)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_1);
    regValue &= ~0x18;
    regValue |= opMode;
    ADS1220_WriteRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS1220_SetReg1ConversionMode(EnumAds1220ConvMode convMode)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_1);
    regValue &= ~0x04;
    regValue |= convMode;
    ADS1220_WriteRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS1220_SetReg1EnableTemperatureSensor(EnumAds1220TempSensorEn tempSensorEn)
{
    uint8_t regValue = _ads1220_dataRate | ADS1220_NORMAL_MODE | ADS1220_CONTINUOUS | tempSensorEn | ADS1220_BURNOUTCURRENT_DISABLE;
    ADS1220_WriteRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS1220_SetReg1EnableBurnOutCurrentSources(EnumAds1220BurnOutCurrentEn burnOutCurrentEn)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_1);
    regValue &= ~0x01;
    regValue |= burnOutCurrentEn;
    ADS1220_WriteRegister(ADS1220_CONF_REG_1, regValue);
}

//ADS1220 Reg 2
void ADS1220_SetReg2_VRef_Fir_Psw_Idac(EnumAds1220Vref vRefSource, EnumAds1220FIR fir, EnumAds1220PSW psw, EnumAds1220IdacCurrent current)
{
    uint8_t regValue;
    regValue = vRefSource | fir | psw | current;
    ADS1220_WriteRegister(ADS1220_CONF_REG_2, regValue);
}

void ADS1220_SetReg2VRefSource(EnumAds1220Vref vRefSource)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_2);
    regValue &= ~0xC0;
    regValue |= vRefSource;
    ADS1220_WriteRegister(ADS1220_CONF_REG_2, regValue);
}

void ADS1220_SetReg2FIRFilter(EnumAds1220FIR fir)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_2);
    regValue &= ~0x30;
    regValue |= fir;
    ADS1220_WriteRegister(ADS1220_CONF_REG_2, regValue);
}

void ADS1220_SetReg2LowSidePowerSwitch(EnumAds1220PSW psw)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_2);
    regValue &= ~0x08;
    regValue |= psw;
    ADS1220_WriteRegister(ADS1220_CONF_REG_2, regValue);
}

void ADS1220_SetReg2IdacCurrent(EnumAds1220IdacCurrent current)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_2);
    regValue &= ~0x07;
    regValue |= current;
    ADS1220_WriteRegister(ADS1220_CONF_REG_2, regValue);
    _delay_ms(200);
}

//ADS1220 Reg 3
void ADS1220_SetReg3_I1Mux_I2Mux_DrdyMode(EnumAds1220IdacRouting i1Mux, EnumAds1220IdacRouting i2Mux, EnumAds1220DrdyMode drdyMode)
{
    uint8_t regValue;
    regValue = i1Mux | i2Mux | drdyMode;
    ADS1220_WriteRegister(ADS1220_CONF_REG_3, regValue);
}

void ADS1220_SetReg3Idac1Routing(EnumAds1220IdacRouting i1Mux)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_3);
    regValue &= ~0xE0;
    regValue |= (i1Mux << 5);
    ADS1220_WriteRegister(ADS1220_CONF_REG_3, regValue);
}

void ADS1220_SetReg3Idac2Routing(EnumAds1220IdacRouting i2Mux)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_3);
    regValue &= ~0x1C;
    regValue |= (i2Mux << 2);
    ADS1220_WriteRegister(ADS1220_CONF_REG_3, regValue);
}

void ADS1220_SetReg3DrdyMode(EnumAds1220DrdyMode drdyMode)
{
    uint8_t regValue;
    regValue = ADS1220_ReadRegister(ADS1220_CONF_REG_3);
    regValue &= ~0x02;
    regValue |= drdyMode;
    ADS1220_WriteRegister(ADS1220_CONF_REG_3, regValue);
}
