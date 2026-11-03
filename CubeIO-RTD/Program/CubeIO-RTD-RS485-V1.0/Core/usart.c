#include "main.h"
#include "usart.h"
#include "modbus_common.h"

STRUCT_USART_INFO g_usartInfo;

uint8_t MX_RS485_IsEnabled()
{
    return TRUE;
}

void MX_RS485_DMA_Init(void)
{
    LL_DMA_InitTypeDef DMA_InitStructure;
    LL_DMA_StructInit(&DMA_InitStructure);

    //时钟使能
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    /* Enable the USART Rx DMA channel */
    LL_DMA_DisableChannel(DMA1, DMA_RS485_RX_CHNL);
    LL_DMA_DeInit(DMA1, DMA_RS485_RX_CHNL);

    /* DMA channel Rx of USART Configuration */
    DMA_InitStructure.PeriphOrM2MSrcAddress = LL_USART_DMA_GetRegAddr(USART_RS485, LL_USART_DMA_REG_DATA_RECEIVE);           //外设地址
    DMA_InitStructure.MemoryOrM2MDstAddress = (uint32_t)g_usartInfo.questionBuf;     //内存地址
    DMA_InitStructure.Direction             = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;        //外设为源
    DMA_InitStructure.Mode                  = LL_DMA_MODE_NORMAL;                       //非循环传输
    DMA_InitStructure.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;                //外设地址固定
    DMA_InitStructure.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;                  //内存地址增加
    DMA_InitStructure.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;                  //字节
    DMA_InitStructure.MemoryOrM2MDstDataSize = LL_DMA_PDATAALIGN_BYTE;                  //字节
    DMA_InitStructure.NbData                = SERCOMM_QUESTIONBUF_LENGTH;               //缓存
    DMA_InitStructure.Priority              = LL_DMA_PRIORITY_LOW;                      //优先级

    LL_DMA_Init(DMA1, DMA_RS485_RX_CHNL, &DMA_InitStructure);
    LL_DMA_SetPeriphRequest(DMA1, DMA_RS485_RX_CHNL, DMA_RS485_RX_REQUESTNUMBER); //STM32F1的没有这个DMA请求号，不需要设置，但是STM32L0系列的有，需要加上
    LL_DMA_EnableChannel(DMA1, DMA_RS485_RX_CHNL);

    /* Enable the USART Tx DMA channel */
    LL_DMA_DisableChannel(DMA1, DMA_RS485_TX_CHNL);
    LL_DMA_DeInit(DMA1, DMA_RS485_TX_CHNL);
    /* DMA channel Tx of USART Configuration */
    DMA_InitStructure.PeriphOrM2MSrcAddress = LL_USART_DMA_GetRegAddr(USART_RS485, LL_USART_DMA_REG_DATA_TRANSMIT);           //外设地址
    DMA_InitStructure.MemoryOrM2MDstAddress = (uint32_t)g_usartInfo.responseBuf;     //内存地址
    DMA_InitStructure.Direction             = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;        //外设为源
    DMA_InitStructure.Mode                  = LL_DMA_MODE_NORMAL;                       //非循环传输
    DMA_InitStructure.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;                //外设地址固定
    DMA_InitStructure.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;                  //内存地址增加
    DMA_InitStructure.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;                  //字节
    DMA_InitStructure.MemoryOrM2MDstDataSize = LL_DMA_PDATAALIGN_BYTE;                  //字节
    DMA_InitStructure.NbData                = SERCOMM_QUESTIONBUF_LENGTH;               //缓存
    DMA_InitStructure.Priority              = LL_DMA_PRIORITY_LOW;                      //优先级

    LL_DMA_Init(DMA1, DMA_RS485_TX_CHNL, &DMA_InitStructure);
    LL_DMA_SetPeriphRequest(DMA1, DMA_RS485_TX_CHNL, DMA_RS485_TX_REQUESTNUMBER); //STM32F1的没有这个DMA请求号，不需要设置，但是STM32L0系列的有，需要加上
    LL_DMA_EnableChannel(DMA1, DMA_RS485_TX_CHNL);
}


