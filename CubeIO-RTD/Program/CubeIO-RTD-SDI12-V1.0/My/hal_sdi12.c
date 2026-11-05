#include "main.h"
#include "hal_sdi12.h"
#include "utils.h"
#include "hal_gpio.h"
#include "sensor.h"
#include "delay_utils.h"
#include "globals.h"

STRUCT_SDI12_INFO g_sdiCommInfo;
volatile ENUM_SDISTATEMACHINE g_sdiStateMachine;

//串口在不使能的情况下也可以初始化，但是必须有时钟
void Sdi12Init(void)//USART 初始化
{
    LL_USART_Disable(USART_SDI12);
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    //TXDEN引脚初始化
    LL_GPIO_ResetOutputPin(SDI12_TXDEN_PORT, SDI12_TXDEN_PIN);//低电平时为接收状态
    GPIO_InitStruct.Pin = SDI12_TXDEN_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SDI12_TXDEN_PORT, &GPIO_InitStruct);

    //RX引脚不使用，设置为开漏输出
    LL_GPIO_SetOutputPin(SDI12_RX_PORT, SDI12_RX_PIN);//输出高组态，以免影响其他引脚
    GPIO_InitStruct.Pin = SDI12_RX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SDI12_RX_PORT, &GPIO_InitStruct);

    //SDI12 Tx PIN
    LL_USART_InitTypeDef USART_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    //只使用TX引脚
    GPIO_InitStruct.Pin = SDI12_TX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = USART_SDI12_GPIO_ALTERNATIVE_FUNCTION;
    LL_GPIO_Init(SDI12_TX_PORT, &GPIO_InitStruct);

    //初始化单线串口, 8个数据（包含1个偶校验），LSB先发送，高低电平切换
    LL_USART_EnableHalfDuplex(USART_SDI12);
    LL_USART_SetTXPinLevel(USART_SDI12, LL_USART_TXPIN_LEVEL_INVERTED); //LL_USART_TXPIN_LEVEL_STANDARD,LL_USART_TXPIN_LEVEL_INVERTED
    LL_USART_SetRXPinLevel(USART_SDI12, LL_USART_RXPIN_LEVEL_INVERTED);
    LL_USART_SetTransferBitOrder(USART_SDI12, LL_USART_BITORDER_LSBFIRST); //LL_USART_BITORDER_LSBFIRST，LL_USART_BITORDER_MSBFIRST

    USART_InitStruct.BaudRate = 1200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;//7个数据位加一个校验位，所以这里应该是8Bit
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_EVEN;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_NONE;//暂时不开启发送和接收
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    LL_USART_Init(USART_SDI12, &USART_InitStruct);
    //LL_USART_ConfigAsyncMode(USART_SDI12);//设置这个以后会清除单线串口使能，必须注释

    //设置接收指针
    g_sdiCommInfo.vm_UART_RX_P_HEAD = 0;
    g_sdiCommInfo.vm_UART_RX_P_TAIL = 0;
}

void Sdi12DeInit(void)//USART 反向初始化
{
    //关闭UART
    LL_USART_Disable(USART_SDI12);
    //关闭UART时钟
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2);

    //设置所有SDI12的IO为开漏输出
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    //SDI-12 interrupt pin, Set to output open drain, The status of this pin depends on RX/TX pin
    //SDI12中断引脚设置为输出模式仍然可以中断

    //RX引脚不使用，设置为开漏输出
    LL_GPIO_SetOutputPin(SDI12_RX_PORT, SDI12_RX_PIN);//输出高组态，以免影响其他引脚
    GPIO_InitStruct.Pin = SDI12_RX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SDI12_RX_PORT, &GPIO_InitStruct);

    //	//以下代码不要重新初始化，否则会引起唤醒后的BREAK检测不到
    //	//TX引脚，设置为开漏输出
    //	LL_GPIO_SetOutputPin(SDI12_TX_PORT, SDI12_TX_PIN);//输出高组态
    //	GPIO_InitStruct.Pin = SDI12_TX_PIN;
    //	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    //	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    //	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    //	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    //	LL_GPIO_Init(SDI12_TX_PORT, &GPIO_InitStruct);
}

