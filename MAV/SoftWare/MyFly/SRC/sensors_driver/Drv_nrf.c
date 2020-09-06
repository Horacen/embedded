


#include "Drv_nrf.h"
#include "Drv_spi.h"
#include "include.h"

void NRF_PinInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIO Clock */
	RCC_APB2PeriphClockCmd(NRF24L01_CS_RCC, ENABLE);
	
	/* CS */ //PB12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = NRF24L01_CS_PIN;
	GPIO_Init(NRF24L01_CS_GPIO, &GPIO_InitStructure);
	
	/* Set CS High */
	GPIO_SetBits(NRF24L01_CS_GPIO, NRF24L01_CS_PIN);
	
	/* CE Pin */	//PB10
	GPIO_InitStructure.GPIO_Pin   = NRF24L01_CE_PIN;
	GPIO_Init(NRF24L01_CE_GPIO, &GPIO_InitStructure);
}



static void nrf24l01_enable(u8 ena)
{
	if(ena)
		GPIO_ResetBits(NRF24L01_CS_GPIO, NRF24L01_CS_PIN);
	else
		GPIO_SetBits(NRF24L01_CS_GPIO, NRF24L01_CS_PIN);
}


static void nrf24l01_ce_low(u8 ena)
{
	if(ena)
		GPIO_ResetBits(NRF24L01_CE_GPIO, NRF24L01_CE_PIN);
	else
		GPIO_SetBits(NRF24L01_CE_GPIO, NRF24L01_CE_PIN);
	
}

/**
  * @brief   ������NRF�ض��ļĴ���д������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  *		@arg dat:��Ҫ��Ĵ���д�������
  * @retval  NRF��status�Ĵ�����״̬
  */
static void nrf24l01_writereg(u8 reg,u8 dat)
{	
	nrf24l01_ce_low(1);
	
  nrf24l01_enable(1);
	Drv_SPI2_RW(reg);
  Drv_SPI2_RW(dat); 
  nrf24l01_enable(0);	
}

/**
  * @brief   ���ڴ�NRF�ض��ļĴ�����������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  * @retval  �Ĵ����е�����
  */
u8 nrf24l01_readreg(u8 reg)
{
 	u8 reg_val;

	nrf24l01_ce_low(1);	
  nrf24l01_enable(1);
	Drv_SPI2_RW(reg); 
	reg_val = Drv_SPI2_RW(NOP);
	nrf24l01_enable(0);		
   	
	return reg_val;
}	

/**
  * @brief   ������NRF�ļĴ�����д��һ������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf�����ڴ洢���������ļĴ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
  */
void nrf24l01_readbuf(u8 reg,u8 *pBuf,u8 bytes)
{
 	u8 byte_cnt;

	nrf24l01_ce_low(1);	
  nrf24l01_enable(1);	
	Drv_SPI2_RW(reg); 
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)		  
	   pBuf[byte_cnt] = Drv_SPI2_RW(NOP); //��NRF24L01��ȡ����  
	nrf24l01_enable(0);	
}

/**
  * @brief   ������NRF�ļĴ�����д��һ������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf���洢�˽�Ҫд��д�Ĵ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
  */
u8 nrf24l01_writebuf(u8 reg ,u8 *pBuf,u8 bytes)
{
	u8 status,byte_cnt;
	
  nrf24l01_ce_low(1);	
  nrf24l01_enable(1);	
  status = Drv_SPI2_RW(reg); 
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		Drv_SPI2_RW(*pBuf++);	//д���ݵ������� 	 
	nrf24l01_enable(0);	
  
  return (status);	//����NRF24L01��״̬ 		
}

/**
  * @brief  ���ò��������ģʽ
  * @param  ��
  * @retval ��
  */
