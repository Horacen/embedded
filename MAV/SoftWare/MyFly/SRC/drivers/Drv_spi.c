/******************** (C) COPYRIGHT 2017 My Tech ********************************
 * ����    �������ƴ�
 * ����    ��www.anotc.com
 * �Ա�    ��anotc.taobao.com
 * ����QȺ ��190169595
 * ����    ��SPI����
**********************************************************************************/
#include "Drv_spi.h"

void Drv_SPI1_init(void)
{
	SPI_InitTypeDef SPI_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI GPIO clock and SPI clock */						//72MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*!< SPI SCK pin configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*!< SPI MOSI pin configuration */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*!< SPI MISO pin configuration */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 		// 9MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);
    /* Enable SPI1 */
    SPI_Cmd(SPI1, ENABLE);
}

u8 Drv_SPI1_RW(u8 dat)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI_I2S_SendData(SPI1, dat);
	
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
			return SPI_I2S_ReceiveData(SPI1);
}
void Drv_SPI1_Transmit(uint8_t *pData, uint16_t Size)
{
    for(uint16_t i=0; i<Size; i++)
    {
        Drv_SPI1_RW(pData[i]);
    }
}
void Drv_SPI1_Receive(uint8_t *pData, uint16_t Size)
{
    for(uint16_t i=0; i<Size; i++)
    {
        pData[i] = Drv_SPI1_RW(0);
    }
}

/**
	*		SPI2Driver: Connected to NRF
	*/
void Drv_SPI2_init(void)
{
	SPI_InitTypeDef SPI_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI GPIO clock and SPI clock */						//36MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/*!< SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //˫��ȫ˫��
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//��ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//���ݴ�С8λ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 				//ʱ�Ӽ��ԣ�����ʱΪ��
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//��1��������Ч��������Ϊ����ʱ��
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS�ź����������
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //8��Ƶ��9MHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//��λ��ǰ
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
	/* Enable SPI1 */
	SPI_Cmd(SPI2, ENABLE);
}

u8 Drv_SPI2_RW(u8 dat)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
			SPI_I2S_SendData(SPI2, dat);
	
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
			return SPI_I2S_ReceiveData(SPI2);
}
void Drv_SPI2_Transmit(uint8_t *pData, uint16_t Size)
{
    for(uint16_t i=0; i<Size; i++)
    {
        Drv_SPI2_RW(pData[i]);
    }
}
void Drv_SPI2_Receive(uint8_t *pData, uint16_t Size)
{
    for(uint16_t i=0; i<Size; i++)
    {
        pData[i] = Drv_SPI2_RW(0);
    }
}