void Sdi12SetTXPinOutputPP(void)
{
    //关闭UART
    LL_USART_Disable(USART_SDI12);

    //设置引脚为输出，并输出低电平
    LL_GPIO_ResetOutputPin(SDI12_TX_PORT, SDI12_TX_PIN);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SDI12_TX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SDI12_TX_PORT, &GPIO_InitStruct);
}

void Sdi12SetTXPinSingleWireTX(void)
{
//  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
//	//设置引脚为输出，并输出高电平
//	LL_GPIO_SetOutputPin(SDI12_INT_PORT, SDI12_INT_PIN);
//	GPIO_InitStruct.Pin = SDI12_TX_PIN;
//	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
//	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
//	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
//	LL_GPIO_Init(SDI12_TX_PORT, &GPIO_InitStruct);

    //74LVC1G45切换为发送状态
    SDI12_RXDEN_OFF();

    uint32_t temp;
    LL_USART_ClearFlag_IDLE(USART_SDI12);
    LL_USART_ClearFlag_TC(USART_SDI12);

    LL_USART_DisableIT_IDLE(USART_SDI12);
    LL_USART_DisableIT_TC(USART_SDI12);
    LL_USART_DisableIT_TXE(USART_SDI12);
    LL_USART_DisableIT_RXNE(USART_SDI12);

    temp = USART_SDI12->ISR; //清USART_IT_IDLE标志
    temp = USART_SDI12->TDR;
    temp = USART_SDI12->RDR;
    temp = temp;

    //打开UART
    //LL_USART_Enable(USART_SDI12);
    //使能发送器(STM32L0x1 reference manual, Set the TE bit in USART_CR1 to send an idle frame as first transmission.)
    //TEACK: Transmit enable acknowledge flag,
    //This bit is set/reset by hardware, when the Transmit Enable value is taken into account by the USART.
    //It can be used when an idle frame request is generated by writing TE=0, followed by TE=1
    //in the USART_CR1 register, in order to respect the TE=0 minimum period.
    //USART.TE从0到1的变化会触发一个IDLE FRAME，为了保证这个时间，需要使用TEACK标志位判断触发结束
    LL_USART_SetTransferDirection(USART_SDI12, LL_USART_DIRECTION_NONE);
    LL_USART_SetTransferDirection(USART_SDI12, LL_USART_DIRECTION_TX);
    while(LL_USART_IsActiveFlag_TEACK(USART_SDI12) == 0) {};
}


void Sdi12SetTXPinSingleWireRX(void)
{
//  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
//	//设置引脚为输出，并输出高电平
//	LL_GPIO_SetOutputPin(SDI12_INT_PORT, SDI12_INT_PIN);
//	GPIO_InitStruct.Pin = SDI12_TX_PIN;
//	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
//	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
//	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
//	LL_GPIO_Init(SDI12_TX_PORT, &GPIO_InitStruct);

    //74LVC1G45切换为接收状态
    SDI12_RXDEN_ON();

    uint32_t temp;
    LL_USART_ClearFlag_IDLE(USART_SDI12);
    LL_USART_ClearFlag_TC(USART_SDI12);

    LL_USART_EnableIT_IDLE(USART_SDI12);
    LL_USART_DisableIT_TC(USART_SDI12);
    LL_USART_DisableIT_TXE(USART_SDI12);
    LL_USART_EnableIT_RXNE(USART_SDI12);

    temp = USART_SDI12->ISR; //清USART_IT_IDLE标志
    temp = USART_SDI12->TDR;
    temp = USART_SDI12->RDR;
    temp = temp;

    //打开UART
    LL_USART_Enable(USART_SDI12);

    //使能接收器,并等待接收器使能标志位
    LL_USART_SetTransferDirection(USART_SDI12, LL_USART_DIRECTION_RX);
    while(LL_USART_IsActiveFlag_REACK(USART_SDI12) == 0)
    {
    }
}

