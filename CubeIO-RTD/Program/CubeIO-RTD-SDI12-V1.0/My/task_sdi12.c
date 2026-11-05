#include "main.h"
#include "hal_sdi12.h"
#include "utils.h"
#include "hal_gpio.h"
#include "sensor.h"
#include "delay_utils.h"
#include "globals.h"
#include "sensor.h"
#include "utils_datetime.h"

void TaskSDI12Process(void)
{
    uint8_t isExitTaskSDI12Process = 0;
    while(1)
    {
        if(isExitTaskSDI12Process == 1)
        {
            break;//exit the TaskSDI12Process to enter low power
        }
        //系统首次上电或者从休眠中被SDI12总线唤醒后，仅仅在这里执行看门狗清除操作。
        //这样可以防止唤醒后程序运行中某处卡住
        //LL_IWDG_ReloadCounter(IWDG);
        switch(g_sdiStateMachine)
        {
        case SDI_STANDBY:
        {
            isExitTaskSDI12Process = 1;
            break;
        }
        case SDI_FIRST_BREAK_DETECTING:
        {
            char sdi12Char;

            MX_TIM2_Init();
            MX_TIM2_ENABLE();
            MX_TIM2_RESETCNT();
            do
            {
                //SDI12协议<<<未要求>>>，但必须处理
                if(MX_TIM2_GETCNT() > 100)
                {
                    MX_TIM2_DISABLE();
                    MX_TIM2_DeInit();
                    g_sdiStateMachine = SDI_STANDBY;
                    break;
                }

                if(sdi12GetChar(&sdi12Char) == 0)
                {
                    continue;//No char received
                }

                //char received
                if((uint8_t)sdi12Char == SDI12_CAHR_BREAK)
                {
                    g_sdiStateMachine = SDI_FIRST_MARKING_DETECTING;
                    break;
                }
            }
            while(1);
            break;
        }
        case SDI_FIRST_MARKING_DETECTING:
        {
            char sdi12Char;

            MX_TIM2_Init();
            MX_TIM2_ENABLE();
            MX_TIM2_RESETCNT();
            do
            {
                //SDI12协议<<<未要求>>>，但必须处理
                if(MX_TIM2_GETCNT() > 100)
                {
                    MX_TIM2_DISABLE();
                    MX_TIM2_DeInit();
                    g_sdiStateMachine = SDI_STANDBY;
                    break;
                }

                if(sdi12GetChar(&sdi12Char) == 0)
                {
                    continue;//No char received
                }

                //SDI12协议<<<未要求>>>，但需要处理
                if((uint8_t)sdi12Char == SDI12_CAHR_BREAK)
                {
                    g_sdiStateMachine = SDI_FIRST_MARKING_DETECTING;
                    break;
                }

                //SDI12协议要求
                if((uint8_t)sdi12Char == SDI12_CAHR_MARKING)
                {
                    g_sdiStateMachine = SDI_ADDRESS;
                    break;
                }
            }
            while(1);
            break;
        }
        case SDI_ADDRESS:
        {
            char sdi12Char;

            MX_TIM2_Init();
            MX_TIM2_ENABLE();
            MX_TIM2_RESETCNT();
            do
            {
                //SDI12协议要求，100ms超时判断
                if(MX_TIM2_GETCNT() > 100)
                {
                    MX_TIM2_DISABLE();
                    MX_TIM2_DeInit();
                    g_sdiStateMachine = SDI_STANDBY;
                    break;
                }

                if(sdi12GetChar(&sdi12Char) == 0)
                {
                    continue;//No char received
                }

                //SDI12协议要求
                if((uint8_t)sdi12Char == SDI12_CAHR_BREAK)
                {
                    g_sdiStateMachine = SDI_FIRST_MARKING_DETECTING;
                    break;
                }

                //SDI12协议要求
                //Sensors must return to a low-power standby mode after receiving an invalid address or
                //after detecting a marking state on the data line for 100 milliseconds.
                if((uint8_t)sdi12Char == SDI12_CAHR_MARKING)
                {
                    continue;
                }

                //SDI12协议要求
                //Sensors must return to a low-power standby mode after receiving an invalid address or
                //after detecting a marking state on the data line for 100 milliseconds.
                //地址字符的处理放在最后，等BREAK和MARKING检测完成后进行
                if(sdi12Char == g_sdiCommInfo.addr || sdi12Char == '?')
                {
                    g_sdiStateMachine = SDI_COMMAND; //合法地址进行下一步指令处理
                    break;
                }
                else
                {
                    g_sdiStateMachine = SDI_STANDBY; //非法地址返回到SDI_STANDBY
                    break;
                }
            }
            while(1);
            break;
        }
        case SDI_COMMAND:
        {
            char sdi12Char;
            g_sdiCommInfo.vm_CommandBufferIndex = 0;
            memset(g_sdiCommInfo.vm_CommandBuffer, 0, vm_UART_RX_BUF_L);
            do
            {
                if(sdi12GetChar(&sdi12Char) == 0)
                {
                    continue;//No char received
                }

                //char received
                if((uint8_t)sdi12Char == SDI12_CAHR_BREAK)
                {
                    g_sdiStateMachine = SDI_FIRST_MARKING_DETECTING;
                    break;
                }
                if((uint8_t)sdi12Char == SDI12_CAHR_MARKING)
                {
                    g_sdiStateMachine = SDI_ADDRESS;
                    break;
                }

                g_sdiCommInfo.vm_CommandBuffer[g_sdiCommInfo.vm_CommandBufferIndex++] = sdi12Char;
                if(sdi12Char == '!')
                {
                    g_sdiStateMachine = SDI_PROCESS;
                    break;
                }
            }
            while(1);
            break;
        }
        case SDI_PROCESS:
        {
            char commandChar = g_sdiCommInfo.vm_CommandBuffer[0];
            char* cmd = &g_sdiCommInfo.vm_CommandBuffer[0];
            switch(commandChar)
            {
            case '!'://a!, Confirm Sensor On Line
            {
                char buf[1];
                sdi12Send(buf, 0, 0); //Zero data to send, the SDI12 address will be append in function: sdi12Send

                //SDI12协议要求
                g_sdiStateMachine = SDI_ADDRESS;
                break;
            }
            case 'V'://aV!, Sensor Validation
            {
                if(strstr(cmd, "V!"))
                {
                    uint16_t warmUpTime = SensorGetMeasureTime();
                    char buf[16];
                    sprintf(buf, "%03d%d", warmUpTime, 1);
                    sdi12Send(buf, 4, 0);

                    //Mesaure
                    int16_t errorCode = SensorStartMeasureAndWaitDone();

                    sdi12Send(buf, 0, 0); //service request
                    g_sdiCommInfo.DCommandRetType = RETTYPE_VERIFY;
                }
                else
                {
                    //send error
                    sdi12SendError();
                }

                //SDI12协议要求
                g_sdiStateMachine = SDI_ADDRESS;
                break;
            }
            case 'I'://aI!, Get Identification Info
            {
                if(strstr(cmd, "I!"))
                {
                    char buf[40];
                    uint8_t idx = 0;

                    //allccccccccmmmmmmvvvxxxxxxxxxxxxx<CR><LF>
                    //len=sprintf(buf,"%c%s",addr,"13INFWIN  DigiWL2.00000000043000");

                    //manu info
                    for(idx = 0; idx < 8; idx++)
                    {
                        uint32_t value = eeprom_readint32(&eepFactoryConfig.productInfo[idx]);
                        buf[4 * idx + 0] = HI8_UINT(HI16_UINT32(value));
                        buf[4 * idx + 1] = LO8_UINT(HI16_UINT32(value));
                        buf[4 * idx + 2] = HI8_UINT(LO16_UINT32(value));
                        buf[4 * idx + 3] = LO8_UINT(LO16_UINT32(value));
                    }
                    //totally 32 bytes
                    sdi12Send(buf, 32, 0);
                }
                else
                {
                    //send error
                    sdi12SendError();
                }
                //SDI12协议要求
                g_sdiStateMachine = SDI_ADDRESS;
                break;
            }
            case 'A'://aAb!, Change Sensor Address, 已对照TEROS12测试
            {
                if(cmd[0] == 'A' && cmd[2] == '!')
                {
                    char newAddr = cmd[1];
                    if((newAddr >= '0' && newAddr <= '9') || (newAddr >= 'A' && newAddr <= 'Z') || (newAddr >= 'a' && newAddr <= 'z'))
                    {
                        eeprom_writeint32(&eepUserConfig.sdi12Address, newAddr);
                        crc32EepromAppend((uint32_t*)&eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));
                        g_sdiCommInfo.addr = newAddr;
                        char buf[1];
                        sdi12Send(buf, 0, 0);
                    }
                    else
                    {
                        //非法地址返回错误, 已对照TEROS12测试
                        //send error
                        sdi12SendError();
                    }
                }
                else
                {
                    //send error
                    sdi12SendError();
                }
                //SDI12协议要求
                g_sdiStateMachine = SDI_ADDRESS;
                break;
            }
            case 'M'://aM, Measure Command,已对照TEROS12测试
            {
                //SDI12协议要求V1.3
                //1. When a data recorder issues an M command, it must complete the command/response
                //sequence with the sensor before it sends any command to any other sensor. After the M command and before the
                //sensor issues the service request, the data recorder will not communicate with any other sensor.
                //The sensor must not drive the data line until it sends a service request. The recorder must
                //not drive the data line while waiting for the service request.
                //2. To comply with Version 1.2 or higher of SDI-12, sensors must respond to the additional
                //measurement commands (aM1! . . . aM9!) and data recorders must be able to log data from the
                //additional measurement commands. If a sensor has no data defined for an additional
                //measurement command, it should return a0000<CR><LF>, saying that it has zero data values
                //ready. Not responding to the command is not acceptable.
                //3. The data recorder must wait for either of two events to occur before issuing a D0 command:
                //------receipt of a service request (a<CR><LF>) from the sensor
                //------the specified time to elapse (ttt seconds)
                //4. After a sensor finishes its measurement procedure, the sensor must retain the measured
                //data in memory until it receives another M or V command, or another command that returns data via the D0 command.

                //aM!, aMC!,aM0!, aMC0!
                //Temperature
                if(strstr(&cmd[0], "M!") || strstr(&cmd[0], "MC!") ||
                        strstr(&cmd[0], "M0!") || strstr(&cmd[0], "MC0!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    ENUM_DCOMMANDRETTYPE commandRetType;
                    if(isCrcEnabled)
                    {
                        commandRetType = (cmd[2] == '0') ? RETTYPE_M0_CRC : RETTYPE_M_CRC;
                    }
                    else
                    {
                        commandRetType = (cmd[1] == '0') ? RETTYPE_M0 : RETTYPE_M;
                    }
                    g_sdiCommInfo.DCommandRetType = commandRetType;

                    //SDI12协议要求V1.3
                    //Caculate measure time
                    //If a sensor says it will take zero seconds before the data are ready, then a sensor must not issue a service request.
                    char buffer[16];
                    uint16_t warmUpTime = SensorGetMeasureTime();
                    sprintf(buffer, "%03d%d", warmUpTime, 2);
                    sdi12Send(buffer, 4, 0);

                    //Measure
                    int16_t errorCode = SensorStartMeasureAndWaitDone();

                    //SDI12协议要求V1.3
                    //SDI12 Break Occured Process
                    //If a sensor detects a break after it receives an M command, but before it issues a service request,
                    //it must abort its measurement procedure. The sensor address, followed by <CR><LF>,
                    //(or followed by <CRC><CR><LF> if a CRC was requested) should be returned in response to
                    //subsequent D commands. This requirement provides a way for a data recorder to abort a measurement
                    //so it can communicate with another sensor, or this sensor, on the SDI-12 bus.
                    char nextChar;
                    if(sdi12FetchChar(&nextChar) != 0)
                    {
                        if(nextChar == SDI12_CAHR_BREAK)
                        {
                            //SDI12 V1.3 Chapter 4.4.5.1
                            g_sdiCommInfo.DCommandRetType = (isCrcEnabled == true) ? RETTYPE_NODATA_CRC : RETTYPE_NODATA;
                            g_sdiStateMachine = SDI_FIRST_MARKING_DETECTING;
                            break;
                        }
                    }

                    //SDI12协议要求V1.3
                    //Sending Service request
                    //A service request is a response from a sensor. It is not a command. It is sent, after an M
                    //command, to tell the data recorder that the sensor has finished its measurement(s) and the data are ready.
                    //A service request is issued by the sensor after an M, MC, or V command, when it has finished its measurement.
                    sdi12Send(buffer, 0, 0);
                }
                //aM1!~aM9!,aMC1!~aMC9!
                //aM1!, aMC1!
                //Temperature
                else if(strstr(&cmd[0], "M1!") || strstr(&cmd[0], "MC1!")
                        || strstr(&cmd[0], "M2!") || strstr(&cmd[0], "MC2!")
                        || strstr(&cmd[0], "M3!") || strstr(&cmd[0], "MC3!")
                        || strstr(&cmd[0], "M4!") || strstr(&cmd[0], "MC4!")
                        || strstr(&cmd[0], "M5!") || strstr(&cmd[0], "MC5!")
                        || strstr(&cmd[0], "M6!") || strstr(&cmd[0], "MC6!")
                        || strstr(&cmd[0], "M7!") || strstr(&cmd[0], "MC7!")
                        || strstr(&cmd[0], "M8!") || strstr(&cmd[0], "MC8!")
                        || strstr(&cmd[0], "M9!") || strstr(&cmd[0], "MC9!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    uint8_t offset = 0;
                    char mIndex;
                    if(isCrcEnabled)
                    {
                        mIndex = cmd[2];
                        offset = mIndex - '1';
                    }
                    else
                    {
                        mIndex = cmd[1];
                        offset = mIndex - '1';
                    }
                    offset = offset * 2;
                    g_sdiCommInfo.DCommandRetType = (isCrcEnabled == true) ? (ENUM_DCOMMANDRETTYPE)(RETTYPE_M1_CRC + offset) : (ENUM_DCOMMANDRETTYPE)(RETTYPE_M1 + offset);

                    //计算返回数据的时间与个数
                    //根据SDI12协议中的SDI12-Timing图，需要在15ms内发送给主机，所以这里不能进行测量，要立即返回
                    uint8_t returnCount;
                    char buf[16];
                    switch(mIndex)
                    {
                    case '1':
                    {
                        returnCount = 4;

                        uint16_t warmUpTime = SensorGetMeasureTime();
                        sprintf(buf, "%03d%d", warmUpTime, returnCount);

                        break;
                    }
                    case '5':
                    {
                        returnCount = 2;

                        uint16_t warmUpTime = SensorGetMeasureTime();
                        sprintf(buf, "%03d%d", warmUpTime, returnCount);

                        break;
                    }
                    case '6':
                    {
                        returnCount = 6;

                        uint16_t warmUpTime = SensorGetMeasureTime();
                        sprintf(buf, "%03d%d", warmUpTime, returnCount);

                        break;
                    }
                    case '2':
                    case '3':
                    case '4':
                    case '7':
                    case '8':
                    case '9':
                    default:
                    {
                        returnCount = 0;
                        sprintf(buf, "%03d%d", 0, returnCount);
                        break;
                    }
                    }
                    sdi12Send(buf, 4, 0);


                    //进行数据测量
                    switch(mIndex)
                    {
                    case '1':
                    case '5':
                    case '6':
                    {
                        //Measure
                        int16_t errorCode = SensorStartMeasureAndWaitDone();
                        break;
                    }
                    case '2':
                    {
                        break;
                    }
                    case '3':
                    {
                        break;
                    }
                    case '4':
                    {
                        break;
                    }
                    case '7':
                    {
                        break;
                    }
                    case '8':
                    {
                        break;
                    }
                    case '9':
                    {
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }

                    char nextChar;
                    if(sdi12FetchChar(&nextChar) != 0)
                    {
                        if(nextChar == SDI12_CAHR_BREAK)
                        {
                            //SDI12 V1.3 Chapter 4.4.5.1
                            g_sdiCommInfo.DCommandRetType = (isCrcEnabled == true) ? RETTYPE_NODATA_CRC : RETTYPE_NODATA;
                            g_sdiStateMachine = SDI_FIRST_MARKING_DETECTING;
                            break;
                        }
                    }

                    //Service Request Needed only for valid aMx! command(by testing TEROS12)
                    //如果返回数据不为0个，则发送Service Request
                    if(returnCount != 0)
                    {
                        sdi12Send(buf, 0, 0);
                    }
                }
                else
                {
                    //send error
                    sdi12SendError();
                }
                //SDI12协议要求
                g_sdiStateMachine = SDI_ADDRESS;
                break;
            }
            case 'C':
            {
                //aC!, aCC!,aC0!, aCC0!
                //Temperature
                if(strstr(&cmd[0], "C!") || strstr(&cmd[0], "CC!") ||
                        strstr(&cmd[0], "C0!") || strstr(&cmd[0], "CC0!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    ENUM_DCOMMANDRETTYPE commandRetType;
                    if(isCrcEnabled)
                    {
                        commandRetType = (cmd[2] == '0') ? RETTYPE_M0_CRC : RETTYPE_M_CRC;
                    }
                    else
                    {
                        commandRetType = (cmd[1] == '0') ? RETTYPE_M0 : RETTYPE_M;
                    }
                    g_sdiCommInfo.DCommandRetType = commandRetType;

                    //SDI12协议要求V1.3
                    //Caculate measure time
                    //If a sensor says it will take zero seconds before the data are ready, then a sensor must not issue a service request.
                    char buffer[16];
                    uint16_t warmUpTime = SensorGetMeasureTime();
                    sprintf(buffer, "%03d%02d", warmUpTime, 2);
                    sdi12Send(buffer, 5, 0);

                    //Measure
                    int16_t errorCode = SensorStartMeasureAndWaitDone();
                }
                //aC1!~aC9!,aCC1!~aCC9!
                //aC1!, aCC1!
                //Temperature
                else if(strstr(&cmd[0], "C1!") || strstr(&cmd[0], "CC1!")
                        || strstr(&cmd[0], "C2!") || strstr(&cmd[0], "CC2!")
                        || strstr(&cmd[0], "C3!") || strstr(&cmd[0], "CC3!")
                        || strstr(&cmd[0], "C4!") || strstr(&cmd[0], "CC4!")
                        || strstr(&cmd[0], "C5!") || strstr(&cmd[0], "CC5!")
                        || strstr(&cmd[0], "C6!") || strstr(&cmd[0], "CC6!")
                        || strstr(&cmd[0], "C7!") || strstr(&cmd[0], "CC7!")
                        || strstr(&cmd[0], "C8!") || strstr(&cmd[0], "CC8!")
                        || strstr(&cmd[0], "C9!") || strstr(&cmd[0], "CC9!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    uint8_t offset = 0;
                    char mIndex;
                    if(isCrcEnabled)
                    {
                        mIndex = cmd[2];
                        offset = mIndex - '1';
                    }
                    else
                    {
                        mIndex = cmd[1];
                        offset = mIndex - '1';
                    }
                    offset = offset * 2;
                    g_sdiCommInfo.DCommandRetType = (isCrcEnabled == true) ? (ENUM_DCOMMANDRETTYPE)(RETTYPE_M1_CRC + offset) : (ENUM_DCOMMANDRETTYPE)(RETTYPE_M1 + offset);

                    //计算返回数据的时间与个数
                    //根据SDI12协议中的SDI12-Timing图，需要在15ms内发送给主机，所以这里不能进行测量，要立即返回
                    uint8_t returnCount;
                    char buf[16];
                    switch(mIndex)
                    {
                    case '1':
                    {
                        returnCount = 4;

                        uint16_t warmUpTime = SensorGetMeasureTime();
                        sprintf(buf, "%03d%02d", warmUpTime, returnCount);

                        break;
                    }
                    case '5':
                    {
                        returnCount = 2;

                        uint16_t warmUpTime = SensorGetMeasureTime();
                        sprintf(buf, "%03d%02d", warmUpTime, returnCount);

                        break;
                    }
                    case '6':
                    {
                        returnCount = 6;

                        uint16_t warmUpTime = SensorGetMeasureTime();
                        sprintf(buf, "%03d%02d", warmUpTime, returnCount);

                        break;
                    }
                    case '2':
                    case '3':
                    case '4':
                    case '7':
                    case '8':
                    case '9':
                    default:
                    {
                        returnCount = 0;
                        sprintf(buf, "%03d%02d", 0, returnCount);
                        break;
                    }
                    }
                    sdi12Send(buf, 5, 0);

                    //进行数据测量
                    switch(mIndex)
                    {
                    case '1':
                    case '5':
                    case '6':
                    {
                        //Mesaure
                        int16_t errorCode = SensorStartMeasureAndWaitDone();
                        break;
                    }
                    case '2':
                    {
                        break;
                    }
                    case '3':
                    {
                        break;
                    }
                    case '4':
                    {
                        break;
                    }
                    case '7':
                    {
                        break;
                    }
                    case '8':
                    {
                        break;
                    }
                    case '9':
                    {
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                else
                {
                    //send error
                    sdi12SendError();
                }
                //SDI12协议要求
                g_sdiStateMachine = SDI_STANDBY;
                break;
            }
            case 'D':
            {
                //SDI12协议要求V1.3
                //The maximum number of characters that can be returned in the <values> part of the
                //response to a D command is either 35 or 75. If the D command is issued to retrieve data in
                //response to a concurrent measurement command, the maximum is 75. Otherwise, the maximum is 35.

                //aD0!
                //作为对 M,MC, C, CC, or V 命令的响应
                if(strstr(&cmd[0], "D0!"))
                {
                    char buffer[128];
                    uint8_t bufferLength;
                    uint8_t isCrcEnabled = false;

                    if(g_sdiCommInfo.DCommandRetType == RETTYPE_M  || g_sdiCommInfo.DCommandRetType == RETTYPE_M_CRC ||
                            g_sdiCommInfo.DCommandRetType == RETTYPE_M0 || g_sdiCommInfo.DCommandRetType == RETTYPE_M0_CRC)
                    {
                        if(g_sdiCommInfo.DCommandRetType == RETTYPE_M_CRC || g_sdiCommInfo.DCommandRetType == RETTYPE_M0_CRC)
                        {
                            isCrcEnabled = true;
                        }
                        else
                        {
                            isCrcEnabled = false;
                        }

                        //Send data response
                        uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                        if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                        {
                            //根据量程进行小数点设置
                            uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                            float ecUnitDividerRefToUsCm = ECRangeTable[ecRangeIdx].ecUnitDividerRefToUsCm;
                            uint16_t ecDecimalNum = ECRangeTable[ecRangeIdx].ecDecimalNum;

                            float valueFloatECTRef = g_MValue.FinalECTRef / ecUnitDividerRefToUsCm;
                            //float valueFloatTDSTRef = g_MValue.FinalTdsTRef / ecUnitDividerRefToUsCm;
                            //float valueFloatSALTTRef = g_MValue.FinalSaltTRef / ecUnitDividerRefToUsCm;
                            //float valueFloatRESISTORTRef = g_MValue.FinalResistorTRef;

                            bufferLength = sprintf(buffer, "%+.2f%+.*f", g_MValue.FinalTemp, ecDecimalNum, valueFloatECTRef);
                        }
                        else
                        {
                            bufferLength = sprintf(buffer, "%+d%+d", SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                        }
                        sdi12Send(buffer, bufferLength, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M1 || g_sdiCommInfo.DCommandRetType == RETTYPE_M1_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M1_CRC) ? true : false;

                        //Send data response
                        uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                        if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                        {
                            //根据量程进行小数点设置
                            uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                            float ecUnitDividerRefToUsCm = ECRangeTable[ecRangeIdx].ecUnitDividerRefToUsCm;
                            uint16_t ecDecimalNum = ECRangeTable[ecRangeIdx].ecDecimalNum;

                            float valueFloatECTRef = g_MValue.FinalECTRef / ecUnitDividerRefToUsCm;
                            float valueFloatTDSTRef = g_MValue.FinalTdsTRef / ecUnitDividerRefToUsCm;
                            float valueFloatSALTTRef = g_MValue.FinalSaltTRef / ecUnitDividerRefToUsCm;

                            bufferLength = sprintf(buffer, "%+.2f%+.*f%+.*f%+.*f", g_MValue.FinalTemp, ecDecimalNum, valueFloatECTRef, ecDecimalNum, valueFloatTDSTRef, ecDecimalNum, valueFloatSALTTRef);
                        }
                        else
                        {
                            bufferLength = sprintf(buffer, "%+d%+d%+d%+d", SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                        }
                        sdi12Send(buffer, bufferLength, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M2 || g_sdiCommInfo.DCommandRetType == RETTYPE_M2_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M2_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M3 || g_sdiCommInfo.DCommandRetType == RETTYPE_M3_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M3_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M4 || g_sdiCommInfo.DCommandRetType == RETTYPE_M4_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M4_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M5 || g_sdiCommInfo.DCommandRetType == RETTYPE_M5_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M5_CRC) ? true : false;

                        //Send data response
                        uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                        if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                        {
                            bufferLength = sprintf(buffer, "%+.2f%+.2f", g_MValue.FinalTemp, g_MValue.FinalTempOrig);
                        }
                        else
                        {
                            bufferLength = sprintf(buffer, "%+d%+d", SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                        }
                        sdi12Send(buffer, bufferLength, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M6 || g_sdiCommInfo.DCommandRetType == RETTYPE_M6_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M6_CRC) ? true : false;

                        //Send data response
                        uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                        uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                        if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                        {
                            //根据量程进行小数点设置
                            uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                            float ecUnitDividerRefToUsCm = ECRangeTable[ecRangeIdx].ecUnitDividerRefToUsCm;
                            uint16_t ecDecimalNum = ECRangeTable[ecRangeIdx].ecDecimalNum;

                            float valueFloatECTRef = g_MValue.FinalECTRef / ecUnitDividerRefToUsCm;
                            //float valueFloatTDSTRef = g_MValue.FinalTdsTRef / ecUnitDividerRefToUsCm;
                            //float valueFloatSALTTRef = g_MValue.FinalSaltTRef / ecUnitDividerRefToUsCm;

                            bufferLength = sprintf(buffer, "%+d%+.2f%+.*f", ecRangeIdx, g_MValue.FinalTemp, ecDecimalNum, valueFloatECTRef);
                        }
                        else
                        {
                            bufferLength = sprintf(buffer, "%+d%+d%+d", ecRangeIdx, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                        }
                        sdi12Send(buffer, bufferLength, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M7 || g_sdiCommInfo.DCommandRetType == RETTYPE_M7_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M7_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M8 || g_sdiCommInfo.DCommandRetType == RETTYPE_M8_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M8_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M9 || g_sdiCommInfo.DCommandRetType == RETTYPE_M9_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M9_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_VERIFY)
                    {
                        bufferLength = sprintf(buffer, "%+d", g_MValue.SensorStatus);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_NODATA || g_sdiCommInfo.DCommandRetType == RETTYPE_NODATA_CRC)
                    {
                        uint8_t isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_NODATA_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }
                //aD1!-aD9!
                //作为对 M,MC, C, CC, or V 命令的响应，如果aD0!返回数据个数不够用，那么用aD1!~aD9!继续读取剩余数据
                else if(strstr(&cmd[0], "D1!")
                        || strstr(&cmd[0], "D2!")
                        || strstr(&cmd[0], "D3!")
                        || strstr(&cmd[0], "D4!")
                        || strstr(&cmd[0], "D5!")
                        || strstr(&cmd[0], "D6!")
                        || strstr(&cmd[0], "D7!")
                        || strstr(&cmd[0], "D8!")
                        || strstr(&cmd[0], "D9!"))
                {
                    //因为aM1~aM9, aC1~aC9号命令都返回 [addr]0000, 表示没有需要返回的数据。
                    //参考SDI-12协议V1.3，If a sensor has no data defined for an additional
                    //measurement command, it should return a0000<CR><LF>, saying that it has zero data values
                    //ready. Not responding to the command is not acceptable.
                    char buffer[128];
                    uint8_t bufferLength;
                    bufferLength = bufferLength; //Prevent compiling warning
                    uint8_t isCrcEnabled = false;
                    char DIndex = cmd[1];
                    DIndex = DIndex; //Prevent compiling warning

                    if(g_sdiCommInfo.DCommandRetType == RETTYPE_M  || g_sdiCommInfo.DCommandRetType == RETTYPE_M_CRC  ||
                            g_sdiCommInfo.DCommandRetType == RETTYPE_M0 || g_sdiCommInfo.DCommandRetType == RETTYPE_M0_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M_CRC  ||
                                        g_sdiCommInfo.DCommandRetType == RETTYPE_M0_CRC) ? true : false;

                        //需要支持aM！或者aM0！的aD1!-aD9!输出的在下面写就行，以下程序以D1和D2为例
                        if(DIndex == '1')
                        {
                            sdi12Send(buffer, 0, isCrcEnabled);
                        }
                        else if(DIndex == '2')
                        {
                            sdi12Send(buffer, 0, isCrcEnabled);
                        }
                        else
                        {
                            sdi12Send(buffer, 0, isCrcEnabled);
                        }
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M1 || g_sdiCommInfo.DCommandRetType == RETTYPE_M1_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M1_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M2 || g_sdiCommInfo.DCommandRetType == RETTYPE_M2_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M2_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M3 || g_sdiCommInfo.DCommandRetType == RETTYPE_M3_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M3_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M4 || g_sdiCommInfo.DCommandRetType == RETTYPE_M4_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M4_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M5 || g_sdiCommInfo.DCommandRetType == RETTYPE_M5_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M5_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M6 || g_sdiCommInfo.DCommandRetType == RETTYPE_M6_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M6_CRC) ? true : false;
                        if(DIndex == '1')
                        {
                            //Send data response
                            uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                            uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                            if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                            {
                                //根据量程进行小数点设置
                                uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                                float ecUnitDividerRefToUsCm = ECRangeTable[ecRangeIdx].ecUnitDividerRefToUsCm;
                                uint16_t ecDecimalNum = ECRangeTable[ecRangeIdx].ecDecimalNum;

                                float valueFloatECTRef = g_MValue.FinalECTRef / ecUnitDividerRefToUsCm;
                                float valueFloatTDSTRef = g_MValue.FinalTdsTRef / ecUnitDividerRefToUsCm;
                                float valueFloatSALTTRef = g_MValue.FinalSaltTRef / ecUnitDividerRefToUsCm;
                                float valueFloatRESISTORTRef = g_MValue.FinalResistorTRef;

                                bufferLength = sprintf(buffer, "%+.*f%+.*f%+.2f", ecDecimalNum, valueFloatTDSTRef, ecDecimalNum, valueFloatSALTTRef, valueFloatRESISTORTRef);
                            }
                            else
                            {
                                bufferLength = sprintf(buffer, "%+d%+d%+d", SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                            }
                            sdi12Send(buffer, bufferLength, isCrcEnabled);
                        }
                        else
                        {
                            sdi12Send(buffer, 0, isCrcEnabled);
                        }
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M7 || g_sdiCommInfo.DCommandRetType == RETTYPE_M7_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M7_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M8 || g_sdiCommInfo.DCommandRetType == RETTYPE_M8_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M8_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_M9 || g_sdiCommInfo.DCommandRetType == RETTYPE_M9_CRC)
                    {
                        isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_M9_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_VERIFY)
                    {
                        isCrcEnabled = false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                    else if(g_sdiCommInfo.DCommandRetType == RETTYPE_NODATA || g_sdiCommInfo.DCommandRetType == RETTYPE_NODATA_CRC)
                    {
                        uint8_t isCrcEnabled = (g_sdiCommInfo.DCommandRetType == RETTYPE_NODATA_CRC) ? true : false;
                        sdi12Send(buffer, 0, isCrcEnabled);
                    }
                }
                else
                {
                    //send error
                    sdi12SendError();
                }
                //SDI12协议要求
                g_sdiStateMachine = SDI_ADDRESS;
                break;
            }
            case 'R':
            {
                //SDI12协议要求V1.3
                //Sensors that are able to continuously monitor the phenomena to be measured, such as a
                //shaft encoder, do not require a start measurement command (M!, M1! . . . M9!). They can be
                //read directly with the R commands (R0! ... R9!).
                //The maximum number of characters that can be returned in the <values> part of the response to an R command is 75.
                //
                //If a sensor is unable to take a continuous measurement, then it must return its address
                //followed by a carriage return/line feed (a<CR><LF>) in response to an R command. If a CRC
                //was requested then the <CR><LF> must be preceded by the CRC.

                //aR0!, aRC0!
                if(strstr(&cmd[0], "R0!") || strstr(&cmd[0], "RC0!"))
                {
                    char buffer[128];
                    uint8_t bufferLength;
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;

                    //Measure
                    int16_t errorCode = SensorStartMeasureAndWaitDone();

                    //Send data response
                    uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                    if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                    {
                        //根据量程进行小数点设置
                        uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                        float ecUnitDividerRefToUsCm = ECRangeTable[ecRangeIdx].ecUnitDividerRefToUsCm;
                        uint16_t ecDecimalNum = ECRangeTable[ecRangeIdx].ecDecimalNum;

                        float valueFloatECTRef = g_MValue.FinalECTRef / ecUnitDividerRefToUsCm;
                        //float valueFloatTDSTRef = g_MValue.FinalTdsTRef / ecUnitDividerRefToUsCm;
                        //float valueFloatSALTTRef = g_MValue.FinalSaltTRef / ecUnitDividerRefToUsCm;

                        bufferLength = sprintf(buffer, "%+.2f%+.*f", g_MValue.FinalTemp, ecDecimalNum, valueFloatECTRef);
                    }
                    else
                    {
                        bufferLength = sprintf(buffer, "%+d%+d", SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                    }
                    sdi12Send(buffer, bufferLength, isCrcEnabled);
                }
                //aR1!, aRC1!
                else if(strstr(&cmd[0], "R1!") || strstr(&cmd[0], "RC1!"))
                {
                    char buffer[128];
                    uint8_t bufferLength;
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;

                    //Measure
                    int16_t errorCode = SensorStartMeasureAndWaitDone();

                    //Send data response
                    uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                    if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                    {
                        //根据量程进行小数点设置
                        uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                        float ecUnitDividerRefToUsCm = ECRangeTable[ecRangeIdx].ecUnitDividerRefToUsCm;
                        uint16_t ecDecimalNum = ECRangeTable[ecRangeIdx].ecDecimalNum;

                        float valueFloatECTRef = g_MValue.FinalECTRef / ecUnitDividerRefToUsCm;
                        float valueFloatTDSTRef = g_MValue.FinalTdsTRef / ecUnitDividerRefToUsCm;
                        float valueFloatSALTTRef = g_MValue.FinalSaltTRef / ecUnitDividerRefToUsCm;

                        bufferLength = sprintf(buffer, "%+.2f%+.*f%+.*f%+.*f", g_MValue.FinalTemp, ecDecimalNum, valueFloatECTRef, ecDecimalNum, valueFloatTDSTRef, ecDecimalNum, valueFloatSALTTRef);
                    }
                    else
                    {
                        bufferLength = sprintf(buffer, "%+d%+d%+d%+d", SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                    }
                    sdi12Send(buffer, bufferLength, isCrcEnabled);
                }
                //aR2!, aRC2!
                else if(strstr(&cmd[0], "R2!") || strstr(&cmd[0], "RC2!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    char buf[1];
                    sdi12Send(buf, 0, isCrcEnabled);
                }
                //aR3!, aRC3!
                else if(strstr(&cmd[0], "R3!") || strstr(&cmd[0], "RC3!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    char buf[1];
                    sdi12Send(buf, 0, isCrcEnabled);
                }
                //aR4!, aRC4!
                else if(strstr(&cmd[0], "R4!") || strstr(&cmd[0], "RC4!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    char buf[1];
                    sdi12Send(buf, 0, isCrcEnabled);
                }
                //aR5!, aRC5!
                else if(strstr(&cmd[0], "R5!") || strstr(&cmd[0], "RC5!"))
                {
                    char buffer[128];
                    uint8_t bufferLength;
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;

                    //Measure
                    int16_t errorCode = SensorStartMeasureAndWaitDone();

                    //Send data response
                    uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                    if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                    {
                        bufferLength = sprintf(buffer, "%+.2f%+.2f", g_MValue.FinalTemp, g_MValue.FinalTempOrig);
                    }
                    else
                    {
                        bufferLength = sprintf(buffer, "%+d%+d", SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                    }
                    sdi12Send(buffer, bufferLength, isCrcEnabled);
                }
                //aR6!, aRC6!
                else if(strstr(&cmd[0], "R6!") || strstr(&cmd[0], "RC6!"))
                {
                    char buffer[128];
                    uint8_t bufferLength;
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;

                    //Measure
                    int16_t errorCode = SensorStartMeasureAndWaitDone();

                    //Send data response
                    uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                    uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                    if(g_MValue.SensorStatus == SENSOR_STATUS_OK)
                    {
                        //根据量程进行小数点设置
                        uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                        float ecUnitDividerRefToUsCm = ECRangeTable[ecRangeIdx].ecUnitDividerRefToUsCm;
                        uint16_t ecDecimalNum = ECRangeTable[ecRangeIdx].ecDecimalNum;

                        float valueFloatECTRef = g_MValue.FinalECTRef / ecUnitDividerRefToUsCm;
                        float valueFloatTDSTRef = g_MValue.FinalTdsTRef / ecUnitDividerRefToUsCm;
                        float valueFloatSALTTRef = g_MValue.FinalSaltTRef / ecUnitDividerRefToUsCm;

                        bufferLength = sprintf(buffer, "%+d%+.2f%+.*f%+.*f%+.*f%+.2f", ecRangeIdx, g_MValue.FinalTemp, ecDecimalNum, valueFloatECTRef, ecDecimalNum, valueFloatTDSTRef, ecDecimalNum, valueFloatSALTTRef, g_MValue.FinalResistorTRef);
                    }
                    else
                    {
                        bufferLength = sprintf(buffer, "%+d%+d%+d%+d%+d%+d", ecRangeIdx, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12, SENSOR_ERRORCODE_BROKEN_ForSDI12);
                    }
                    sdi12Send(buffer, bufferLength, isCrcEnabled);
                }
                //aR7!, aRC7!
                else if(strstr(&cmd[0], "R7!") || strstr(&cmd[0], "RC7!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    char buf[1];
                    sdi12Send(buf, 0, isCrcEnabled);
                }
                //aR8!, aRC8!
                else if(strstr(&cmd[0], "R8!") || strstr(&cmd[0], "RC8!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    char buf[1];
                    sdi12Send(buf, 0, isCrcEnabled);
                }
                //aR9!, aRC9!
                else if(strstr(&cmd[0], "R9!") || strstr(&cmd[0], "RC9!"))
                {
                    uint8_t isCrcEnabled = (cmd[1] == 'C') ? true : false;
                    char buf[1];
                    sdi12Send(buf, 0, isCrcEnabled);
                }
                else
                {
                    //send error
                    sdi12SendError();
                }
                //SDI12协议要求
                g_sdiStateMachine = SDI_ADDRESS;
                break;
            }

            case 'X': //aXxxxxxxx! , Extension Command
            {
                uint8_t cmdCounter = g_sdiCommInfo.vm_CommandBufferIndex;

                //Get Debug MSVEC
                //Request: aXR_MSVEC!
                if(strstr(&cmd[0], "XR_MSVEC!"))
                {
                    char buffer[192];
                    uint8_t bufferLength;

                    //Measure, 这里不进行测量，因为调试时会进行aRx测量
                    //int16_t errorCode = SensorStartMeasureAndWaitDone();

                    float AD5934MagnitudeHH = eeprom_readfloat((uint32_t*) & (eepCalibRaw.AD5934MagnitudeHH));
                    float AD5934MagnitudeHL = eeprom_readfloat((uint32_t*) & (eepCalibRaw.AD5934MagnitudeHL));
                    float AD5934MagnitudeMH = eeprom_readfloat((uint32_t*) & (eepCalibRaw.AD5934MagnitudeMH));
                    float AD5934MagnitudeML = eeprom_readfloat((uint32_t*) & (eepCalibRaw.AD5934MagnitudeML));
                    float AD5934MagnitudeLH = eeprom_readfloat((uint32_t*) & (eepCalibRaw.AD5934MagnitudeLH));
                    float AD5934MagnitudeLL = eeprom_readfloat((uint32_t*) & (eepCalibRaw.AD5934MagnitudeLL));

                    bufferLength = sprintf(buffer, "M=%.2f,MHH=%.2f,MHL=%.2f,MMH=%.2f,MML=%.2f,MLH=%.2f,MLL=%.2f,ROrigTx=%.2f,RTRef=%.2f,RgnSel=%d,MDone=%d,MTime=%d",
                                           g_MValue.AD5934Magnitude, AD5934MagnitudeHH, AD5934MagnitudeHL, AD5934MagnitudeMH, AD5934MagnitudeML, AD5934MagnitudeLH, AD5934MagnitudeLL,
                                           g_MValue.FinalResistorOrigTx, g_MValue.FinalResistorTRef,
                                           g_MValue.AD5934RangeSelection, g_MValue.SensorMeasureStatus, g_MValue.measTimeCompletedAsync);
                    sdi12Send(buffer, bufferLength, 0);
                }
                //Get Debug MSVT
                //Request: aXR_MSVT!
                else if(strstr(&cmd[0], "XR_MSVT!"))
                {
                    char buffer[192];
                    uint8_t bufferLength;

                    //Measure, 这里不进行测量，因为调试时会进行aRx测量
                    //int16_t errorCode = SensorStartMeasureAndWaitDone();

                    int16_t calibValuePT1K0 = (int32_t)eeprom_readint32((uint32_t*)&eepCalibRaw.CalibRawPT1000AD[0]);
                    int16_t calibValuePT1K1 = (int32_t)eeprom_readint32((uint32_t*)&eepCalibRaw.CalibRawPT1000AD[1]);

                    bufferLength = sprintf(buffer, "ADntc=%d,Tntc=%.2f,ADpt=%d,Tpt=%.2f,Cpt0=%d,Cpt1=%d",
                                           (int32_t)g_MValue.FinalTempNTCRawAD, g_MValue.FinalTemp, (int32_t)g_MValue.FinalTempPT1000RawAD, g_MValue.FinalTemp,
                                           calibValuePT1K0, calibValuePT1K1);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Temperature Unit Set
                //Request: aXW_TUNIT_x!
                //Response:aTUNIT=x
                //x=(T,F)
                else if(strstr(&cmd[0], "XW_TUNIT_") && cmd[cmdCounter - 1] == '!')
                {
                    char tempUnit = cmd[9];
                    if(tempUnit == CONST_TEMPUNIT_C || tempUnit == CONST_TEMPUNIT_F)
                    {
                        eeprom_writeint32((uint32_t*)&eepUserConfig.temperatureUnit, (int32_t)tempUnit);
                        crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));

                        tempUnit = (char)eeprom_readint32((uint32_t*)&eepUserConfig.temperatureUnit);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "TUNIT=%c", tempUnit);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Temperature Unit Get
                //Request: aXR_TUNIT!
                //Response:aTUNIT=x
                else if(strstr(&cmd[0], "XR_TUNIT!"))
                {
                    char tempUnit = (char)eeprom_readint32((uint32_t*)&eepUserConfig.temperatureUnit);
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "TUNIT=%c", tempUnit);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set T Offset
                //Request: aXW_TOFFSET_xxxxxx!
                //Response:aTOFFSET=xxxxxx
                else if(strstr(&cmd[0], "XW_TOFFSET_") && cmd[cmdCounter - 1] == '!')
                {
                    float tempOffset = 0;
                    tempOffset = atof(&cmd[11]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                    if((tempOffset >= CONST_TEMPOFFSET_MIN) && (tempOffset <= CONST_TEMPOFFSET_MAX))
                    {
                        eeprom_writefloat((uint32_t*)&eepUserConfig.temperatureOffset, tempOffset);
                        crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));

                        tempOffset = eeprom_readfloat((uint32_t*)&eepUserConfig.temperatureOffset);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "TOFFSET=%.2f", tempOffset);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get T Offset
                //Request: aXR_TOFFSET!
                //Response:aTOFFSET=xxxxxx
                else if(strstr(&cmd[0], "XR_TOFFSET!"))
                {
                    float tempOffset = 0;
                    tempOffset = eeprom_readfloat((uint32_t*)&eepUserConfig.temperatureOffset);

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "TOFFSET=%.2f", tempOffset);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Filter Window
                //Request: aXW_FILTER_x!
                //Response:aFILTER=x
                //x=(T,F)
                else if(strstr(&cmd[0], "XW_FILTER_") && cmd[cmdCounter - 1] == '!')
                {
                    uint16_t filterWindow = atoi(&cmd[10]);
                    if(filterWindow <= CONST_FILTERWINDOW_MAX)
                    {
                        eeprom_writeint32((uint32_t*)&eepUserConfig.filterWindow, (int32_t)filterWindow);
                        crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));

                        filterWindow = (uint16_t)eeprom_readint32((uint32_t*)&eepUserConfig.filterWindow);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "FILTER=%d", filterWindow);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Filter Window
                //Request: aXR_FILTER!
                //Response:aFILTER=x
                else if(strstr(&cmd[0], "XR_FILTER!"))
                {
                    uint16_t filterWindow = (uint16_t)eeprom_readint32((uint32_t*)&eepUserConfig.filterWindow);
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "FILTER=%d", filterWindow);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Async Acq Interval
                //Request: aXW_AAI_x!
                //Response:aAAI=x
                //x=(T,F)
                else if(strstr(&cmd[0], "XW_AAI_") && cmd[cmdCounter - 1] == '!')
                {
                    uint16_t asyncAcqInterval = atoi(&cmd[7]);
                    if(asyncAcqInterval == CONST_ASYNCACQINTERVAL_DISABLED || ((asyncAcqInterval >= CONST_ASYNCACQINTERVAL_MIN) && (asyncAcqInterval <= CONST_ASYNCACQINTERVAL_MAX)))
                    {
                        eeprom_writeint32((uint32_t*)&eepUserConfig.asyncAcqInterval, (int32_t)asyncAcqInterval);
                        crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(uint8_t));

                        asyncAcqInterval = eeprom_readint32((uint32_t*)&eepUserConfig.asyncAcqInterval);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "AAI=%d", asyncAcqInterval);
                        sdi12Send(buffer, bufferLength, 0);

                        NVIC_SystemReset();
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Async Acq Interval
                //Request: aXR_AAI!
                //Response:aAAI=x
                else if(strstr(&cmd[0], "XR_AAI!"))
                {
                    uint16_t asyncAcqInterval = (uint16_t)eeprom_readint32((uint32_t*)&eepUserConfig.asyncAcqInterval);
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "AAI=%d", asyncAcqInterval);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Temperature Source
                //Request: aXW_TSOURCE_x!
                //Response:aTSOURCE=x
                //x=(T,F)
                else if(strstr(&cmd[0], "XW_TSRC_") && cmd[cmdCounter - 1] == '!')
                {
                    uint16_t tempSource = atoi(&cmd[8]);
                    if(tempSource == CONST_TEMPSOURCE_SENSOR || tempSource == CONST_TEMPSOURCE_MANUALINPUT)
                    {
                        eeprom_writeint32((uint32_t*)&eepSensorConfig.tempSource, (int32_t)tempSource);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        tempSource = (uint16_t)eeprom_readint32((uint32_t*)&eepSensorConfig.tempSource);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "TSRC=%d", tempSource);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Temperature Source
                //Request: aXR_TSOURCE!
                //Response:aTSOURCE=x
                else if(strstr(&cmd[0], "XR_TSRC!"))
                {
                    uint16_t tempSource = (uint16_t)eeprom_readint32((uint32_t*)&eepSensorConfig.tempSource);
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "TSRC=%d", tempSource);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set T Manual Input
                //Request: aXW_TMI_xxxxxx!
                //Response:aTMI=xxxxxx
                else if(strstr(&cmd[0], "XW_TMI_") && cmd[cmdCounter - 1] == '!')
                {
                    float tempManualInput = 0;
                    tempManualInput = atof(&cmd[7]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                    if((tempManualInput >= CONST_TEMP_MANUALINPUT_MIN) && (tempManualInput <= CONST_TEMP_MANUALINPUT_MAX))
                    {
                        eeprom_writefloat((uint32_t*)&eepSensorConfig.tempManualInput, tempManualInput);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        tempManualInput = eeprom_readfloat((uint32_t*)&eepSensorConfig.tempManualInput);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "TMI=%.2f", tempManualInput);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get T Manual Input
                //Request: aXR_TMI!
                //Response:aTMI=xxxxxx
                else if(strstr(&cmd[0], "XR_TMI!"))
                {
                    float tempManualInput = 0;
                    tempManualInput = eeprom_readfloat((uint32_t*)&eepSensorConfig.tempManualInput);

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "TMI=%.2f", tempManualInput);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set EC Reference Temp
                //Request: aXW_ECRT_xxxxxx!
                //Response:aECRT=xxxxxx
                else if(strstr(&cmd[0], "XW_ECRT_") && cmd[cmdCounter - 1] == '!')
                {
                    float ecRefTemperature = 0;
                    ecRefTemperature = atof(&cmd[8]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                    if((ecRefTemperature >= CONST_ECREFTEMPERATURE_MIN) && (ecRefTemperature <= CONST_ECREFTEMPERATURE_MAX))
                    {
                        eeprom_writefloat((uint32_t*)&eepSensorConfig.ecRefTemperature, ecRefTemperature);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        ecRefTemperature = eeprom_readfloat((uint32_t*)&eepSensorConfig.ecRefTemperature);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "ECRT=%.2f", ecRefTemperature);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get EC Reference Temp
                //Request: aXR_ECRT!
                //Response:aECRT=xxxxxx
                else if(strstr(&cmd[0], "XR_ECRT!"))
                {
                    float ecRefTemperature = 0;
                    ecRefTemperature = eeprom_readfloat((uint32_t*)&eepSensorConfig.ecRefTemperature);

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "ECRT=%.2f", ecRefTemperature);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set EC Compensation Mode
                //Request: aXW_ECTCM_x!
                //Response:aECTCM=x
                //x=(T,F)
                else if(strstr(&cmd[0], "XW_ECTCM_") && cmd[cmdCounter - 1] == '!')
                {
                    uint16_t ecTCompensationMode = atoi(&cmd[9]);
                    if(ecTCompensationMode == CONST_ECTCOMPENSATIONMODE_DISABLE || ecTCompensationMode == CONST_ECTCOMPENSATIONMODE_LINEAR)
                    {
                        eeprom_writeint32((uint32_t*)&eepSensorConfig.ecTCompensationMode, (int32_t)ecTCompensationMode);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        ecTCompensationMode = (uint16_t)eeprom_readint32((uint32_t*)&eepSensorConfig.ecTCompensationMode);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "ECTCM=%d", ecTCompensationMode);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get EC Compensation Mode
                //Request: aXR_ECTCM!
                //Response:aECTCM=x
                else if(strstr(&cmd[0], "XR_ECTCM!"))
                {
                    uint16_t ecTCompensationMode = (uint16_t)eeprom_readint32((uint32_t*)&eepSensorConfig.ecTCompensationMode);
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "ECTCM=%d", ecTCompensationMode);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set EC Calib Mode
                //Request: aXW_ECCM_x!
                //Response:aECCM=x
                //x=(T,F)
                else if(strstr(&cmd[0], "XW_ECCM_") && cmd[cmdCounter - 1] == '!')
                {
                    uint16_t ecCalibMode = atoi(&cmd[8]);
                    if(ecCalibMode == CONST_ECCALIBMODE_CONSTANT || ecCalibMode == CONST_ECCALIBMODE_SOLUTION)
                    {
                        eeprom_writeint32((uint32_t*)&eepSensorConfig.ecCalibMode, (int32_t)ecCalibMode);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        ecCalibMode = (uint16_t)eeprom_readint32((uint32_t*)&eepSensorConfig.ecCalibMode);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "ECCM=%d", ecCalibMode);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get EC Calib Mode
                //Request: aXR_ECCM!
                //Response:aECCM=x
                else if(strstr(&cmd[0], "XR_ECCM!"))
                {
                    uint16_t ecCalibMode = (uint16_t)eeprom_readint32((uint32_t*)&eepSensorConfig.ecCalibMode);
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "ECCM=%d", ecCalibMode);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set Cell Constant Value
                //Request: aXW_CCV_xxxxxx!
                //Response:aCCV=xxxxxx
                else if(strstr(&cmd[0], "XW_CCV_") && cmd[cmdCounter - 1] == '!')
                {
                    float cellConstantValue = 0;
                    cellConstantValue = atof(&cmd[7]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                    if((cellConstantValue >= CONST_CELLCONSTANTVALUE_MIN) && (cellConstantValue <= CONST_CELLCONSTANTVALUE_MAX))
                    {
                        eeprom_writefloat((uint32_t*)&eepSensorConfig.cellConstantValue, cellConstantValue);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        cellConstantValue = eeprom_readfloat((uint32_t*)&eepSensorConfig.cellConstantValue);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "CCV=%.4f", cellConstantValue);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get Cell Constant Value
                //Request: aXR_CCV!
                //Response:aCCV=xxxxxx
                else if(strstr(&cmd[0], "XR_CCV!"))
                {
                    float cellConstantValue = 0;
                    cellConstantValue = eeprom_readfloat((uint32_t*)&eepSensorConfig.cellConstantValue);

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "CCV=%.4f", cellConstantValue);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set coef EC Compensation
                //Request: aXW_COEFECC_xxxxxx!
                //Response:aCOEFECC=xxxxxx
                else if(strstr(&cmd[0], "XW_COEFECTC_") && cmd[cmdCounter - 1] == '!')
                {
                    float coefECTCompensation = 0;
                    coefECTCompensation = atof(&cmd[12]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                    if((coefECTCompensation >= CONST_COEF_ECTCOMPENSATION_MIN) && (coefECTCompensation <= CONST_COEF_ECTCOMPENSATION_MAX))
                    {
                        eeprom_writefloat((uint32_t*)&eepSensorConfig.coefECTCompensation, coefECTCompensation);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        coefECTCompensation = eeprom_readfloat((uint32_t*)&eepSensorConfig.coefECTCompensation);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "COEFECTC=%.3f", coefECTCompensation);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get coef EC Compensation
                //Request: aXR_COEFECC!
                //Response:aCOEFEC=xxxxxx
                else if(strstr(&cmd[0], "XR_COEFECTC!"))
                {
                    float coefECTCompensation = 0;
                    coefECTCompensation = eeprom_readfloat((uint32_t*)&eepSensorConfig.coefECTCompensation);

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "COEFECTC=%.3f", coefECTCompensation);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set coef TDS
                //Request: aXW_COEFTDS_xxxxxx!
                //Response:aCOEFTDS=xxxxxx
                else if(strstr(&cmd[0], "XW_COEFTDS_") && cmd[cmdCounter - 1] == '!')
                {
                    float coefTDS = 0;
                    coefTDS = atof(&cmd[11]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                    if((coefTDS >= CONST_COEF_TDS_MIN) && (coefTDS <= CONST_COEF_TDS_MAX))
                    {
                        eeprom_writefloat((uint32_t*)&eepSensorConfig.coefTDS, coefTDS);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        coefTDS = eeprom_readfloat((uint32_t*)&eepSensorConfig.coefTDS);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "COEFTDS=%.3f", coefTDS);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get coef TDS
                //Request: aXR_COEFTDS!
                //Response:aCOEFTDS=xxxxxx
                else if(strstr(&cmd[0], "XR_COEFTDS!"))
                {
                    float coefTDS = 0;
                    coefTDS = eeprom_readfloat((uint32_t*)&eepSensorConfig.coefTDS);

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "COEFTDS=%.3f", coefTDS);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set coef Salinity
                //Request: aXW_COEFSAL_xxxxxx!
                //Response:aCOEFSAL=xxxxxx
                else if(strstr(&cmd[0], "XW_COEFSAL_") && cmd[cmdCounter - 1] == '!')
                {
                    float coefSalinity = 0;
                    coefSalinity = atof(&cmd[11]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                    if((coefSalinity >= CONST_COEF_SALINITY_MIN) && (coefSalinity <= CONST_COEF_SALINITY_MAX))
                    {
                        eeprom_writefloat((uint32_t*)&eepSensorConfig.coefSalinity, coefSalinity);
                        crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                        coefSalinity = eeprom_readfloat((uint32_t*)&eepSensorConfig.coefSalinity);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "COEFSAL=%.3f", coefSalinity);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get coef Salinity
                //Request: aXR_COEFSAL!
                //Response:aCOEFSAL=xxxxxx
                else if(strstr(&cmd[0], "XR_COEFSAL!"))
                {
                    float coefSalinity = 0;
                    coefSalinity = eeprom_readfloat((uint32_t*)&eepSensorConfig.coefSalinity);

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    bufferLength = sprintf(buffer, "COEFSAL=%.3f", coefSalinity);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set EC Calib Air
                //Request: aXW_ECCALAIR!
                //Response:aECCALAIR=0,xxxx
                else if(strstr(&cmd[0], "XW_ECCALAIR!"))
                {
                    //Measure
                    int16_t errorCode = SensorStartMeasureAndWaitDone();
                    if(errorCode != 0)
                    {
                        g_MValue.CalibResultPointZero = (int16_t)CONST_CALIBRESULT_ERROR_SENSOR;
                    }
                    else
                    {
                        g_MValue.CalibResultPointZero = (int16_t)CONST_CALIBRESULT_SUCCESS;
                    }

                    uint32_t targetValue = 0;
                    uint32_t internalValue = (uint32_t)(g_MValue.FinalECTRefForCalib * 100.00);
                    eeprom_writeint32((uint32_t*)&eepCalibUser.calibPointZero.targetValue, targetValue);
                    eeprom_writeint32((uint32_t*)&eepCalibUser.calibPointZero.internalValue, internalValue);
                    crc32EepromAppend((uint32_t*)&eepCalibUser, sizeof(STRUCT_CALIBUSER) / sizeof(uint8_t));

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    internalValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointZero.internalValue);

                    bufferLength = sprintf(buffer, "ECCALAIR=%d,%d,%d", g_MValue.CalibResultPointZero, targetValue, internalValue);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Get EC Calib Air
                //Request: aXR_ECCAL0!
                //Response:aECCAL0=0,xxxx
                else if(strstr(&cmd[0], "XR_ECCALAIR!"))
                {
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    uint32_t targetValue = 0;
                    uint32_t internalValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointZero.internalValue);

                    bufferLength = sprintf(buffer, "ECCALAIR=%d,%d,%d", g_MValue.CalibResultPointZero, targetValue, internalValue);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set EC Calib Cell Constant
                //Request: aXW_ECCALCC_xxxx!
                //Response:aECCALCC=xxxx,xxxx
                else if(strstr(&cmd[0], "XW_ECCALCC_") && cmd[cmdCounter - 1] == '!')
                {
                    uint32_t targetValue = 0;
                    targetValue = atoi(&cmd[11]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。

                    uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                    uint32_t ecRangeMax = ECRangeTable[ecRangeIdx].ecRangeMax;

                    uint32_t internalValue;
                    if((targetValue < 1) || (targetValue > ecRangeMax))
                    {
                        //标准液数值超限制
                        g_MValue.CalibResultPointFor1Pt = (int16_t)CONST_CALIBRESULT_ERROR_TARGETVALUE;
                    }
                    else
                    {
                        //Measure
                        int16_t errorCode = SensorStartMeasureAndWaitDone();
                        if(errorCode != 0)
                        {
                            g_MValue.CalibResultPointFor1Pt = (int16_t)CONST_CALIBRESULT_ERROR_SENSOR;
                        }
                        else
                        {
                            //计算电极常数并保存
                            float cellConstantCalculated = ((float)targetValue) / g_MValue.FinalECTRefForCalib;
                            if((cellConstantCalculated < CONST_CELLCONSTANTVALUE_MIN) || (cellConstantCalculated > CONST_CELLCONSTANTVALUE_MAX))
                            {
                                g_MValue.CalibResultPointFor1Pt = (int16_t)CONST_CALIBRESULT_ERROR_TARGETVALUE;
                            }
                            else
                            {
                                g_MValue.CalibResultPointFor1Pt = (int16_t)CONST_CALIBRESULT_SUCCESS;

                                internalValue = (uint32_t)(g_MValue.FinalECTRefForCalib * 100.00);
                                //保存原始数据
                                eeprom_writeint32((uint32_t*)&eepCalibUser.calibPointFor1Pt.targetValue, targetValue);
                                eeprom_writeint32((uint32_t*)&eepCalibUser.calibPointFor1Pt.internalValue, internalValue);
                                crc32EepromAppend((uint32_t*)&eepCalibUser, sizeof(STRUCT_CALIBUSER) / sizeof(uint8_t));
                                //float cellConstantDivideValue = 10000;//10000/10000=1
                                eeprom_writefloat((uint32_t*)&eepSensorConfig.cellConstantValue, cellConstantCalculated);
                            }
                        }
                    }
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    targetValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor1Pt.targetValue);
                    internalValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor1Pt.internalValue);

                    bufferLength = sprintf(buffer, "ECCALCC=%d,%d,%d", g_MValue.CalibResultPointFor1Pt, targetValue, internalValue);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Get EC Calib Cell Constant
                //Request: aXR_ECCALCC_xxxx!
                //Response:aECCALCC=xxxx,xxxx
                else if(strstr(&cmd[0], "XR_ECCALCC!"))
                {
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                    uint32_t targetValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor1Pt.targetValue);
                    uint32_t internalValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor1Pt.internalValue);

                    bufferLength = sprintf(buffer, "ECCALCC=%d,%d,%d", g_MValue.CalibResultPointFor1Pt, targetValue, internalValue);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set EC Calib Solution
                //Request: aXW_ECCALSLx_xxxx!
                //Response:aECCALSLx=xxxx,xxxx
                else if(strstr(&cmd[0], "XW_ECCALSL") && strstr(&cmd[11], "_") && cmd[cmdCounter - 1] == '!')
                {
                    uint16_t calibPointIndex = cmd[10] - '0';
                    if(calibPointIndex < CONST_CALIBPOINT_NUM)
                    {
                        uint32_t targetValue = atoi(&cmd[12]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                        uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                        uint32_t ecRangeMax = ECRangeTable[ecRangeIdx].ecRangeMax;

                        uint32_t internalValue;
                        if((targetValue < 1) || (targetValue > ecRangeMax))
                        {
                            //标准液数值超限制
                            g_MValue.CalibResultPointFor2Pt[calibPointIndex] = (int16_t)CONST_CALIBRESULT_ERROR_TARGETVALUE;
                        }
                        else
                        {
                            //Measure
                            int16_t errorCode = SensorStartMeasureAndWaitDone();

                            if(errorCode != 0)
                            {
                                g_MValue.CalibResultPointFor2Pt[calibPointIndex] = (int16_t)CONST_CALIBRESULT_ERROR_SENSOR;
                            }
                            else
                            {
                                int32_t targetValueLow = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor2Pt[0].targetValue);
                                int32_t targetValueHigh = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor2Pt[1].targetValue);
                                int16_t isValidateTargetValueFailed = 0;

                                if(calibPointIndex == 0)
                                {
                                    //低校准点，是否目标值高于高校准点
                                    if(targetValue >= targetValueHigh)
                                    {
                                        isValidateTargetValueFailed = 1;
                                    }
                                }
                                else if(calibPointIndex == 1)
                                {
                                    //高校准点，是否目标值低于低校准点
                                    if(targetValue <= targetValueLow)
                                    {
                                        isValidateTargetValueFailed = 1;
                                    }
                                }

                                if(isValidateTargetValueFailed > 0)
                                {
                                    g_MValue.CalibResultPointFor2Pt[calibPointIndex] = (int16_t)CONST_CALIBRESULT_ERROR_TARGETVALUE;
                                }
                                else
                                {
                                    g_MValue.CalibResultPointFor2Pt[calibPointIndex] = (int16_t)CONST_CALIBRESULT_SUCCESS;

                                    internalValue = (uint32_t)(g_MValue.FinalECTRefForCalib * 100.00);

                                    eeprom_writeint32((uint32_t*)&eepCalibUser.calibPointFor2Pt[calibPointIndex].targetValue, targetValue);
                                    eeprom_writeint32((uint32_t*)&eepCalibUser.calibPointFor2Pt[calibPointIndex].internalValue, internalValue);
                                    crc32EepromAppend((uint32_t*)&eepCalibUser, sizeof(STRUCT_CALIBUSER) / sizeof(uint8_t));
                                }
                            }
                        }
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        targetValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor2Pt[calibPointIndex].targetValue);
                        internalValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor2Pt[calibPointIndex].internalValue);

                        bufferLength = sprintf(buffer, "ECCALSL%d=%d,%d,%d", calibPointIndex, g_MValue.CalibResultPointFor2Pt[calibPointIndex], targetValue, internalValue);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Get EC Calib Solution
                //Request: aXR_ECCALSLx!
                //Response:aECCALSLx=xxxx,xxxx
                else if(strstr(&cmd[0], "XR_ECCALSL") && cmd[cmdCounter - 1] == '!')
                {
                    uint16_t calibPointIndex = cmd[10] - '0';
                    if(calibPointIndex < CONST_CALIBPOINT_NUM)
                    {
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                        uint32_t targetValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor2Pt[calibPointIndex].targetValue);
                        uint32_t internalValue = eeprom_readint32((uint32_t*)&eepCalibUser.calibPointFor2Pt[calibPointIndex].internalValue);
                        bufferLength = sprintf(buffer, "ECCALSL%d=%d,%d,%d", calibPointIndex, g_MValue.CalibResultPointFor2Pt[calibPointIndex], targetValue, internalValue);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                    else
                    {
                        sdi12SendError();
                    }
                }

                //Set Reset System
                //Request: aXW_RESETSYSTEM!
                //Response:aRESETSYSTEM=xxxxxx
                else if(strstr(&cmd[0], "XW_RESETSYSTEM!"))
                {
                    char buffer[128];
                    uint8_t bufferLength = 0;

                    //Send data response
                    bufferLength = sprintf(buffer, "RESETSYSTEM=%d", 0);
                    sdi12Send(buffer, bufferLength, 0);

                    NVIC_SystemReset();
                }
                //Set Reset Factory
                //Request: aXW_RESETFACTORY!
                //Response:aRESETCALIB=xxxxxx
                else if(strstr(&cmd[0], "XW_RESETFACTORY!"))
                {
                    char buffer[128];
                    uint8_t bufferLength = 0;

                    eeprom_reset_factory();

                    //Send data response
                    bufferLength = sprintf(buffer, "RESETFACTORY=%d", 0);
                    sdi12Send(buffer, bufferLength, 0);
                }
                //Set Reset Calib
                //Request: aXW_RESETCALIB!
                //Response:aRESETCALIB=xxxxxx
                else if(strstr(&cmd[0], "XW_RESETCALIB!"))
                {
                    char buffer[128];
                    uint8_t bufferLength = 0;

                    eeprom_reset_calibration();

                    //Send data response
                    bufferLength = sprintf(buffer, "RESETCALIB=%d", 0);
                    sdi12Send(buffer, bufferLength, 0);
                }
                //Set Self Calib
                //Request: aXW_SELFCALIB!
                //Response:aSELFCALIB=xxxxxx
                else if(strstr(&cmd[0], "XW_SELFCALIB!"))
                {
                    char buffer[128];
                    uint8_t bufferLength = 0;

                    int16_t errorCode = SensorStartSelfCalibAndWaitDone();

                    //Send data response
                    bufferLength = sprintf(buffer, "SELFCALIB=%d", g_MValue.SelfCalibStatus);
                    sdi12Send(buffer, bufferLength, 0);
                }
                //Write SN
                //Request:  aXW_SN_12345678!
                //Response: aSN=12345678
                else if(strstr(&cmd[0], "XW_SN_") && cmd[14] == '!')
                {
                    uint8_t* pValueStartPos = (uint8_t*)&cmd[6];

                    for(uint8_t i = 0; i < 2; i++)
                    {
                        uint16_t hi16 = MAKE_16BIT(pValueStartPos[4 * i], pValueStartPos[4 * i + 1]);
                        uint16_t lo16 = MAKE_16BIT(pValueStartPos[4 * i + 2], pValueStartPos[4 * i + 3]);
                        uint32_t value = MAKE_32BIT(hi16, lo16);

                        eeprom_writeint32(&eepUserConfig.sn[i], value);
                    }
                    crc32EepromAppend((uint32_t*) &eepUserConfig, sizeof(STRUCT_USERCONFIG) / sizeof(char));

                    char buffer[128];
                    uint8_t bufferLength;
                    char bufValue[16];
                    memset(bufValue, 0x00, 16);
                    for(uint8_t idx = 0; idx < 2; idx++)
                    {
                        uint32_t value = eeprom_readint32(&eepUserConfig.sn[idx]);
                        bufValue[4 * idx + 0] = HI8_UINT(HI16_UINT32(value));
                        bufValue[4 * idx + 1] = LO8_UINT(HI16_UINT32(value));
                        bufValue[4 * idx + 2] = HI8_UINT(LO16_UINT32(value));
                        bufValue[4 * idx + 3] = LO8_UINT(LO16_UINT32(value));
                    }

                    bufferLength = sprintf(buffer, "SN=%s", bufValue);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Read SN
                //Request:  aXR_SN!
                //Response: aSN=12345678
                else if(strstr(&cmd[0], "XR_SN!"))
                {
                    char buffer[128];
                    uint8_t bufferLength;
                    char bufValue[16];
                    memset(bufValue, 0x00, 16);
                    for(uint8_t idx = 0; idx < 2; idx++)
                    {
                        uint32_t value = eeprom_readint32(&eepUserConfig.sn[idx]);
                        bufValue[4 * idx + 0] = HI8_UINT(HI16_UINT32(value));
                        bufValue[4 * idx + 1] = LO8_UINT(HI16_UINT32(value));
                        bufValue[4 * idx + 2] = HI8_UINT(LO16_UINT32(value));
                        bufValue[4 * idx + 3] = LO8_UINT(LO16_UINT32(value));
                    }

                    bufferLength = sprintf(buffer, "SN=%s", bufValue);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set Debug Enabled
                //Request:  aXSPwDdBg!
                //Response: aXSPwDdBg!
                else if(strstr(&cmd[0], "XSPwDdBg!"))
                {
                    setDebugEnabled();

                    char buffer[128];
                    uint8_t bufferLength;
                    bufferLength = sprintf(buffer, "XSPwDdBg!");
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Set Prod info
                //Request:  aXW_SENSORINFO_12345678123456781234567812345678!
                //Response: aSENSORINFO=12345678123456781234567812345678
                else if(strstr(&cmd[0], "XW_SENSORINFO_") && cmd[46] == '!')
                {
                    uint8_t* pValueStartPos = (uint8_t*)&cmd[14];

                    for(uint8_t i = 0; i < 8; i++)
                    {
                        uint16_t hi16 = MAKE_16BIT(pValueStartPos[4 * i], pValueStartPos[4 * i + 1]);
                        uint16_t lo16 = MAKE_16BIT(pValueStartPos[4 * i + 2], pValueStartPos[4 * i + 3]);
                        uint32_t value = MAKE_32BIT(hi16, lo16);

                        eeprom_writeint32(&eepFactoryConfig.productInfo[i], value);
                    }
                    crc32EepromAppend((uint32_t*) &eepFactoryConfig, sizeof(STRUCT_FACTORYCONFIG) / sizeof(char));

                    char buffer[128];
                    uint8_t bufferLength;
                    char bufValue[36];
                    memset(bufValue, 0x00, 36);
                    for(uint8_t idx = 0; idx < 8; idx++)
                    {
                        uint32_t value = eeprom_readint32(&eepFactoryConfig.productInfo[idx]);
                        bufValue[4 * idx + 0] = HI8_UINT(HI16_UINT32(value));
                        bufValue[4 * idx + 1] = LO8_UINT(HI16_UINT32(value));
                        bufValue[4 * idx + 2] = HI8_UINT(LO16_UINT32(value));
                        bufValue[4 * idx + 3] = LO8_UINT(LO16_UINT32(value));
                    }

                    bufferLength = sprintf(buffer, "SENSORINFO=%s", bufValue);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Sensor Type Set
                //Request:  aXW_SENSORTYPE_0!
                //Response: aSENSORTYPE=0
                else if(strstr(&cmd[0], "XW_SENSORTYPE_") && cmd[cmdCounter - 1] == '!')
                {
                    if(getDebugEnabled() == DEBUG_DISABLE)
                    {
                        sdi12SendError();
                    }
                    else
                    {
                        uint16_t sensorType;
                        sensorType = atoi(&cmd[14]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。
                        if(sensorType == CONST_SENSORTYPE_CubeIoRtd)
                        {
                            eeprom_writeint32((uint32_t*)&eepFactoryConfig.sensorType, sensorType);
                            crc32EepromAppend((uint32_t*) &eepFactoryConfig, sizeof(STRUCT_FACTORYCONFIG) / sizeof(uint8_t));

                            sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                            char buffer[64];
                            uint8_t bufferLength = 0;
                            bufferLength = sprintf(buffer, "SENSORTYPE=%d", sensorType);
                            sdi12Send(buffer, bufferLength, 0);
                        }
                        else
                        {
                            sdi12SendError();
                        }
                    }
                }

                //Sensor Type Get
                //Request:  aXR_SENSORTYPE!
                //Response: aSENSORTYPE=0
                else if(strstr(&cmd[0], "XR_SENSORTYPE!"))
                {
                    //if(getDebugEnabled() == DEBUG_DISABLE)
                    //{
                    //    sdi12SendError();
                    //}
                    //else
                    {
                        uint16_t sensorType = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.sensorType);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "SENSORTYPE=%d", sensorType);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                }

                //Temperature Sensor Set
                //Request:  aXW_TSENSOR_0/1/2!
                //Response: aTSENSOR=0/1/2
                else if(strstr(&cmd[0], "XW_TSENSOR_") && cmd[cmdCounter - 1] == '!')
                {
                    if(getDebugEnabled() == DEBUG_DISABLE)
                    {
                        sdi12SendError();
                    }
                    else
                    {
                        uint16_t tempSensor = 0;
                        tempSensor = atoi(&cmd[11]);//atoi()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。

                        if(tempSensor <= CONST_TEMPSENSOR_PT1000)
                        {
                            eeprom_writeint32((uint32_t*)&eepFactoryConfig.tempSensor, (int32_t)tempSensor);
                            crc32EepromAppend((uint32_t*) &eepFactoryConfig, sizeof(STRUCT_FACTORYCONFIG) / sizeof(uint8_t));

                            tempSensor = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.tempSensor);
                            char buffer[64];
                            uint8_t bufferLength = 0;
                            bufferLength = sprintf(buffer, "TSENSOR=%d", tempSensor);
                            sdi12Send(buffer, bufferLength, 0);
                        }
                        else
                        {
                            sdi12SendError();
                        }
                    }
                }

                //Temperature Sensor Get
                //Request:  aXR_TSENSOR!
                //Response: aTSENSOR=0/1/2
                else if(strstr(&cmd[0], "XR_TSENSOR!"))
                {
                    //if(getDebugEnabled() == DEBUG_DISABLE)
                    //{
                    //    sdi12SendError();
                    //}
                    //else
                    {
                        uint16_t tempSensor = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.tempSensor);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "TSENSOR=%d", tempSensor);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                }

                //Set Cell Constant
                //Request:  aXW_CC_0/1/2!
                //Response: aCC=0/1/2
                else if(strstr(&cmd[0], "XW_CC_") && cmd[cmdCounter - 1] == '!')
                {
                    if(getDebugEnabled() == DEBUG_DISABLE)
                    {
                        sdi12SendError();
                    }
                    else
                    {
                        uint16_t cellConstantReserved = 0;
                        cellConstantReserved = atoi(&cmd[6]);//atoi()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。

                        if(cellConstantReserved == CONST_CELLCONSTANT_1P0)
                        {
                            eeprom_writeint32((uint32_t*)&eepSensorConfig.cellConstantReserved, (int32_t)cellConstantReserved);
                            crc32EepromAppend((uint32_t*) &eepSensorConfig, sizeof(STRUCT_SENSORCONFIG) / sizeof(uint8_t));

                            cellConstantReserved = (uint16_t)eeprom_readint32((uint32_t*)&eepSensorConfig.cellConstantReserved);
                            char buffer[64];
                            uint8_t bufferLength = 0;
                            bufferLength = sprintf(buffer, "CC=%d", cellConstantReserved);
                            sdi12Send(buffer, bufferLength, 0);
                        }
                        else
                        {
                            sdi12SendError();
                        }
                    }
                }

                //Get Cell Constant
                //Request:  aXR_CC!
                //Response: aCC=0/1/2
                else if(strstr(&cmd[0], "XR_CC!"))
                {
                    //if(getDebugEnabled() == DEBUG_DISABLE)
                    //{
                    //    sdi12SendError();
                    //}
                    //else
                    {
                        uint16_t cellConstantReserved = (uint16_t)eeprom_readint32((uint32_t*)&eepSensorConfig.cellConstantReserved);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "CC=%d", cellConstantReserved);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                }

                //Set EC Range
                //Request:  aXW_ECR_0/1/2!
                //Response: aECR=0/1/2
                else if(strstr(&cmd[0], "XW_ECR_") && cmd[cmdCounter - 1] == '!')
                {
                    if(getDebugEnabled() == DEBUG_DISABLE)
                    {
                        sdi12SendError();
                    }
                    else
                    {
                        uint16_t ecRangeIdx = 0;
                        ecRangeIdx = atoi(&cmd[7]);//atoi()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。

                        if(ecRangeIdx <= CONST_ECRANGEIDX_END)
                        {
                            eeprom_writeint32((uint32_t*)&eepFactoryConfig.ecRangeIdx, ecRangeIdx);
                            crc32EepromAppend((uint32_t*) &eepFactoryConfig, sizeof(STRUCT_FACTORYCONFIG) / sizeof(uint8_t));

                            eeprom_reset_calibration();

                            ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                            char buffer[64];
                            uint8_t bufferLength = 0;
                            bufferLength = sprintf(buffer, "ECR=%d", ecRangeIdx);
                            sdi12Send(buffer, bufferLength, 0);
                        }
                        else
                        {
                            sdi12SendError();
                        }
                    }
                }

                //Get EC Range
                //Request:  aXR_ECR!
                //Response: aECR=0/1/2
                else if(strstr(&cmd[0], "XR_ECR!"))
                {
                    //if(getDebugEnabled() == DEBUG_DISABLE)
                    //{
                    //    sdi12SendError();
                    //}
                    //else
                    {
                        uint16_t ecRangeIdx = (uint16_t)eeprom_readint32((uint32_t*)&eepFactoryConfig.ecRangeIdx);
                        char buffer[64];
                        uint8_t bufferLength = 0;
                        bufferLength = sprintf(buffer, "ECR=%d", ecRangeIdx);
                        sdi12Send(buffer, bufferLength, 0);
                    }
                }

                //PT1000 Calibration
                //Request: aXW_CALPT1K[0/1]_xxxx!
                //Response:aCALPT1K[0/1]=xxxx
                //xxxxxx=(-16000~16000,32767)
                else if((strstr(&cmd[0], "XW_CALPT1K0_") || strstr(&cmd[0], "XW_CALPT1K1_")) && cmd[cmdCounter - 1] == '!')
                {
                    if(getDebugEnabled() == DEBUG_DISABLE)
                    {
                        sdi12SendError();
                    }
                    else
                    {
                        char calibChannel = cmd[10];
                        float calibValue = 0;
                        calibValue = atof(&cmd[12]);//atof()会扫描参数nptr 字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做 转换，而再遇到非数字或字符串 结束时('\0')才结束转换，并将结果返回。参数nptr字符串可包含正负号、 小数点或E(e)来表示指数部分，如123.456或123e-2。

                        if(calibValue >= -32768 && calibValue <= 32767)
                        {
                            //Measure
                            int16_t errorCode = SensorStartMeasureAndWaitDone();

                            if(calibValue == 32767)
                            {
                                calibValue = g_MValue.FinalTempPT1000RawAD;
                            }

                            if(calibChannel == '0')
                            {
                                eeprom_writeint32((uint32_t*)&eepCalibRaw.CalibRawPT1000AD[0], (int32_t)calibValue);
                                crc32EepromAppend((uint32_t*) &eepCalibRaw, sizeof(STRUCT_CALIBRAW) / sizeof(uint8_t));
                                calibValue = (int32_t)eeprom_readint32((uint32_t*)&eepCalibRaw.CalibRawPT1000AD[0]);
                            }
                            else
                            {
                                eeprom_writeint32((uint32_t*)&eepCalibRaw.CalibRawPT1000AD[1], (int32_t)calibValue);
                                crc32EepromAppend((uint32_t*) &eepCalibRaw, sizeof(STRUCT_CALIBRAW) / sizeof(uint8_t));
                                calibValue = (int32_t)eeprom_readint32((uint32_t*)&eepCalibRaw.CalibRawPT1000AD[1]);
                            }

                            char buffer[64];
                            uint8_t bufferLength = 0;
                            if(calibChannel == '0')
                            {
                                bufferLength = sprintf(buffer, "CALIBPT1K0=%.2f", calibValue);
                            }
                            else
                            {
                                bufferLength = sprintf(buffer, "CALIBPT1K1=%.2f", calibValue);
                            }
                            sdi12Send(buffer, bufferLength, 0);
                        }
                        else
                        {
                            sdi12SendError();
                        }
                    }
                }

                //PT1000 Calibration Get
                //Request: aXR_CALPT1K[0/1]!
                //Response:aCALPT1K[0/1]=xxxx
                else if(strstr(&cmd[0], "XR_CALPT1K0!") || strstr(&cmd[0], "XR_CALPT1K1!"))
                {
                    float calibValue;
                    char calibChannel = cmd[9];

                    if(calibChannel == '0')
                    {
                        calibValue = (int32_t)eeprom_readint32((uint32_t*)&eepCalibRaw.CalibRawPT1000AD[0]);
                    }
                    else
                    {
                        calibValue = (int32_t)eeprom_readint32((uint32_t*)&eepCalibRaw.CalibRawPT1000AD[1]);
                    }

                    char buffer[64];
                    uint8_t bufferLength = 0;
                    if(calibChannel == '0')
                    {
                        bufferLength = sprintf(buffer, "CALIBPT1K0=%.2f", calibValue);
                    }
                    else
                    {
                        bufferLength = sprintf(buffer, "CALIBPT1K1=%.2f", calibValue);
                    }
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Read run time in seconds
                //Request: aXR_RUNTIME!
                //Response:RUNTIME=0-*******
                else if(strstr(&cmd[0], "XR_RUNTIME!"))
                {
                    char buffer[64];
                    uint8_t bufferLength = 0;
                    uint32_t timenow = GetTimeStamp();
                    bufferLength = sprintf(buffer, "RUNTIME=%d", timenow);
                    sdi12Send(buffer, bufferLength, 0);
                }

                //Invalid aXxxxxx! Command
                else
                {
                    sdi12SendError();
                }
                g_sdiStateMachine = SDI_STANDBY;
                break;
            }
            default:
            {
                //send error
                sdi12SendError();
                //SDI12协议要求(Invalid Command)
                g_sdiStateMachine = SDI_FIRST_MARKING_DETECTING;
                break;
            }
            }
        }
        default:
        {
            break;
        }
        }
    }
}