u8 my_remote[5]={0xCA,0xAC,0x58,0xCE,0xEA};		//remote address(PTX)
static void nrf244l01_prx(void)
{
	nrf24l01_ce_low(1);	

   nrf24l01_writebuf(NRF_WRITE_REG+RX_ADDR_P0, my_remote, 5);//дRX�ڵ��ַ
   nrf24l01_writereg(NRF_WRITE_REG+EN_AA, 0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
   nrf24l01_writereg(NRF_WRITE_REG+EN_RXADDR, 0x01);//ʹ��ͨ��0�Ľ��յ�ַ    
   nrf24l01_writereg(NRF_WRITE_REG+RF_CH, CHANAL);      //����RFͨ��Ƶ��    
   nrf24l01_writereg(NRF_WRITE_REG+RX_PW_P0, 12);//ѡ��ͨ��0����Ч���ݿ��      
	 nrf24l01_writereg(NRF_WRITE_REG+FEATURE, 0x02);//enable payload with ack packets
   nrf24l01_writereg(NRF_WRITE_REG+RF_SETUP, 0x0f); //����TX�������,0db����,2Mbps,���������濪��   
   nrf24l01_writereg(NRF_WRITE_REG+CONFIG, 0x0f);  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ
	
	/*CE���ߣ��������ģʽ*/	
  nrf24l01_ce_low(0);
}    

/**
  * @brief  ��Ҫ����NRF��MCU�Ƿ���������
  * @param  ��
  * @retval SUCCESS/ERROR ��������/����ʧ��
  */
static u8 nrf24l01_check(void)
{
	u8 id=0;
	id = nrf24l01_readreg(NRF_READ_REG+CONFIG);
	id = id & 0x01;									//read config register's PRIM_RX bit, if 1, success
	
	return id;
}


void nrf24l01_int_config(void)	//PB11 active low 
{
	NVIC_InitTypeDef NVIC_InitStructure ;
	EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
	
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11 ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, GPIO_Pin_11);
	
	/* Connect EXTI0 Line to PA.00 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
	/* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;;               
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_NRF_P;    	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_NRF_S;           
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                
	NVIC_Init(&NVIC_InitStructure);                               
}


u8 NRF_Init(void)
{
	u8 id=0;
	NRF_PinInit();
	nrf24l01_int_config();
	nrf244l01_prx();
	id = nrf24l01_check();
	/*CE���ߣ��������ģʽ*/	
  nrf24l01_ce_low(0);
	return id;
}


/**
  * @brief   ������NRF�ķ��ͻ�������д������
  * @param   
  *		@arg txBuf���洢�˽�Ҫ���͵����ݵ����飬�ⲿ����	
  * @retval  LSB first
  */
u8 battery_voltage[2];

void nrf24l01_ackpacket(u8 *ack_payload)
{
	nrf24l01_ce_low(1);	
  nrf24l01_enable(1);	

	/* flush Tx FIFO */						
  nrf24l01_writereg(FLUSH_TX, NOP);
	/* Write ack payload */
  nrf24l01_writebuf(W_ACK_PAYLOAD+0x00, ack_payload, 4);  

	nrf24l01_enable(0);
	nrf24l01_ce_low(0);	
} 


/**
  * @brief   ���ڴ�NRF�Ľ��ջ������ж�������
  * @param   
  *		@arg rxBuf �����ڽ��ո����ݵ����飬�ⲿ����	
  * @retval 
  *		@arg ���ս��
  */
u8 remote_data[12];
u8 nrf24l01_readdr(u8 *rxbuf)
{
	u8 state; 
	
  nrf24l01_ce_low(1);	 	 //�������״̬
	/*��ȡstatus�Ĵ�����ֵ  */               
	state=nrf24l01_readreg(STATUS);
	/* ����жϱ�־*/      
	nrf24l01_writereg(NRF_WRITE_REG+STATUS,state);
	/*�ж��Ƿ���յ�����*/
	if(state & RX_DR)                                 //���յ�����
	{
	  nrf24l01_readbuf(RD_RX_PLOAD,rxbuf,12);//��ȡ����
	  nrf24l01_writereg(FLUSH_RX, NOP);          //���RX FIFO�Ĵ���
	  return 1; 
	}
	else    
		return 0;                    //û�յ��κ�����
}