// \r\n is not included in the buf and count
void sdi12Send(char* buf, uint8_t count, uint8_t crcEnabled)
{
    uint8_t i = 0;
    uint8_t countToSend = count + 1; //加上一个SDI12地址
    PIN_LEDPWR_ON();

    // 0. Preapre data buffer (sdi12 address and data and crc and cr lf)
    g_sdiCommInfo.vm_UART_TX_BUF[0] = g_sdiCommInfo.addr;
    for(i = 0; i < count; i++)
    {
        g_sdiCommInfo.vm_UART_TX_BUF[1 + i] = buf[i];
    }

    // 2. Prepare CRC if needed
    if(crcEnabled != 0)
    {
        //send crc
        sdi12Crc16Caculate((uint8_t*)&g_sdiCommInfo.vm_UART_TX_BUF[0],
                           countToSend,
                           (uint8_t*)&g_sdiCommInfo.vm_UART_TX_BUF[countToSend]);
        countToSend = countToSend + 3; //3 byte CRC checksum
    }

    // 3. Prepare \r\n
    g_sdiCommInfo.vm_UART_TX_BUF[countToSend++] = '\r';
    g_sdiCommInfo.vm_UART_TX_BUF[countToSend++] = '\n';

    // 4. Prepare Counter
    g_sdiCommInfo.vm_UART_TX_CNT = countToSend;

    // 5.Sending marking state for 8.33ms (SDI12 V1.3 4.4.13.1 P.24)
    //Sdi12SetTXPinOutputPP();
    //SDI12_TX_LOW();

    //PIN_LEDPWR_OFF();
    //_delay_ms(10); //SDI12 Specification, V1.3, Page 5 "Step3"
    //PIN_LEDPWR_TOGGLE();
    Sdi12SetTXPinSingleWireTX();//设置为串口输出(使能串口发送时会自动发送一个IDLE FRAME，相当于8.33ms的MARKING)
    //PIN_LEDPWR_TOGGLE();
    // 6. sending, and wait for sending done
    LL_USART_ClearFlag_TC(USART_SDI12);
    for(i = 0; i < g_sdiCommInfo.vm_UART_TX_CNT; i++)
    {
        while(LL_USART_IsActiveFlag_TXE(USART_SDI12) != 1) {};
        LL_USART_TransmitData8(USART_SDI12, g_sdiCommInfo.vm_UART_TX_BUF[i]);
        //PIN_LEDPWR_TOGGLE();
    }
    while(LL_USART_IsActiveFlag_TC(USART_SDI12) != 1)
    {
    }
    //PIN_LEDPWR_TOGGLE();

    //wait line stable
    _delay_ms(1);
    PIN_LEDPWR_OFF();

    //发送完成后切换为接收状态
    Sdi12SetTXPinSingleWireRX();
}

/*
采用当队列只剩下一个空闲结点的空间时,认为队列已满的简单办法,以区分队空和队满.
所以队列中始终一个空元素不用,
tail始终指向最后一个元素后面的那个空元素,那个空元素作为队尾的标志.

因此只有一个元素时,tail = head + 1,没有元素的时候（队空）,正好是tail = head；
队满的时候,（tail+1）mod n = head,

这里书上可能写错了.初始的时候队空,所以head = tail = 0, 加入一个元素以后,应该是 head =0 , tail = 1,tail始终指向最后一个元素的后面一个位置（当然是将队列看作环状的）
*/

uint8_t sdi12PushChar(char value)
{
    ENTER_CRITICAL_SECTION();

    uint8_t head = g_sdiCommInfo.vm_UART_RX_P_HEAD;
    uint8_t tail = g_sdiCommInfo.vm_UART_RX_P_TAIL;

    if(head == ((tail + 1) % vm_UART_RX_BUF_L))
    {
        //Full
        EXIT_CRITICAL_SECTION();
        return 1;
    }

    g_sdiCommInfo.vm_UART_RX_BUF[tail] = value; //一个Break接收完毕
    g_sdiCommInfo.vm_UART_RX_P_TAIL = (tail + 1) % vm_UART_RX_BUF_L;

    EXIT_CRITICAL_SECTION();
    return 0;
}

