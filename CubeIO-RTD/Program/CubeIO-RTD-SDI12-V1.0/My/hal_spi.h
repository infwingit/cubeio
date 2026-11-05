#ifndef __SPI_HAL_H__
#define __SPI_HAL_H__
#include "main.h"

/************************* SPI 引脚配置（可自由修改） *************************/
// SPI时钟引脚 SCK
#define SPI_SCK_GPIO_PORT       GPIOA
#define SPI_SCK_GPIO_PIN        LL_GPIO_PIN_6
// SPI主机输出从机输入 MOSI
#define SPI_MOSI_GPIO_PORT      GPIOA
#define SPI_MOSI_GPIO_PIN       LL_GPIO_PIN_7
// SPI主机输入从机输出 MISO
#define SPI_MISO_GPIO_PORT      GPIOB
#define SPI_MISO_GPIO_PIN       LL_GPIO_PIN_1
// SPI片选引脚 CS（可根据设备修改，支持多设备）
#define SPI_CS_GPIO_PORT        GPIOB
#define SPI_CS_GPIO_PIN         LL_GPIO_PIN_0

/************************* SPI 模式配置（CPOL/CPHA） *************************/
// 模式0：CPOL=0 空闲低电平 | CPHA=0 第一个边沿采样
#define SPI_CPOL                0
#define SPI_CPHA                1

/************************* SPI 速率配置（延时微秒数） *************************/
// 延时越小速率越快，STM32L0 推荐 1~5us
#define SPI_DELAY_US            10

/************************* 引脚电平操作宏定义 *************************/
#define SPI_SCK_HIGH()          LL_GPIO_SetOutputPin(SPI_SCK_GPIO_PORT, SPI_SCK_GPIO_PIN)
#define SPI_SCK_LOW()           LL_GPIO_ResetOutputPin(SPI_SCK_GPIO_PORT, SPI_SCK_GPIO_PIN)

#define SPI_MOSI_HIGH()         LL_GPIO_SetOutputPin(SPI_MOSI_GPIO_PORT, SPI_MOSI_GPIO_PIN)
#define SPI_MOSI_LOW()          LL_GPIO_ResetOutputPin(SPI_MOSI_GPIO_PORT, SPI_MOSI_GPIO_PIN)

#define SPI_MISO_READ()          LL_GPIO_IsInputPinSet(SPI_MISO_GPIO_PORT, SPI_MISO_GPIO_PIN)

#define SPI_CS_HIGH()            LL_GPIO_SetOutputPin(SPI_CS_GPIO_PORT, SPI_CS_GPIO_PIN)
#define SPI_CS_LOW()             LL_GPIO_ResetOutputPin(SPI_CS_GPIO_PORT, SPI_CS_GPIO_PIN)

/************************* 函数声明 *************************/
void SPI_Soft_Init(void);                // 软件SPI初始化
void SPI_Soft_DeInit(void);              // 软件SPI反向初始化
uint8_t SPI_Soft_SwapByte(uint8_t tx);   // 单字节读写（主机必须先发）
void SPI_Soft_WriteBytes(uint8_t* pData, uint16_t len);  // 批量写
void SPI_Soft_ReadBytes(uint8_t* pData, uint16_t len);   // 批量读

#endif