void MX_RS485_Init(void)
{
    uint32_t baudrate;
    uint16_t stopBits;
    uint16_t parity;
    uint16_t wordlen;
    uint32_t temp;

    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    //1. 设置UART时钟选择

    LL_RCC_SetUSARTClockSource(LL_RCC_USART_RS485_CLKSOURCE_HSI);

    //2. 开启UART时钟
    //USART able to wake up the MCU from Stop mode. When this function is active, the clock
    //source for the USART must be HSI16 or LSE
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART_RS485);

    //3. 设置DMA
    MX_RS485_DMA_Init();

    //4. IO初始化
    LL_IOP_GRP1_EnableClock(USART_RS485_PORT);

    GPIO_InitStruct.Pin = RS485_TXD_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = USART_RS485_GPIO_ALTERNATIVE_FUNCTION;
    LL_GPIO_Init(RS485_TXD_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RS485_RXD_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = USART_RS485_GPIO_ALTERNATIVE_FUNCTION;
    LL_GPIO_Init(RS485_RXD_PORT, &GPIO_InitStruct);

    //RS485通信收发控制
    LL_GPIO_ResetOutputPin(RS485_TXDEN_PORT, RS485_TXDEN_PIN);
    GPIO_InitStruct.Pin = RS485_TXDEN_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(RS485_TXDEN_PORT, &GPIO_InitStruct);
    RS485_RXDEN_ON();

    g_usartInfo.usartStatus = USART_IDLE;
    g_usartInfo.timeoutCounter = 0;//timeout counter
    g_usartInfo.timeoutConstant = 10;
    g_usartInfo.questionBufCount = 0;

    if(0 == crc32EepromCheck((uint32_t*) &eepUsart, sizeof(STRUCT_USARTCOMM) / sizeof(char)))
    {
        g_usartInfo.address = eeprom_readint32(&eepUsart.address);//模块地质
        g_usartInfo.baudrate = eeprom_readint32(&eepUsart.baudrate); //波特率
        g_usartInfo.protocol = eeprom_readint32(&eepUsart.protocol); //协议
        g_usartInfo.parity = eeprom_readint32(&eepUsart.parity); //协议
        g_usartInfo.dataBits = eeprom_readint32(&eepUsart.dataBits); //协议
        g_usartInfo.stopBits = eeprom_readint32(&eepUsart.stopBits); //协议
        g_usartInfo.responseDelayConstant = eeprom_readint32(&eepUsart.responseDealy); //response delay time
        g_usartInfo.actvOutputIntervalConstant = eeprom_readint32(&eepUsart.activeOutputInterval);
    }
    else
    {
        g_usartInfo.address = MB_DEFAULT_ADDRESS;
        g_usartInfo.baudrate = SERCOMM_BAUDRATE_9600;
        g_usartInfo.protocol = SERCOMM_PROTOCOL_MODRTU;
        g_usartInfo.parity = SERCOMM_PARITY_NONE;
        g_usartInfo.dataBits = SERCOMM_DATABITS_8;
        g_usartInfo.stopBits = SERCOMM_STOPBITS_1;
        g_usartInfo.responseDelayConstant = SERCOMM_RESPONSEDELAY_0MS; //response delay time
        g_usartInfo.actvOutputIntervalConstant = SERCOMM_ACTVOUTPUTINTERVAL_0S;

        //不成功那么初始化到默认
        eeprom_writeint32(&eepUsart.address, g_usartInfo.address); //默认为0
        eeprom_writeint32(&eepUsart.baudrate, g_usartInfo.baudrate); //9600
        eeprom_writeint32(&eepUsart.protocol, g_usartInfo.protocol); //modbus rtu
        eeprom_writeint32(&eepUsart.parity, g_usartInfo.parity); //modbus rtu
        eeprom_writeint32(&eepUsart.dataBits, g_usartInfo.dataBits); //modbus rtu
        eeprom_writeint32(&eepUsart.stopBits, g_usartInfo.stopBits); //modbus rtu
        eeprom_writeint32(&eepUsart.responseDealy, g_usartInfo.responseDelayConstant); //response delay time
        eeprom_writeint32(&eepUsart.activeOutputInterval, g_usartInfo.actvOutputIntervalConstant);
        crc32EepromAppend((uint32_t*) &eepUsart, sizeof(STRUCT_USARTCOMM) / sizeof(char)); //重新计算crc}
    }

    switch(g_usartInfo.baudrate)
    {
    case SERCOMM_BAUDRATE_1200:
        baudrate = 1200;
        break;
    case SERCOMM_BAUDRATE_2400:
        baudrate = 2400;
        break;
    case SERCOMM_BAUDRATE_4800:
        baudrate = 4800;
        break;
    case SERCOMM_BAUDRATE_9600:
        baudrate = 9600;
        break;
    case SERCOMM_BAUDRATE_19200:
        baudrate = 19200;
        break;
    case SERCOMM_BAUDRATE_38400:
        baudrate = 38400;
        break;
    case SERCOMM_BAUDRATE_57600:
        baudrate = 57600;
        break;
    case SERCOMM_BAUDRATE_115200:
        baudrate = 115200;
        break;
    default:
        baudrate = 9600;
        break;
    }

    switch(g_usartInfo.stopBits)
    {
    case SERCOMM_STOPBITS_1:
        stopBits = LL_USART_STOPBITS_1;
        break;
    case SERCOMM_STOPBITS_2:
        stopBits = LL_USART_STOPBITS_2;
        break;
    default:
        stopBits = LL_USART_STOPBITS_1;
        break;
    }

    switch(g_usartInfo.parity)
    {
    case SERCOMM_PARITY_NONE:
        parity = LL_USART_PARITY_NONE;
        wordlen = LL_USART_DATAWIDTH_8B;
        break;
    case SERCOMM_PARITY_EVEN:
        parity = LL_USART_PARITY_EVEN;
        wordlen = LL_USART_DATAWIDTH_9B;
        break;
    case SERCOMM_PARITY_ODD:
        parity = LL_USART_PARITY_ODD;
        wordlen = LL_USART_DATAWIDTH_9B;
        break;
    default:
        parity = LL_USART_PARITY_NONE;
        wordlen = LL_USART_DATAWIDTH_8B;
        break;
    }

    USART_InitStruct.BaudRate = baudrate;
    USART_InitStruct.DataWidth = wordlen;
    USART_InitStruct.StopBits = stopBits;
    USART_InitStruct.Parity = parity;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    LL_USART_Init(USART_RS485, &USART_InitStruct);

    LL_USART_ClearFlag_RTO(USART_RS485);
    LL_USART_ClearFlag_TC(USART_RS485);

    LL_USART_EnableRxTimeout(USART_RS485);
    LL_USART_SetRxTimeout(USART_RS485, 35);//35 bits, 3.5 bytes timeout
    LL_USART_EnableIT_RTO(USART_RS485);
    LL_USART_DisableIT_TC(USART_RS485);
    LL_USART_DisableIT_TXE(USART_RS485);
    LL_USART_DisableIT_RXNE(USART_RS485);
    temp = USART_RS485->ISR; //清USART_IT_IDLE标志
    temp = USART_RS485->TDR;
    temp = USART_RS485->RDR;
    temp = temp;
    LL_USART_EnableDMAReq_RX(USART_RS485); //采用DMA方式接收
    LL_USART_EnableDMAReq_TX(USART_RS485); //采用DMA方式发送

    /* Set the wake-up event type : specify wake-up on RXNE flag */
    RS485_RXDEN_ON();
    LL_USART_Enable(USART_RS485);

    /* Polling USART initialisation */
    //while((!(LL_USART_IsActiveFlag_TEACK(USART1))) || (!(LL_USART_IsActiveFlag_REACK(USART1))))
    //{
    //}
}

