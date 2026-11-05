#include "hal_spi.h"
#include "hal_gpio.h"
#include "delay_utils.h"

void SPI_Soft_Init(void)
{
    // 开启GPIO时钟（根据你的引脚修改端口）
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    // GPIO配置：推挽输出、无上下拉、高速
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // SCK + MOSI 推挽输出
    GPIO_InitStruct.Pin = SPI_SCK_GPIO_PIN | SPI_MOSI_GPIO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStruct);
		
    // CS 推挽输出
    GPIO_InitStruct.Pin = SPI_CS_GPIO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPI_CS_GPIO_PORT, &GPIO_InitStruct);

    // MISO 浮空输入
    GPIO_InitStruct.Pin = SPI_MISO_GPIO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

    // 初始电平：CS拉高，SCK按SPI模式设置
    SPI_CS_HIGH();
    if(SPI_CPOL == 1)
        SPI_SCK_HIGH();
    else
        SPI_SCK_LOW();
}

void SPI_Soft_DeInit(void)
{
    // 开启GPIO时钟（根据你的引脚修改端口）
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    // GPIO配置：推挽输出、低电平
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // SCK + MOSI + CS 推挽输出
    GPIO_InitStruct.Pin = SPI_SCK_GPIO_PIN | SPI_MOSI_GPIO_PIN | SPI_CS_GPIO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

    // MISO 浮空输入
    GPIO_InitStruct.Pin = SPI_MISO_GPIO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStruct);
}

static void SPI_Soft_DelayUs(uint32_t us)
{
    _delay_us(us);  // STM32L0 LL库最小1ms，如需精准us可自行实现
}

uint8_t SPI_Soft_SwapByte(uint8_t tx)
{
    uint8_t rx = 0;
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        SPI_SCK_HIGH();

        // 发送高位数据
        if(tx & 0x80)
            SPI_MOSI_HIGH();
        else
            SPI_MOSI_LOW();
        tx <<= 1;

        SPI_Soft_DelayUs(SPI_DELAY_US);
        SPI_SCK_LOW();

        rx <<= 1;  // 第二个边沿采样

        // 读取MISO电平
        if(SPI_MISO_READ())
            rx |= 0x01;
    }

    return rx;
}

void SPI_Soft_WriteBytes(uint8_t* pData, uint16_t len)
{
    SPI_CS_LOW();
    while(len--)
    {
        SPI_Soft_SwapByte(*pData++);
    }
    SPI_CS_HIGH();
}

void SPI_Soft_ReadBytes(uint8_t* pData, uint16_t len)
{
    SPI_CS_LOW();
    while(len--)
    {
        *pData++ = SPI_Soft_SwapByte(0xFF);  // 主机发0xFF提供时钟
    }
    SPI_CS_HIGH();
}