uint8_t sdi12GetChar(char* pChar)
{
    ENTER_CRITICAL_SECTION();

    uint8_t head = g_sdiCommInfo.vm_UART_RX_P_HEAD;
    uint8_t tail = g_sdiCommInfo.vm_UART_RX_P_TAIL;

    if(head == tail)
    {
        //Empty
        EXIT_CRITICAL_SECTION();
        return 0;
    }

    pChar[0] = g_sdiCommInfo.vm_UART_RX_BUF[head];
    head = (head + 1) % vm_UART_RX_BUF_L;
    g_sdiCommInfo.vm_UART_RX_P_HEAD = head;

    EXIT_CRITICAL_SECTION();
    return 1;
}

//仅取出下一个字节，但并不移动指针
uint8_t sdi12FetchChar(char* pChar)
{
    ENTER_CRITICAL_SECTION();

    uint8_t head = g_sdiCommInfo.vm_UART_RX_P_HEAD;
    uint8_t tail = g_sdiCommInfo.vm_UART_RX_P_TAIL;

    if(head == tail)
    {
        //Empty
        EXIT_CRITICAL_SECTION();
        return 0;
    }

    pChar[0] = g_sdiCommInfo.vm_UART_RX_BUF[head];

    EXIT_CRITICAL_SECTION();
    return 1;
}

void sdi12Crc16Caculate(uint8_t* buf, uint8_t count, uint8_t* bufCrc16)
{
    uint8_t i;
    uint16_t sum = 0;
    for(i = 0; i < count; i++)
        sum = _crc16_update(sum, buf[i]);

    bufCrc16[0] = 0x0040 | (sum >> 12);
    bufCrc16[1] = 0x0040 | ((sum >> 6) & 0x003f);
    bufCrc16[2] = 0x0040 | (sum & 0x003f);
}

void sdi12SendError(void)
{
    char buf[5] = {'E', 'R', 'R', 'O', 'R'};
    sdi12Send(buf, 5, 0);
}

#if 0  //DigiTEMP暂不支持DDI协议输出

//AlexReviewedAndUpdated - This is updated according to METER TEROS11/12 integrator guide
static uint8_t ddi_CRC6_Offset(const char* buffer, uint8_t count)
{
    uint8_t i;
    uint8_t bit;
    uint8_t crc = 0xfc; // Set upper 6 bits to 1’s

    // Loop through all the bytes in the buffer
    for(i = 0; i < count; i++)
    {
        // Get the next byte in the buffer and XOR it with the crc
        crc ^= buffer[i];

        // Loop through all the bits in the current byte
        for(bit = 8; bit > 0; bit--)
        {
            // If the uppermost bit is a 1...
            if(crc & 0x80)
            {
                // Shift to the next bit and XOR it with a polynomial
                crc = (crc << 1) ^ 0x9c;
            }
            else
            {
                // Shift to the next bit
                crc = crc << 1;
            }
        }
    }
    // Shift upper 6 bits down for crc
    crc = (crc >> 2);
    // Add 48 to shift crc to printable character avoiding \r \n and !
    return (crc + 48);
}

