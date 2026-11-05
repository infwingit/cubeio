#include <stddef.h>
#include "modbus_common.h"
#include "modbus_reg.h"
#include "usart.h"
#include "modbus.h"
#include "utils.h"
#include "main.h"
#include "eeprom.h"
#include "hal_eeprom.h"
#include "utils.h"
#include "globals.h"
#include "sensor.h"
#include "sensor_static_data.h"

//Define a function pointer to process param checking
typedef int16_t (*PFUNC_VALIDATE_COMM_PARAM)(uint8_t);
PFUNC_VALIDATE_COMM_PARAM g_pfnArrayCommParamValidate[] =
{
    validateCommParamSlaveAddress,
    validateCommParamBaudrate,
    validateCommParamProtocol,
    validateCommParamParity,
    validateCommParamDataBits,
    validateCommParamStopBits,
    validateCommParamRespDelay,
    validateCommParamActvOtptItvl
};

//Define a function pointer to process param checking
typedef int16_t (*PFUNC_VALIDATE_SENSORDEF)(uint16_t);
PFUNC_VALIDATE_SENSORDEF g_pfnArraySensorDefValidate[] =
{
    validateFactoryConfigSensorType,
};

//Define a function pointer to process param checking
typedef int16_t (*PFUNC_VALIDATE_USERCFG)(uint16_t);
PFUNC_VALIDATE_USERCFG g_pfnArrayUserCfgValidate[] =
{
    validateUserCfgFloatByteOrder,
    validateUserCfgReserved,
    validateUserCfgReserved,
    validateUserCfgReserved,
    validateUserCfgReserved,
    validateUserCfgReserved,
    validateUserCfgReserved,
    validateUserCfgReserved,
};

//This is DigiEC info define
#define MODULEINFO_PRODUCT_CubeIoRtd_IDENTITY 		0x0025
#define MODULEINFO_PRODUCT_CubeIoRtd_SERIES   		0x0312  //0x0312 is DigiEC series
#define MODULEINFO_PRODUCT_CubeIoRtd_NUMBER   		10
#define MODULEINFO_VERSION_DIGIEC_HARDWARE 		0x0100
#define MODULEINFO_VERSION_DIGIEC_SOFTWARE 		0x0100

const uint8_t PROGMEM_DEVICE_NAMEVERSION_CubeIoRtd[] =
{
    MODULEINFO_PRODUCT_CubeIoRtd_SERIES / 256,
    MODULEINFO_PRODUCT_CubeIoRtd_SERIES % 256,
    MODULEINFO_PRODUCT_CubeIoRtd_NUMBER / 256,
    MODULEINFO_PRODUCT_CubeIoRtd_NUMBER % 256,
    MODULEINFO_VERSION_DIGIEC_HARDWARE / 256,
    MODULEINFO_VERSION_DIGIEC_HARDWARE % 256,
    MODULEINFO_VERSION_DIGIEC_SOFTWARE / 256,
    MODULEINFO_VERSION_DIGIEC_SOFTWARE % 256,
};

int16_t validateUserCfgFloatByteOrder(uint16_t value)
{
    if(value > CONST_FLOATBYTEORDER_END)
    {
        return 0;
    }
    return 1;
}

int16_t validateUserCfgReserved(uint16_t value)
{
    return 1;
}

int16_t validateFactoryConfigSensorType(uint16_t value)
{
    if(value > CONST_SENSORTYPE_CubeIoRtd)
    {
        return 0;
    }
    return 1;
}

int16_t validateCommParamSlaveAddress(uint8_t value)
{
    if(value == MB_BROADCAST_ADDRESS)
    {
        return 0;
    }
    return 1;
}

int16_t validateCommParamBaudrate(uint8_t value)
{
    if(value > SERCOMM_BAUDRATE_END)
    {
        return 0;
    }
    return 1;
}

int16_t validateCommParamProtocol(uint8_t value)
{
    if(value != SERCOMM_PROTOCOL_MODRTU)
    {
        return 0;
    }
    return 1;
}

int16_t validateCommParamDataBits(uint8_t value)
{
    if(value != SERCOMM_DATABITS_8)
    {
        return 0;
    }
    return 1;
}

int16_t validateCommParamParity(uint8_t value)
{
    if(value > SERCOMM_PARITY_ODD)
    {
        return 0;
    }
    return 1;
}

int16_t validateCommParamStopBits(uint8_t value)
{
    if(value > SERCOMM_STOPBITS_2)
    {
        return 0;
    }
    return 1;
}

int16_t validateCommParamRespDelay(uint8_t value)
{
    if(value > 250)
    {
        return 0;
    }
    return 1;
}

int16_t validateCommParamActvOtptItvl(uint8_t value)
{
    if(value > 250)
    {
        return 0;
    }
    return 1;
}


uint16_t modbusRtuTransValidate(STRUCT_USART_INFO* pCommInfo)
{
    //如果不等于自身地址或者广播地址，则返回错误
    if((pCommInfo->questionBuf[0] != pCommInfo->address) && (pCommInfo->questionBuf[0] != MB_BROADCAST_ADDRESS))
        return (1);

    return crc16Check(pCommInfo->questionBuf, pCommInfo->questionBufCount);
}