void MX_RS485_EnterStopMode(void)
{
    //使能接收引脚
    RS485_RXDEN_ON();

    //禁用DMA通道
    LL_DMA_DisableChannel(DMA1, DMA_RS485_RX_CHNL);
    LL_DMA_DisableChannel(DMA1, DMA_RS485_TX_CHNL);
    LL_DMA_SetDataLength(DMA1, DMA_RS485_RX_CHNL, SERCOMM_QUESTIONBUF_LENGTH);

    //使能RTO中断并清除标志位，等待接收完成
    LL_USART_EnableIT_RTO(USART_RS485);
    LL_USART_ClearFlag_RTO(USART_RS485);

    //禁用TC中断并清除标志位
    LL_USART_DisableIT_TC(USART_RS485);
    LL_USART_ClearFlag_TC(USART_RS485);

    //确保接收缓存为空，以防使能串口后，缓存内有一个字节会被DMA读取到
    /* Empty RX Fifo before entering Stop mode (Otherwise, characters already present in FIFO
    	 will lead to immediate wake up */
    while(LL_USART_IsActiveFlag_RXNE(USART_RS485))
    {
        /* Read Received character. RXNE flag is cleared by reading of RDR register */
        LL_USART_ReceiveData8(USART_RS485);
    }

    /* Clear OVERRUN flag */
    LL_USART_ClearFlag_ORE(USART_RS485);

    //禁用DMA以设置参数
    LL_DMA_DisableChannel(DMA1, DMA_RS485_TX_CHNL);
    LL_DMA_EnableChannel(DMA1, DMA_RS485_RX_CHNL);
    LL_DMA_SetDataLength(DMA1, DMA_RS485_RX_CHNL, SERCOMM_QUESTIONBUF_LENGTH);

    //使能USART接收器与DMA请求，使能DMA通道
    LL_DMA_EnableChannel(DMA1, DMA_RS485_RX_CHNL);
    g_usartInfo.questionBufCount = 0;
    //在唤醒时再启用，否则如果主机一直发送，唤醒后会接收到缓存的最后一个字符，影响正常接收
    LL_USART_EnableDMAReq_RX(USART_RS485);
}

