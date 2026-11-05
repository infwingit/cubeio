#ifndef SDI12_HAL_H_
#define SDI12_HAL_H_

#include "main.h"

#define SDI12_CAHR_BREAK    0x00
#define SDI12_CAHR_MARKING  0xFF

typedef enum ENUM_SDISTATEMACHINE
{
    SDI_STANDBY,
    SDI_FIRST_BREAK_DETECTING,
    SDI_FIRST_MARKING_DETECTING,
    SDI_ADDRESS,
    SDI_COMMAND,
    SDI_PROCESS
} ENUM_SDISTATEMACHINE;
extern volatile ENUM_SDISTATEMACHINE g_sdiStateMachine;

typedef enum ENUM_DCOMMANDRETTYPE
{
    RETTYPE_DATA = 0,
    RETTYPE_VERIFY,
    RETTYPE_NODATA,
    RETTYPE_DATA_CRC,
    RETTYPE_NODATA_CRC,
    RETTYPE_M,
    RETTYPE_M_CRC,
    RETTYPE_M0,
    RETTYPE_M0_CRC,
    RETTYPE_M1,
    RETTYPE_M1_CRC,
    RETTYPE_M2,
    RETTYPE_M2_CRC,
    RETTYPE_M3,
    RETTYPE_M3_CRC,
    RETTYPE_M4,
    RETTYPE_M4_CRC,
    RETTYPE_M5,
    RETTYPE_M5_CRC,
    RETTYPE_M6,
    RETTYPE_M6_CRC,
    RETTYPE_M7,
    RETTYPE_M7_CRC,
    RETTYPE_M8,
    RETTYPE_M8_CRC,
    RETTYPE_M9,
    RETTYPE_M9_CRC,
} ENUM_DCOMMANDRETTYPE;

typedef enum ENUM_PROTOCOL
{
    PROTOCOL_DDI,
    PROTOCOL_SDI12
} ENUM_PROTOCOL;

#define vm_UART_RX_BUF_L 196//接收缓存长度, 实际只存储127个 数据
#define vm_UART_TX_BUF_L 196//发送缓冲数据长度
typedef struct STRUCT_SDI12_INFO
{
    char addr;

    volatile uint8_t vm_UART_RX_P_HEAD;//接收缓存指针
    volatile uint8_t vm_UART_RX_P_TAIL;//接收缓存指针
    volatile uint8_t vm_UART_RX_BUF[vm_UART_RX_BUF_L];//环形接收缓存

    volatile uint8_t vm_UART_TX_CNT;//要发送的数据个数
    volatile uint8_t vm_UART_TX_BUF[vm_UART_TX_BUF_L];//发送缓存

    char vm_CommandBuffer[vm_UART_RX_BUF_L];
    uint8_t vm_CommandBufferIndex;

    volatile ENUM_DCOMMANDRETTYPE DCommandRetType;
    volatile ENUM_PROTOCOL protocol;

} STRUCT_SDI12_INFO;
extern STRUCT_SDI12_INFO g_sdiCommInfo;

extern void ddiInit(void);
extern uint8_t ddiPrepare(char* buf);
extern void ddiSend(void);

extern void Sdi12Init(void);
extern void Sdi12DeInit(void);
extern void Sdi12SetTXPinSingleWireRX(void);
extern void Sdi12SetTXPinSingleWireTX(void);
extern uint8_t sdi12PushChar(char value);
extern uint8_t sdi12GetChar(char* pChar);
extern uint8_t sdi12FetchChar(char* pChar);
extern void sdi12Send(char* buf, uint8_t count, uint8_t crcEnabled);
extern void sdi12Crc16Caculate(uint8_t* buf, uint8_t count, uint8_t* bufCrc16) ;
extern void sdi12SendError(void);

#endif