/* Question here start directly with function code
 (no IP header or Slave number) */
void modbusRtuRequest2Respond(STRUCT_USART_INFO* pCommInfo)
{
    uint8_t* pQuestion = pCommInfo->questionBuf;
    uint8_t* pResponse = pCommInfo->responseBuf;
    pResponse[0] = pQuestion[0];

    switch(pQuestion[1])
    {
    case MB_FC_R_HOLDING_R:
        modbusRtu_3(pCommInfo);
        break;
    case MB_FC_R_INPUT_R:
        modbusRtu_4(pCommInfo);
        break;
    case MB_FC_W_SINGLE_R:
        modbusRtu_6(pCommInfo);
        break;
    case MB_FC_W_MULTI_R:
        modbusRtu_16(pCommInfo);
        break;
    default:
        pResponse[1] = 0x80 | pQuestion[1];
        pResponse[2] = MB_ILLEGAL_FUNCTION;
        pCommInfo->responseLength = 3;
        break;
    }

    crc16Append(pResponse, pCommInfo->responseLength);
    pCommInfo->responseLength += 2;//include 2 bytes crc
}


//MODBUS 3
void modbusRtu_3(STRUCT_USART_INFO* pCommInfo)
{
    uint8_t responseLength = 0;
    uint16_t i;
    uint16_t uint16Value;
    uint8_t* pQuestion = pCommInfo->questionBuf;
    uint8_t* pResponse = pCommInfo->responseBuf;
    uint16_t firstWord = (pQuestion[2] << 8) | pQuestion[3];
    uint16_t nbrWords = (pQuestion[4] << 8) | pQuestion[5];
    uint8_t errorCode = 0;

    if(pCommInfo->questionBufCount != 8)
    {
        errorCode = MB_ILLEGAL_DATA_VALUE;
    }
    else
    {
        if(firstWord == MBR_D_DEBUGENABLE_PASSWORD1 && nbrWords == 2)
        {
            //Enable Factory Setting'
        }
        else
        {
            if(((MX_RS485_IsEnabled() == TRUE) && (firstWord < MBR_D_DEBUGENABLE_PASSWORD1))
                    //|| ((MX_RS485_IsEnabled() == TRUE) && (firstWord >= MBR_DEBUG_FLOAT_BASE))
                    || (GetDebugEnabled() == DEBUG_ENABLE))
            {
            }
            else
            {
                errorCode = MB_ILLEGAL_DATA_ADDRESS;
                goto LABEL_modbusRtu_3;
            }
        }

        if(nbrWords == 0 || nbrWords > 0x007D)
        {
            errorCode = MB_ILLEGAL_DATA_VALUE;
        }
        else
        {
            responseLength = 3 + 2 * nbrWords;
            pResponse[1] = MB_FC_R_HOLDING_R;
            pResponse[2] = 2 * nbrWords;

            switch(firstWord)
            {
            case MBR_MSV_CHN00:
            case MBR_MSV_CHN01:
            case MBR_MSV_CHN02:
            case MBR_MSV_CHN03:
            case MBR_MSV_CHN04:
            case MBR_MSV_CHN05:
            case MBR_MSV_CHN06:
            case MBR_MSV_CHN07:
            case MBR_MSV_CHN08:
            case MBR_MSV_CHN09:
            case MBR_MSV_CHN10:
            case MBR_MSV_CHN11:
            case MBR_MSV_CHN12:
            case MBR_MSV_CHN13:
            case MBR_MSV_CHN14:
            case MBR_MSV_CHN15:
            {
                int16_t values[MBR_MSV_NUM];
                for(i = 0; i < MBR_MSV_NUM; i++)
                {
                    values[i] = 0;
                }

                if((firstWord - MBR_MSV_BASE + nbrWords) <= MBR_MSV_NUM)
                {
                    if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                    {
                        for(uint8_t k=0; k<SENSOR_CHANNEL_SIZE_MAXSUPPORT; k++)
                        {
                            values[k] = (int16_t)(100.00 * g_MValue.SensorChnlData[k].ValueOffseted);
                        }
                    }
                    else
                    {
                        for(uint8_t k=0; k<SENSOR_CHANNEL_SIZE_MAXSUPPORT; k++)
                        {
                            values[k] = (int16_t)(SENSOR_CHNL_STATUS_SNR_ERROR_VALUE);
                        }
                    }

                    int16_t startIdx = firstWord - MBR_MSV_BASE;
                    for(i = 0; i < nbrWords; i++)
                    {
                        pResponse[3 + 2 * i] = HI8_UINT(values[startIdx]);
                        pResponse[3 + 2 * i + 1] = LO8_UINT(values[startIdx]);
                        startIdx++;
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_MSV_FLOAT_CHN00:
            case MBR_MSV_FLOAT_CHN01:
            case MBR_MSV_FLOAT_CHN02:
            case MBR_MSV_FLOAT_CHN03:
            case MBR_MSV_FLOAT_CHN04:
            case MBR_MSV_FLOAT_CHN05:
            case MBR_MSV_FLOAT_CHN06:
            case MBR_MSV_FLOAT_CHN07:
            case MBR_MSV_FLOAT_CHN08:
            case MBR_MSV_FLOAT_CHN09:
            case MBR_MSV_FLOAT_CHN10:
            case MBR_MSV_FLOAT_CHN11:
            case MBR_MSV_FLOAT_CHN12:
            case MBR_MSV_FLOAT_CHN13:
            case MBR_MSV_FLOAT_CHN14:
            case MBR_MSV_FLOAT_CHN15:
            {
                float values[MBR_MSV_FLOAT_NUM / 2];
                uint8_t* pFloatByte;

                for(i = 0; i < MBR_MSV_FLOAT_NUM / 2; i++)
                {
                    values[i] = 0;
                }

                if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                {
                    for(uint8_t k=0; k<SENSOR_CHANNEL_SIZE_MAXSUPPORT; k++)
                    {
                        values[k] = g_MValue.SensorChnlData[k].RawAdcValue;
                    }
                }
                else
                {
                    for(uint8_t k=0; k<SENSOR_CHANNEL_SIZE_MAXSUPPORT; k++)
                    {
                        values[k] = (int16_t)(SENSOR_CHNL_STATUS_SNR_ERROR_VALUE);
                    }
                }

                uint8_t startChnl = firstWord - MBR_MSV_FLOAT_BASE;
                if((startChnl + nbrWords) <= MBR_MSV_FLOAT_NUM && (nbrWords % 2 == 0))
                {
                    uint8_t floatArrayStartIndex = startChnl / 2;
                    uint8_t floatArrayToReadCount = nbrWords / 2;
                    uint16_t floatByteOrder = (uint16_t)eeprom_readint32(&eepUserConfig.floatByteOrder);

                    for(i = 0; i < floatArrayToReadCount; i++)
                    {
                        pFloatByte = (uint8_t*)&values[floatArrayStartIndex + i];

                        if(floatByteOrder == CONST_FLOATBYTEORDER_IEEE754_BIGENDIAN) //3,2,1,0(内存中顺序)
                        {
                            pResponse[3 + 4 * i + 0] = pFloatByte[3];
                            pResponse[3 + 4 * i + 1] = pFloatByte[2];
                            pResponse[3 + 4 * i + 2] = pFloatByte[1];
                            pResponse[3 + 4 * i + 3] = pFloatByte[0];
                        }
                        else if(floatByteOrder == CONST_FLOATBYTEORDER_IEEE754_LITTLEENDIAN) //0,1,2,3(内存中顺序)
                        {
                            pResponse[3 + 4 * i + 0] = pFloatByte[0];
                            pResponse[3 + 4 * i + 1] = pFloatByte[1];
                            pResponse[3 + 4 * i + 2] = pFloatByte[2];
                            pResponse[3 + 4 * i + 3] = pFloatByte[3];
                        }
                        else if(floatByteOrder == CONST_FLOATBYTEORDER_IEEE754_BIGENDIANWORDSWAP) //1,0,3,2(内存中顺序)
                        {
                            pResponse[3 + 4 * i + 0] = pFloatByte[2];
                            pResponse[3 + 4 * i + 1] = pFloatByte[3];
                            pResponse[3 + 4 * i + 2] = pFloatByte[0];
                            pResponse[3 + 4 * i + 3] = pFloatByte[1];
                        }
                        else//2,3,1,0(内存中顺序)
                        {
                            pResponse[3 + 4 * i + 0] = pFloatByte[1];
                            pResponse[3 + 4 * i + 1] = pFloatByte[0];
                            pResponse[3 + 4 * i + 2] = pFloatByte[3];
                            pResponse[3 + 4 * i + 3] = pFloatByte[2];
                        }
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_INFO_MSV_00:
            case MBR_INFO_MSV_01:
            case MBR_INFO_MSV_02:
            case MBR_INFO_MSV_03:
            case MBR_INFO_MSV_04:
            case MBR_INFO_MSV_05:
            case MBR_INFO_MSV_06:
            case MBR_INFO_MSV_07:
            case MBR_INFO_MSV_08:
            case MBR_INFO_MSV_09:
            case MBR_INFO_MSV_10:
            case MBR_INFO_MSV_11:
            case MBR_INFO_MSV_12:
            case MBR_INFO_MSV_13:
            case MBR_INFO_MSV_14:
            case MBR_INFO_MSV_15:
            {
                int16_t values[16];
                for(i = 0; i < MBR_INFO_MSV_NUM; i++)
                {
                    values[i] = 0;
                }

                uint32_t sensorType = eeprom_readint32((uint32_t*) & (eepFactoryConfig.sensorType));
                values[0] = MODULEINFO_PRODUCT_CubeIoRtd_IDENTITY; //Product Identity
                values[1] = pCommInfo->address; //Product Address

                if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                {
                }
                else
                {
                }

                uint8_t startChnl = firstWord - MBR_INFO_MSV_00;
                if((startChnl + nbrWords) <= MBR_INFO_MSV_NUM)
                {
                    for(i = 0; i < nbrWords; i++)
                    {
                        pResponse[3 + 2 * i] = HI8_UINT(values[startChnl + i]);
                        pResponse[3 + 2 * i + 1] = LO8_UINT(values[startChnl + i]);
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_USERCONFIG_FLOATBYTEORDER:
            case MBR_USERCONFIG_RESERVED1:
            case MBR_USERCONFIG_RESERVED2:
            case MBR_USERCONFIG_RESERVED3:
            case MBR_USERCONFIG_RESERVED4:
            case MBR_USERCONFIG_RESERVED5:
            case MBR_USERCONFIG_RESERVED6:
            case MBR_USERCONFIG_RESERVED7:
            {
                uint8_t startChnl = firstWord - MBR_USERCONFIG_BASE;
                if((startChnl + nbrWords) <= MBR_USERCONFIG_NUM)
                {
                    for(i = 0; i < nbrWords; i++)
                    {
                        uint8_t idx = 2 * i + 3;
                        uint16_t value = (uint16_t)eeprom_readint32((uint32_t*)&eepUserConfig + startChnl + i);
                        pResponse[idx] = HI8_UINT(value);
                        pResponse[idx + 1] = LO8_UINT(value);
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_FACTORYRESET:
            {
                if((firstWord == MBR_FACTORYRESET) && (nbrWords == 1))
                {
                    uint8_t idx = 3;
                    uint16_t value = g_MValue.FactoryResetStatus;
                    pResponse[idx] = HI8_UINT(value);
                    pResponse[idx + 1] = LO8_UINT(value);
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_SYSTEMRESET:
            {
                if((firstWord == MBR_SYSTEMRESET) && (nbrWords == 1))
                {
                    uint8_t idx = 3;
                    uint16_t value = g_MValue.SystemResetStatus;
                    pResponse[idx] = HI8_UINT(value);
                    pResponse[idx + 1] = LO8_UINT(value);
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_CALIBRESET:
            {
                if((firstWord == MBR_CALIBRESET) && (nbrWords == 1))
                {
                    uint8_t idx = 3;
                    uint16_t value = g_MValue.CalibResetStatus;
                    pResponse[idx] = HI8_UINT(value);
                    pResponse[idx + 1] = LO8_UINT(value);
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_SELFCALIB:
            {
                if((firstWord == MBR_SELFCALIB) && (nbrWords == 1))
                {
                    uint8_t idx = 3;
                    uint16_t value = g_MValue.SelfCalibStatus;
                    pResponse[idx] = HI8_UINT(value);
                    pResponse[idx + 1] = LO8_UINT(value);
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_USART_ADDRESS:
            case MBR_USART_BAUDRATE:
            case MBR_USART_PROTOCOL:
            case MBR_USART_PARITY:
            case MBR_USART_DATABITS:
            case MBR_USART_STOPBITS:
            case MBR_USART_RESPONSEDELAY:
            case MBR_USART_ACTIVEOUTPUTINTERVAL:
            {
                //caculate start channel, startChnl vary from 0~5
                uint8_t startChnl = firstWord - MBR_USART_ADDRESS;
                if((startChnl + nbrWords) <= MBR_USART_NUM)
                {
                    for(i = 0; i < nbrWords; i++)
                    {
                        uint8_t idx = 2 * i + 3;//3=salveAddr+Function+ByteCount
                        uint16Value = eeprom_readint32(&eepUsart.address + startChnl + i);
                        pResponse[idx] = 0;
                        pResponse[idx + 1] = LO8_UINT(uint16Value);
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_USERTAG_0:
            case MBR_USERTAG_1:
            case MBR_USERTAG_2:
            case MBR_USERTAG_3:
            {
                if((firstWord == MBR_USERTAG_0) && (nbrWords == MBR_USERTAG_NUM))
                {
                    uint8_t idx = 3;//3=salveAddr+Function+ByteCount

                    uint32_t value = eeprom_readint32(&eepUserTag.sn[0]);
                    pResponse[idx + 0] = HI8_UINT(HI16_UINT32(value));
                    pResponse[idx + 1] = LO8_UINT(HI16_UINT32(value));
                    pResponse[idx + 2] = HI8_UINT(LO16_UINT32(value));
                    pResponse[idx + 3] = LO8_UINT(LO16_UINT32(value));

                    value = eeprom_readint32(&eepUserTag.sn[1]);
                    pResponse[idx + 4] = HI8_UINT(HI16_UINT32(value));
                    pResponse[idx + 5] = LO8_UINT(HI16_UINT32(value));
                    pResponse[idx + 6] = HI8_UINT(LO16_UINT32(value));
                    pResponse[idx + 7] = LO8_UINT(LO16_UINT32(value));
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_DEVICE_MODULENAME1:
            {
                uint8_t* pPROGMEM_DEVICE_NAMEVERSION;
                uint32_t sensorType = eeprom_readint32((uint32_t*) & (eepFactoryConfig.sensorType));
                if(sensorType == CONST_SENSORTYPE_CubeIoRtd)
                {
                    pPROGMEM_DEVICE_NAMEVERSION = (uint8_t*)PROGMEM_DEVICE_NAMEVERSION_CubeIoRtd;
                }
                else
                {
                    pPROGMEM_DEVICE_NAMEVERSION = (uint8_t*)PROGMEM_DEVICE_NAMEVERSION_CubeIoRtd;
                }

                if((firstWord == MBR_DEVICE_MODULENAME1) && (nbrWords == MBR_DEVICE_NAMEVERSION_NUM))
                {
                    uint8_t idx = 3;//3=salveAddr+Function+ByteCount
                    for(i = 0; i < 2 * nbrWords; i++)
                    {
                        pResponse[idx + i] = pPROGMEM_DEVICE_NAMEVERSION[i];
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_D_MODINFO_YYYY:
            {
                if((firstWord == MBR_D_MODINFO_YYYY) && (nbrWords == MBR_D_MODINFO_NUM))
                {
                    uint16_t uint16Value;
                    for(i = 0; i < nbrWords; i++)
                    {
                        uint8_t idx = 2 * i + 3;//3=salveAddr+Function+ByteCount
                        uint16Value = (uint16_t)eeprom_readint32((uint32_t*)((uint32_t*) & (eepManuInfo.yyyy) + i)); //
                        pResponse[idx] = HI8_UINT(uint16Value);
                        pResponse[idx + 1] = LO8_UINT(uint16Value);
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_FACTORYCONFIG_SENSORTYPE:
            case MBR_FACTORYCONFIG_TEMPSENSOR:
            case MBR_FACTORYCONFIG_RESERVED1:
            case MBR_FACTORYCONFIG_RESERVED2:
            {
                uint8_t startChnl = firstWord - MBR_FACTORYCONFIG_SENSORTYPE;
                if((startChnl + nbrWords) <= MBR_FACTORYCONFIG_NUM)
                {
                    for(i = 0; i < nbrWords; i++)
                    {
                        uint8_t idx = 2 * i + 3;
                        uint16Value = (uint16_t)eeprom_readint32(&eepFactoryConfig.sensorType + startChnl + i);
                        pResponse[idx] = HI8_UINT(uint16Value);
                        pResponse[idx + 1] = LO8_UINT(uint16Value);
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            default:
                errorCode = MB_ILLEGAL_DATA_ADDRESS;
                break;
            }
        }
    }
LABEL_modbusRtu_3:
    if(errorCode != 0)
    {
        pResponse[1] = 0x80 | pQuestion[1];
        pResponse[2] = errorCode;
        responseLength = 3;
    }

    pCommInfo->responseLength = responseLength;
}

//MODBUS 4
void modbusRtu_4(STRUCT_USART_INFO* pCommInfo)
{
    uint8_t* pResponse = pCommInfo->responseBuf;
    modbusRtu_3(pCommInfo);

    /* Alex updated on 2018019---在使用Modbus的4号请求功能时，如果寄存器地址不正确会造成错误响应数据
    if(pResponse[1]==0x83)
    {
        pResponse[1]=0x84;
    }
    else
    {
        pResponse[1]=0x04;
    }
    */
    if(pResponse[1] == 0x03)
    {
        pResponse[1] = 0x04;
    }
}


// 修改模块地址，波特率，协议选择字节
void modbusRtu_6(STRUCT_USART_INFO* pCommInfo)
{
    uint8_t* pQuestion = pCommInfo->questionBuf;
    uint8_t* pResponse = pCommInfo->responseBuf;
    uint16_t firstWord = (pQuestion[2] << 8) | pQuestion[3];
    uint8_t errorCode = 0;
    uint8_t responseLength;

    pResponse[1] = pQuestion[1];
    pResponse[2] = pQuestion[2];
    pResponse[3] = pQuestion[3];
    pResponse[4] = pQuestion[4];
    pResponse[5] = pQuestion[5];
    responseLength = 6;

    if(pCommInfo->questionBufCount != 8)
    {
        errorCode = MB_ILLEGAL_DATA_VALUE;
    }
    else
    {
        if((MX_RS485_IsEnabled() == TRUE && firstWord < MBR_D_DEBUGENABLE_PASSWORD1) || (GetDebugEnabled() == DEBUG_ENABLE))
        {
        }
        else
        {
            errorCode = MB_ILLEGAL_DATA_ADDRESS;
            goto LABEL_modbusRtu_6;
        }

        switch(firstWord)
        {
        case MBR_USERCONFIG_FLOATBYTEORDER:
        case MBR_USERCONFIG_RESERVED1:
        case MBR_USERCONFIG_RESERVED2:
        case MBR_USERCONFIG_RESERVED3:
        case MBR_USERCONFIG_RESERVED4:
        case MBR_USERCONFIG_RESERVED5:
        case MBR_USERCONFIG_RESERVED6:
        case MBR_USERCONFIG_RESERVED7:
        {
            int16_t value = MAKE_UINT(pQuestion[4], pQuestion[5]);
            uint8_t startChnl = firstWord - MBR_USERCONFIG_BASE;
            if(!(g_pfnArrayUserCfgValidate[startChnl])(value))
            {
                errorCode = MB_ILLEGAL_DATA_VALUE;
            }
            else
            {
                eeprom_writeint32((uint32_t*)&eepUserConfig + startChnl, (int32_t)value);
                crc32EepromAppend((uint32_t*)&eepUserConfig, sizeof(eepUserConfig) / sizeof(uint8_t));

                if(firstWord == MBR_USERCONFIG_RESERVED4)
                {
                    //如果设置了周期性采集，则延时1秒后重新启动
                    extern UTIL_TIMER_Object_t TimerSystemReset;
                    UTIL_TIMER_Start(&TimerSystemReset);
                }
            }
            break;
        }

        case MBR_USART_ADDRESS:
        case MBR_USART_BAUDRATE:
        case MBR_USART_PROTOCOL:
        case MBR_USART_PARITY:
        case MBR_USART_DATABITS:
        case MBR_USART_STOPBITS:
        case MBR_USART_RESPONSEDELAY:
        case MBR_USART_ACTIVEOUTPUTINTERVAL:
        {
            uint8_t charValue = pQuestion[5];//low
            uint8_t startChnl = firstWord - MBR_USART_ADDRESS;
            if(!(g_pfnArrayCommParamValidate[startChnl])(charValue))
            {
                errorCode = MB_ILLEGAL_DATA_VALUE;
            }
            else
            {
                eeprom_writeint32(&eepUsart.address + startChnl, charValue);
                crc32EepromAppend((uint32_t*) & (eepUsart), sizeof(eepUsart) / sizeof(uint8_t));
            }
            break;
        }

        case MBR_FACTORYRESET:
        {
            uint16_t value = MAKE_UINT(pQuestion[4], pQuestion[5]);
            if(value != 0xFFFF)
            {
                errorCode = MB_ILLEGAL_DATA_VALUE;
            }
            else
            {
                //Factory Reset
                g_MValue.FactoryResetStatus = CONST_FACTORYRESET_STATUS_PROCESSING;
                eeprom_reset_factory();
                g_MValue.FactoryResetStatus = CONST_FACTORYRESET_STATUS_DONE;
            }
            break;
        }

        case MBR_SYSTEMRESET:
        {
            uint16_t value = MAKE_UINT(pQuestion[4], pQuestion[5]);
            if(value != 0xFFFF)
            {
                errorCode = MB_ILLEGAL_DATA_VALUE;
            }
            else
            {
                //System Reset
                g_MValue.SystemResetStatus = CONST_SYSTEMRESET_STATUS_PROCESSING;
                extern UTIL_TIMER_Object_t TimerSystemReset;
                UTIL_TIMER_Start(&TimerSystemReset);
                g_MValue.SystemResetStatus = CONST_SYSTEMRESET_STATUS_DONE;
            }
            break;
        }

        case MBR_CALIBRESET:
        {
            uint16_t value = MAKE_UINT(pQuestion[4], pQuestion[5]);
            if(value != 0xFFFF)
            {
                errorCode = MB_ILLEGAL_DATA_VALUE;
            }
            else
            {
                //Factory Reset
                g_MValue.CalibResetStatus = CONST_CALIBRESET_STATUS_PROCESSING;
                eeprom_reset_factory();
                g_MValue.CalibResetStatus = CONST_CALIBRESET_STATUS_DONE;
            }
            break;
        }

        case MBR_SELFCALIB:
        {
            uint16_t value = MAKE_UINT(pQuestion[4], pQuestion[5]);
            if(value != 0xFFFF)
            {
                errorCode = MB_ILLEGAL_DATA_VALUE;
            }
            else
            {
                if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                {
                    //如果传感器处于正常状态，则设置自校准请求，在下一轮采集完成后进行自校准
                    g_MValue.SelfCalibStatus = CONST_SELFCALIB_STATUS_PROCESSING;
                }
                else
                {
                    //如果传感器处于异常状态，则直接设置为校准失败
                    g_MValue.SelfCalibStatus = CONST_SELFCALIB_STATUS_FAIL;
                }
            }
            break;
        }
        default:
            errorCode = MB_ILLEGAL_DATA_ADDRESS;
            break;
        }
    }
LABEL_modbusRtu_6:
    if(errorCode != 0)
    {
        pResponse[1] = 0x80 | pQuestion[1];
        pResponse[2] = errorCode;
        responseLength = 3;
    }
    pCommInfo->responseLength = responseLength;
}

//修改
void modbusRtu_16(STRUCT_USART_INFO* pCommInfo)
{
    uint16_t i;
    uint16_t uint16Value;
    uint8_t* pQuestion = pCommInfo->questionBuf;
    uint8_t* pResponse = pCommInfo->responseBuf;
    uint16_t firstWord = (pQuestion[2] << 8) | pQuestion[3];
    uint16_t nbrWords = (pQuestion[4] << 8) | pQuestion[5];
    uint8_t* pSrcStart = &pQuestion[7];
    uint8_t errorCode = 0;
    uint8_t responseLength = 0;
    uint8_t byteNbr = pQuestion[6];

    pResponse[0] = pQuestion[0];//addr
    pResponse[1] = pQuestion[1];//func
    pResponse[2] = pQuestion[2];//
    pResponse[3] = pQuestion[3];
    pResponse[4] = pQuestion[4];
    pResponse[5] = pQuestion[5];
    responseLength = 6;

    if(pCommInfo->questionBufCount != (9 + byteNbr))
    {
        errorCode = MB_ILLEGAL_DATA_VALUE;
    }
    else
    {
        if(firstWord == MBR_D_DEBUGENABLE_PASSWORD1 && nbrWords == 2)
        {
            //Enable Factory Setting'
        }
        else
        {
            if((MX_RS485_IsEnabled() == TRUE && firstWord < MBR_D_DEBUGENABLE_PASSWORD1) || (GetDebugEnabled() == DEBUG_ENABLE))
            {
            }
            else
            {
                errorCode = MB_ILLEGAL_DATA_ADDRESS;
                goto LABEL_modbusRtu_16;
            }
        }

        if(nbrWords == 0 || nbrWords > 0x007D)
        {
            //Modbus protocol limits Number of word should <0x007D
            errorCode = MB_ILLEGAL_DATA_VALUE;
        }
        else
        {
            switch(firstWord)
            {
            case MBR_USERCONFIG_FLOATBYTEORDER:
            case MBR_USERCONFIG_RESERVED1:
            case MBR_USERCONFIG_RESERVED2:
            case MBR_USERCONFIG_RESERVED3:
            case MBR_USERCONFIG_RESERVED4:
            case MBR_USERCONFIG_RESERVED5:
            case MBR_USERCONFIG_RESERVED6:
            case MBR_USERCONFIG_RESERVED7:
            {
                uint8_t startChnl = firstWord - MBR_USERCONFIG_BASE;
                uint16_t value;
                if((startChnl + nbrWords) <= MBR_USERCONFIG_NUM)
                {
                    for(i = 0; i < nbrWords; i++)
                    {
                        value = MAKE_UINT(pSrcStart[2 * i], pSrcStart[2 * i + 1]);
                        if(!(g_pfnArrayUserCfgValidate[startChnl + i])(value))
                        {
                            errorCode = MB_ILLEGAL_DATA_VALUE;
                            break;
                        }
                    }
                    if(errorCode == 0)
                    {
                        for(i = 0; i < nbrWords; i++)
                        {
                            value = MAKE_UINT(pSrcStart[2 * i], pSrcStart[2 * i + 1]);
                            eeprom_writeint32((uint32_t*)&eepUserConfig + startChnl + i, (int32_t)value);
                        }
                        crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(eepUserConfig) / sizeof(uint8_t));

                        if((firstWord <= MBR_USERCONFIG_RESERVED4) && ((firstWord + nbrWords) > MBR_USERCONFIG_RESERVED4))
                        {
                            //如果设置了周期性采集，则延时1秒后重新启动
                            extern UTIL_TIMER_Object_t TimerSystemReset;
                            UTIL_TIMER_Start(&TimerSystemReset);
                        }
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_FACTORYCONFIG_SENSORTYPE:
            case MBR_FACTORYCONFIG_TEMPSENSOR:
            case MBR_FACTORYCONFIG_RESERVED1:
            case MBR_FACTORYCONFIG_RESERVED2:
            {
                uint8_t startChnl = firstWord - MBR_FACTORYCONFIG_SENSORTYPE;
                uint16_t value;
                if((startChnl + nbrWords) <= MBR_FACTORYCONFIG_NUM)
                {
                    for(i = 0; i < nbrWords; i++)
                    {
                        value = MAKE_UINT(pSrcStart[2 * i], pSrcStart[2 * i + 1]);
                        eeprom_writeint32((uint32_t*)&eepFactoryConfig.sensorType + startChnl + i, (int32_t)value);
                    }
                    crc32EepromAppend((uint32_t*) &eepFactoryConfig, sizeof(eepFactoryConfig) / sizeof(uint8_t));
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_USART_ADDRESS:
            case MBR_USART_BAUDRATE:
            case MBR_USART_PROTOCOL:
            case MBR_USART_PARITY:
            case MBR_USART_DATABITS:
            case MBR_USART_STOPBITS:
            case MBR_USART_RESPONSEDELAY:
            case MBR_USART_ACTIVEOUTPUTINTERVAL:
            {
                uint8_t startChnl = firstWord - MBR_USART_ADDRESS;
                if((startChnl + nbrWords) <= MBR_USART_NUM)
                {
                    for(i = 0; i < nbrWords; i++)
                    {
                        uint16Value = pSrcStart[2 * i + 1];
                        if(!(g_pfnArrayCommParamValidate[startChnl + i])(uint16Value))
                        {
                            errorCode = MB_ILLEGAL_DATA_VALUE;
                            break;
                        }
                    }
                    if(errorCode == 0)
                    {
                        for(i = 0; i < nbrWords; i++)
                        {
                            uint16Value = pSrcStart[2 * i + 1];
                            eeprom_writeint32(&(eepUsart.address) + startChnl + i, uint16Value);
                        }
                        crc32EepromAppend((uint32_t*) &eepUsart, sizeof(eepUsart) / sizeof(uint8_t));
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_USERTAG_0:
            case MBR_USERTAG_1:
            case MBR_USERTAG_2:
            case MBR_USERTAG_3:
            {
                if((firstWord == MBR_USERTAG_0) && (nbrWords == MBR_USERTAG_NUM))
                {
                    uint8_t* pValueStartPos = (uint8_t*)&pSrcStart[0];

                    for(uint8_t i = 0; i < 2; i++)
                    {
                        uint16_t hi16 = MAKE_16BIT(pValueStartPos[4 * i], pValueStartPos[4 * i + 1]);
                        uint16_t lo16 = MAKE_16BIT(pValueStartPos[4 * i + 2], pValueStartPos[4 * i + 3]);
                        uint32_t value = MAKE_32BIT(hi16, lo16);
                        eeprom_writeint32(&eepUserTag.sn[i], value);
                    }
                    crc32EepromAppend((uint32_t*) &eepUserTag, sizeof(STRUCT_USERTAG) / sizeof(char));
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_D_MODINFO_YYYY:
            {
                if((firstWord == MBR_D_MODINFO_YYYY) && (nbrWords == MBR_D_MODINFO_NUM))
                {
                    for(i = 0; i < nbrWords; i++)
                    {
                        UNION_INT_VALUE unionValue;
                        unionValue.uint8Value[0] = pSrcStart[2 * i + 1]; //high
                        unionValue.uint8Value[1] = pSrcStart[2 * i];//low
                        eeprom_writeint32(&(eepManuInfo.yyyy) + i, unionValue.uint16Value); //写EEPROM
                    }
                    crc32EepromAppend((uint32_t*) &eepManuInfo, sizeof(STRUCT_MANUINFO)
                                      / sizeof(uint8_t));//重新计算crc
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_D_DEBUGENABLE_PASSWORD1:
            {
                UNION_LONG_VALUE unionValue;
                if((firstWord == MBR_D_DEBUGENABLE_PASSWORD1) && (nbrWords
                        == MBR_D_DEBUGENABLE_PASSWORD_NUM))
                {
                    unionValue.uint8Value[3] = pSrcStart[0];//high
                    unionValue.uint8Value[2] = pSrcStart[1];//low
                    unionValue.uint8Value[1] = pSrcStart[2];//high
                    unionValue.uint8Value[0] = pSrcStart[3];//low
                    if(unionValue.uint32Value == 127731617)
                    {
                        SetDebugEnabled(DEBUG_ENABLE);
                    }
                    else
                    {
                        SetDebugEnabled(DEBUG_DISABLE);
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_FACTORYRESET:
            {
                if((firstWord == MBR_FACTORYRESET) && (nbrWords == 1))
                {
                    uint16_t value = MAKE_UINT(pSrcStart[0], pSrcStart[1]);
                    if(value != 0xFFFF)
                    {
                        errorCode = MB_ILLEGAL_DATA_VALUE;
                    }
                    else
                    {
                        //Factory Reset
                        g_MValue.FactoryResetStatus = CONST_FACTORYRESET_STATUS_PROCESSING;
                        eeprom_reset_factory();
                        g_MValue.FactoryResetStatus = CONST_FACTORYRESET_STATUS_DONE;
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }

            case MBR_SYSTEMRESET:
            {
                if((firstWord == MBR_SYSTEMRESET) && (nbrWords == 1))
                {
                    uint16_t value = MAKE_UINT(pSrcStart[0], pSrcStart[1]);
                    if(value != 0xFFFF)
                    {
                        errorCode = MB_ILLEGAL_DATA_VALUE;
                    }
                    else
                    {
                        //Reset System
                        extern UTIL_TIMER_Object_t TimerSystemReset;
                        UTIL_TIMER_Start(&TimerSystemReset);
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_CALIBRESET:
            {
                if((firstWord == MBR_CALIBRESET) && (nbrWords == 1))
                {
                    uint16_t value = MAKE_UINT(pSrcStart[0], pSrcStart[1]);
                    if(value != 0xFFFF)
                    {
                        errorCode = MB_ILLEGAL_DATA_VALUE;
                    }
                    else
                    {
                        //Calib Reset
                        g_MValue.CalibResetStatus = CONST_CALIBRESET_STATUS_PROCESSING;
                        eeprom_reset_calibration();
                        g_MValue.CalibResetStatus = CONST_CALIBRESET_STATUS_DONE;
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            case MBR_SELFCALIB:
            {
                if((firstWord == MBR_SELFCALIB) && (nbrWords == 1))
                {
                    uint16_t value = MAKE_UINT(pSrcStart[0], pSrcStart[1]);
                    if(value != 0xFFFF)
                    {
                        errorCode = MB_ILLEGAL_DATA_VALUE;
                    }
                    else
                    {
                        if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                        {
                            //如果传感器处于正常状态，则设置自校准请求，在下一轮采集完成后进行自校准
                            g_MValue.SelfCalibStatus = CONST_SELFCALIB_STATUS_PROCESSING;
                        }
                        else
                        {
                            //如果传感器处于异常状态，则直接设置为校准失败
                            g_MValue.SelfCalibStatus = CONST_SELFCALIB_STATUS_FAIL;
                        }
                    }
                }
                else
                {
                    errorCode = MB_ILLEGAL_DATA_ADDRESS;
                }
                break;
            }
            default:
                errorCode = MB_ILLEGAL_DATA_ADDRESS;
                break;
            }
        }
    }
LABEL_modbusRtu_16:
    if(errorCode != 0)
    {
        pResponse[1] = 0x80 | pQuestion[1];
        pResponse[2] = errorCode;
        responseLength = 3;
    }
    pCommInfo->responseLength = responseLength;
}