void MX_RS485_Transmit_DMA(STRUCT_USART_INFO* pCommInfo)
{
    RS485_RXDEN_OFF();
    _delay_ms(1);//挂起1ms

    LL_USART_ClearFlag_TC(USART_RS485);
    LL_USART_EnableIT_TC(USART_RS485);
    //LL_DMA_DisableChannel(DMA1, DMA_RS485_RX_CHNL);
    LL_DMA_DisableChannel(DMA1, DMA_RS485_TX_CHNL);
    LL_DMA_SetDataLength(DMA1, DMA_RS485_TX_CHNL, (uint16_t)pCommInfo->responseLength);
    LL_DMA_EnableChannel(DMA1, DMA_RS485_TX_CHNL);
}

void MX_RS485_Receive_DMA(STRUCT_USART_INFO* pCommInfo)
{
    RS485_RXDEN_ON();
    _delay_ms(1);//挂起1ms
    LL_USART_ClearFlag_RTO(USART_RS485);
    LL_USART_EnableIT_RTO(USART_RS485);
    LL_DMA_EnableChannel(DMA1, DMA_RS485_RX_CHNL);
}

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART_RS485_IRQHandler(void)
{
    //如果增加以下代码，则波特率设置比如115200的时候，先用9600和模块通信（通信不上），然后在用115200也通信不上了。
    if(LL_USART_IsActiveFlag_ORE(USART_RS485)
            || LL_USART_IsActiveFlag_NE(USART_RS485)
            || LL_USART_IsActiveFlag_PE(USART_RS485)
            || LL_USART_IsActiveFlag_FE(USART_RS485))
    {
        //Clear Sequence
        LL_USART_ClearFlag_ORE(USART_RS485);
        LL_USART_ClearFlag_NE(USART_RS485);
        LL_USART_ClearFlag_PE(USART_RS485);
        LL_USART_ClearFlag_FE(USART_RS485);
        return;
    }

    if(LL_USART_IsActiveFlag_RTO(USART_RS485) && LL_USART_IsEnabledIT_RTO(USART_RS485))
    {
        //禁用RTO中断
        LL_USART_DisableIT_RTO(USART_RS485);
        LL_USART_ClearFlag_RTO(USART_RS485);

        uint16_t dataLen = (uint16_t)SERCOMM_QUESTIONBUF_LENGTH - LL_DMA_GetDataLength(DMA1, DMA_RS485_RX_CHNL);
        g_usartInfo.questionBufCount = dataLen;//message counter

        //禁用接收USART接收器与DMA请求，禁用DMA通道
        LL_DMA_DisableChannel(DMA1, DMA_RS485_RX_CHNL);
        //LL_USART_DisableDMAReq_RX(USART_RS485);
        LL_DMA_SetDataLength(DMA1, DMA_RS485_RX_CHNL, SERCOMM_QUESTIONBUF_LENGTH);
        UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_RS485), 0);
        return;
    }

    if(LL_USART_IsActiveFlag_TC(USART_RS485) && LL_USART_IsEnabledIT_TC(USART_RS485))
    {
        //禁用TC中断
        LL_USART_DisableIT_TC(USART_RS485);
        LL_USART_ClearFlag_TC(USART_RS485);
        MX_RS485_Receive_DMA(&g_usartInfo);
        return;
    }
}