//DDI串口初始化。对于DDI接口，不能设置串口为串口单线模式，单线模式时TX引脚在不发送时会处于接收状态，不是高电平，不符合DDI的要求。
void ddiInit(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    //打开UART时钟，打开时钟后才能进行寄存器的读写
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    //RX引脚再设置为串口输入
    GPIO_InitStruct.Pin = SDI12_RX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(SDI12_RX_PORT, &GPIO_InitStruct);

    //TX引脚再设置为串口输出
    GPIO_InitStruct.Pin = SDI12_TX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(SDI12_TX_PORT, &GPIO_InitStruct);

    //初始化DDI单线串口, 1个起始位，1个停止位，8个数据（无校验），LSB先发送，高低电平不切换（ActiveHigh）
    //LL_USART_EnableHalfDuplex(USART_SDI12);//对于DDI接口，不能设置串口为串口单线模式，单线模式时TX引脚在不发送时会处于接收状态，不是高电平，不符合DDI的要求。
    LL_USART_SetTXPinLevel(USART_SDI12, LL_USART_TXPIN_LEVEL_STANDARD); //LL_USART_TXPIN_LEVEL_STANDARD,LL_USART_RXPIN_LEVEL_INVERTED
    LL_USART_SetRXPinLevel(USART_SDI12, LL_USART_RXPIN_LEVEL_STANDARD);
    LL_USART_SetTransferBitOrder(USART_SDI12, LL_USART_BITORDER_LSBFIRST); //LL_USART_BITORDER_LSBFIRST，LL_USART_BITORDER_MSBFIRST

    USART_InitStruct.BaudRate = 1200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;//8个数据位无校验位，所以这里应该是8Bit
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX;//暂时不开启发送和接收
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    LL_USART_Init(USART_SDI12, &USART_InitStruct);
    //LL_USART_ConfigAsyncMode(USART_SDI12);//设置这个以后会清除单线串口使能，必须注释
    LL_USART_Enable(USART_SDI12);//串口操作的原则，时钟要打开，打开时钟后可以设置寄存器，设置完寄存器才能使能串口，如果先使能串口之后，不能设置相关寄存器比如波特率

    //使能发送器(STM32L0x1 reference manual, Set the TE bit in USART_CR1 to send an idle frame as first transmission.)
    //TEACK: Transmit enable acknowledge flag,
    //This bit is set/reset by hardware, when the Transmit Enable value is taken into account by the USART.
    //It can be used when an idle frame request is generated by writing TE=0, followed by TE=1
    //in the USART_CR1 register, in order to respect the TE=0 minimum period.
    //USART.TE从0到1的变化会触发一个IDLE FRAME，为了保证这个时间，需要使用TEACK标志位判断触发结束
    LL_USART_SetTransferDirection(USART_SDI12, LL_USART_DIRECTION_NONE);
    LL_USART_SetTransferDirection(USART_SDI12, LL_USART_DIRECTION_TX);
    while(LL_USART_IsActiveFlag_TEACK(USART_SDI12) == 0) {};

    //设置接收指针
    g_sdiCommInfo.vm_UART_RX_P_HEAD = 0;
    g_sdiCommInfo.vm_UART_RX_P_TAIL = 0;
}

//AlexReviewedAndUpdated- According to TEROS11/12, Format is :
//For aR3 & aR4 sdi12 command   : a<TAB><calibratedCountsVWC> <temperature> <electricalConductivity><CR><sensorType><Checksum><CRC>
//For ADI output, no address "a":  <TAB><calibratedCountsVWC> <temperature> <electricalConductivity><CR><sensorType><Checksum><CRC>
//我们在这里暂时不加入DDI协议结尾的\r\n，因为SDI12协议中的aR3 & aR4也要用到这个函数，对于SDI12协议中的aR3 & aR4是不需要结尾的\r\n的
uint8_t ddiPrepare(char* buf)
{
    uint8_t byteCount = 0;
    uint16_t checksum;
    uint8_t i;
    uint8_t crc6;

    //AlexReviewedAndUpdated Below-According to METER TEROS11/12
    byteCount = sprintf(buf, "\t%.1f %.1f %d", g_MValue.SubstrateCalibratedRawCounts, g_MValue.NtcTempC, g_MValue.SubstrateECBulk);

    // 0. Append \r <CR>
    buf[byteCount++] = '\r';

    // 1. Append  sensorType
    char sensorType;
    sensorType = 'g'; //AlexReviewedAndUpdated-According to METER TEROS12
    //sensorType='h';//AlexReviewedAndUpdated-According to METER TEROS11
    buf[byteCount++] = sensorType;

    // 2. Append Checksum
    checksum = 0;
    for(i = 0; i < byteCount; i++)
    {
        checksum += buf[i];
    }
    buf[byteCount++] = checksum % 64 + 32;

    // 3. Append CRC6 checksum
    crc6 = ddi_CRC6_Offset(buf, byteCount);
    buf[byteCount++] = crc6;

    return byteCount;
}

