#ifndef HAL_USART_H_
#define HAL_USART_H_

#include "main.h"

//******************************************
// GPIO定义
//******************************************
/* Definition for LPUSART1 Pins */
// GPIOA.2.3作为RS485(USART2)通信接口，在相应的文件中初始化
#define RS485_TXD_PORT          GPIOA
#define RS485_TXD_PIN           LL_GPIO_PIN_2

#define RS485_RXD_PORT          GPIOA
#define RS485_RXD_PIN           LL_GPIO_PIN_3

#define RS485_TXDEN_PORT        GPIOA
#define RS485_TXDEN_PIN         LL_GPIO_PIN_4
#define RS485_RXDEN_OFF()       LL_GPIO_SetOutputPin(RS485_TXDEN_PORT,RS485_TXDEN_PIN)
#define RS485_RXDEN_ON()        LL_GPIO_ResetOutputPin(RS485_TXDEN_PORT,RS485_TXDEN_PIN)

#define USART_RS485 									   USART2
#define USART_RS485_PORT                 LL_IOP_GRP1_PERIPH_GPIOA
#define USART_RS485_GPIO_ALTERNATIVE_FUNCTION  LL_GPIO_AF_4

#define DMA_RS485_RX_REQUESTNUMBER       LL_DMA_REQUEST_4
#define DMA_RS485_TX_REQUESTNUMBER       LL_DMA_REQUEST_4

#define DMA_RS485_RX_CHNL 						   LL_DMA_CHANNEL_5
#define DMA_RS485_TX_CHNL 						   LL_DMA_CHANNEL_4

#define LL_APB1_GRP1_PERIPH_USART_RS485  LL_APB1_GRP1_PERIPH_USART2
#define USART_RS485_IRQHandler           USART2_IRQHandler

#define DMA1_IRQHandler_ForRS485RS232    DMA1_Channel4_5_6_7_IRQHandler

#define LL_DMA_IsActiveFlag_TC_ForRS485RS232 LL_DMA_IsActiveFlag_TC4
#define LL_DMA_ClearFlag_GI_ForRS485RS232    LL_DMA_ClearFlag_GI4

#define LL_DMA_IsActiveFlag_TE_ForRS485RS232 LL_DMA_IsActiveFlag_TE4
#define LL_DMA_ClearFlag_TE_ForRS485RS232    LL_DMA_ClearFlag_TE4

#define LL_RCC_USART_RS485_CLKSOURCE_HSI LL_RCC_USART2_CLKSOURCE_HSI
#define LL_RCC_USART_RS485_CLKSOURCE_LSE LL_RCC_USART2_CLKSOURCE_LSE

//******************************************
// 变量定义
//******************************************
#define SERCOMM_QUESTIONBUF_LENGTH	    COMMAND_QUESTIONBUF_LENGTH
#define SERCOMM_RESPONSEBUF_LENGTH	    COMMAND_RESPONSEBUF_LENGTH

typedef enum
{
    //port is idle
    USART_IDLE = 0,
    //port is recving now
    USART_RCVING = 1,
    //a frame was recved
    USART_RCVED = 2,
    //port is transmitting now
    USART_XMITING = 3
} ENUM_USART_STATUS;

typedef struct STRUCT_USART_INFO
{
    uint8_t baudrate;//baudrate selection
    uint8_t parity;//checksum type
    uint8_t protocol;//protocol
    uint8_t address;//modbus slave address
    uint8_t addressSwitch;
    uint8_t dataBits;//data bits for usart comm, 7,8
    uint8_t stopBits;//data bits for usart comm, 1,2

    volatile uint8_t timeoutConstant;//3.5 byte timeout constant
    volatile uint8_t timeoutCounter;//3.5 bytes timeout counter
    volatile ENUM_USART_STATUS usartStatus;
    volatile uint8_t questionBufCount;//send/receive counter
    volatile uint8_t responseLength;//data length to be sent

    volatile uint8_t responseDelayConstant;//response delay time, for master switch from SEND to RECEIVE, TimerBase=10ms
    volatile uint8_t responseDelayCounter;//response delay start time

    volatile uint8_t actvOutputIntervalConstant;//active output interval, unit is second
    volatile uint8_t actvOutputIntervalCounter;//response delay start time

    uint8_t questionBuf[SERCOMM_QUESTIONBUF_LENGTH];//receive buffer
    uint8_t responseBuf[SERCOMM_RESPONSEBUF_LENGTH];//send buffer
} STRUCT_USART_INFO;
extern STRUCT_USART_INFO g_usartInfo;

//----------------------------------------------
//Serial Comm Constant
//----------------------------------------------
//Value for STRUCT_USART_INFO.commBaudrate
#define SERCOMM_BAUDRATE_1200          0
#define SERCOMM_BAUDRATE_2400          1
#define SERCOMM_BAUDRATE_4800          2
#define SERCOMM_BAUDRATE_9600          3
#define SERCOMM_BAUDRATE_19200         4
#define SERCOMM_BAUDRATE_38400         5
#define SERCOMM_BAUDRATE_57600         6
#define SERCOMM_BAUDRATE_115200        7
#define SERCOMM_BAUDRATE_END         SERCOMM_BAUDRATE_38400

//Value for STRUCT_USART_INFO.commProtocol
#define SERCOMM_PROTOCOL_MODRTU         0
#define SERCOMM_PROTOCOL_END         SERCOMM_PROTOCOL_MODRTU

//Value for STRUCT_USART_INFO.commParity
#define SERCOMM_PARITY_NONE             0
#define SERCOMM_PARITY_EVEN             1
#define SERCOMM_PARITY_ODD              2

//Value for STRUCT_USART_INFO.commDataBits
#define SERCOMM_DATABITS_7               0
#define SERCOMM_DATABITS_8               1

//Value for STRUCT_USART_INFO.commStopBits
#define SERCOMM_STOPBITS_1               0
#define SERCOMM_STOPBITS_2               1

//Value for STRUCT_USART_INFO.responseDelay
#define SERCOMM_RESPONSEDELAY_0MS        0
#define SERCOMM_RESPONSEDELAY_10MS       1
#define SERCOMM_RESPONSEDELAY_20MS       2
#define SERCOMM_RESPONSEDELAY_30MS       3
#define SERCOMM_RESPONSEDELAY_40MS       4
#define SERCOMM_RESPONSEDELAY_50MS       5
#define SERCOMM_RESPONSEDELAY_2550MS     255

//Value for STRUCT_USART_INFO.activeOutputInterval
#define SERCOMM_ACTVOUTPUTINTERVAL_0S        0 //Disabled
#define SERCOMM_ACTVOUTPUTINTERVAL_2S        2
#define SERCOMM_ACTVOUTPUTINTERVAL_255S      255

//------------------------------------------
//Comm Macro Define
//------------------------------------------
//Communication Realted
//common
void MX_RS485_Receive_DMA(STRUCT_USART_INFO* pCommInfo);
void MX_RS485_Transmit_DMA(STRUCT_USART_INFO* pCommInfo);
void MX_RS485_StartRx(void (*MyRxCpltCallback)(uint8_t* rxChar, uint16_t size, uint8_t error));
void MX_RS485_Init(void);
void MX_RS485_EnterStopMode(void);
void MX_RS485_Process(STRUCT_USART_INFO* pCommInfo);
uint8_t MX_RS485_IsEnabled(void);

#endif /* HAL_USART_H_ */