//AlexReviewedAndUpdated- According to TEROS11/12, Format is :
//For ADI output, no address "a":  <TAB><calibratedCountsVWC> <temperature> <electricalConductivity><CR><sensorType><Checksum><CRC>
void ddiSend()
{
    char tempBuff[128];
    uint8_t countToSend;
    countToSend = ddiPrepare(tempBuff);

    // 0. Preapre data buffer
    for(uint8_t i = 0; i < countToSend; i++)
    {
        g_sdiCommInfo.vm_UART_TX_BUF[i] = tempBuff[i];
    }

    // 1. Prepare \r\n
    g_sdiCommInfo.vm_UART_TX_BUF[countToSend++] = '\r';
    g_sdiCommInfo.vm_UART_TX_BUF[countToSend++] = '\n';

    // 2. Prepare Counter
    g_sdiCommInfo.vm_UART_TX_CNT = countToSend;

    // 3. sending, and wait for sending done
    Sdi12SetTXPinSingleWireTX();//设置为串口输出(使能串口发送时会自动发送一个IDLE FRAME，相当于8.33ms的MARKING)
    LL_USART_ClearFlag_TC(USART_SDI12);
    for(uint8_t i = 0; i < g_sdiCommInfo.vm_UART_TX_CNT; i++)
    {
        while(LL_USART_IsActiveFlag_TXE(USART_SDI12) != 1) {};
        LL_USART_TransmitData8(USART_SDI12, g_sdiCommInfo.vm_UART_TX_BUF[i]);
        //PIN_LEDPWR_TOGGLE();
    }
    while(LL_USART_IsActiveFlag_TC(USART_SDI12) != 1) //保证发送完成
    {
    }
    //PIN_LEDPWR_TOGGLE();

    //wait line stable
    _delay_ms(1);
    PIN_LEDPWR_OFF();
}
#endif

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART2_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_ORE(USART_SDI12)
            || LL_USART_IsActiveFlag_NE(USART_SDI12)
            || LL_USART_IsActiveFlag_PE(USART_SDI12))
    {
        //Clear Sequence
        LL_USART_ClearFlag_ORE(USART_SDI12);
        LL_USART_ClearFlag_NE(USART_SDI12);
        LL_USART_ClearFlag_PE(USART_SDI12);
    }

    if(LL_USART_IsActiveFlag_FE(USART_SDI12))
    {
        //Clear Sequence
        LL_USART_ClearFlag_FE(USART_SDI12);
        sdi12PushChar(SDI12_CAHR_BREAK);
    }

    if(LL_USART_IsActiveFlag_IDLE(USART_SDI12) && LL_USART_IsEnabledIT_IDLE(USART_SDI12))
    {
        //Clear Sequence
        LL_USART_ClearFlag_IDLE(USART_SDI12);
        sdi12PushChar(SDI12_CAHR_MARKING);
    }

    if(LL_USART_IsActiveFlag_RXNE(USART_SDI12) && LL_USART_IsEnabledIT_RXNE(USART_SDI12))
    {
        sdi12PushChar(LL_USART_ReceiveData8(USART_SDI12) & 0x7F); //最高位为奇偶校验
    }
}

void EXTI_IRQHandler_SDI12(void)
{
    if(LL_EXTI_IsActiveFlag_0_31(SDI12_TXPIN_AS_EXTI_LINE) != RESET)
    {
        LL_EXTI_ClearFlag_0_31(SDI12_TXPIN_AS_EXTI_LINE);//清除所有挂起的中断
        if(g_sdiStateMachine == SDI_STANDBY)
        {
            //disable interrupt
            SDI12_TXPIN_AS_EXTI_DISABLE();
            //status change
            g_sdiStateMachine = SDI_FIRST_BREAK_DETECTING;
            //SDI12 line init
            Sdi12Init();
            Sdi12SetTXPinSingleWireRX();
            UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SDI12), 0);
        }
    }
    return;
}

