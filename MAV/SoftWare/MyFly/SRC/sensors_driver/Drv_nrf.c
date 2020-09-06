


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
  * @brief   用于向NRF特定的寄存器写入数据
  * @param   
  *		@arg reg:NRF的命令+寄存器地址
  *		@arg dat:将要向寄存器写入的数据
  * @retval  NRF的status寄存器的状态
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
  * @brief   用于从NRF特定的寄存器读出数据
  * @param   
  *		@arg reg:NRF的命令+寄存器地址
  * @retval  寄存器中的数据
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
  * @brief   用于向NRF的寄存器中写入一串数据
  * @param   
  *		@arg reg : NRF的命令+寄存器地址
  *		@arg pBuf：用于存储将被读出的寄存器数据的数组，外部定义
  * 	@arg bytes: pBuf的数据长度
  * @retval  NRF的status寄存器的状态
  */
void nrf24l01_readbuf(u8 reg,u8 *pBuf,u8 bytes)
{
 	u8 byte_cnt;

	nrf24l01_ce_low(1);	
  nrf24l01_enable(1);	
	Drv_SPI2_RW(reg); 
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)		  
	   pBuf[byte_cnt] = Drv_SPI2_RW(NOP); //从NRF24L01读取数据  
	nrf24l01_enable(0);	
}

/**
  * @brief   用于向NRF的寄存器中写入一串数据
  * @param   
  *		@arg reg : NRF的命令+寄存器地址
  *		@arg pBuf：存储了将要写入写寄存器数据的数组，外部定义
  * 	@arg bytes: pBuf的数据长度
  * @retval  NRF的status寄存器的状态
  */
u8 nrf24l01_writebuf(u8 reg ,u8 *pBuf,u8 bytes)
{
	u8 status,byte_cnt;
	
  nrf24l01_ce_low(1);	
  nrf24l01_enable(1);	
  status = Drv_SPI2_RW(reg); 
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		Drv_SPI2_RW(*pBuf++);	//写数据到缓冲区 	 
	nrf24l01_enable(0);	
  
  return (status);	//返回NRF24L01的状态 		
}

/**
  * @brief  配置并进入接收模式
  * @param  无
  * @retval 无
  */
u8 my_remote[5]={0xCA,0xAC,0x58,0xCE,0xEA};		//remote address(PTX)
static void nrf244l01_prx(void)
{
	nrf24l01_ce_low(1);	

   nrf24l01_writebuf(NRF_WRITE_REG+RX_ADDR_P0, my_remote, 5);//写RX节点地址
   nrf24l01_writereg(NRF_WRITE_REG+EN_AA, 0x01);    //使能通道0的自动应答    
   nrf24l01_writereg(NRF_WRITE_REG+EN_RXADDR, 0x01);//使能通道0的接收地址    
   nrf24l01_writereg(NRF_WRITE_REG+RF_CH, CHANAL);      //设置RF通信频率    
   nrf24l01_writereg(NRF_WRITE_REG+RX_PW_P0, 12);//选择通道0的有效数据宽度      
	 nrf24l01_writereg(NRF_WRITE_REG+FEATURE, 0x02);//enable payload with ack packets
   nrf24l01_writereg(NRF_WRITE_REG+RF_SETUP, 0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
   nrf24l01_writereg(NRF_WRITE_REG+CONFIG, 0x0f);  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
	
	/*CE拉高，进入接收模式*/	
  nrf24l01_ce_low(0);
}    

/**
  * @brief  主要用于NRF与MCU是否正常连接
  * @param  无
  * @retval SUCCESS/ERROR 连接正常/连接失败
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
	/*CE拉高，进入接收模式*/	
  nrf24l01_ce_low(0);
	return id;
}


/**
  * @brief   用于向NRF的发送缓冲区中写入数据
  * @param   
  *		@arg txBuf：存储了将要发送的数据的数组，外部定义	
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
  * @brief   用于从NRF的接收缓冲区中读出数据
  * @param   
  *		@arg rxBuf ：用于接收该数据的数组，外部定义	
  * @retval 
  *		@arg 接收结果
  */
u8 remote_data[12];
u8 nrf24l01_readdr(u8 *rxbuf)
{
	u8 state; 
	
  nrf24l01_ce_low(1);	 	 //进入待机状态
	/*读取status寄存器的值  */               
	state=nrf24l01_readreg(STATUS);
	/* 清除中断标志*/      
	nrf24l01_writereg(NRF_WRITE_REG+STATUS,state);
	/*判断是否接收到数据*/
	if(state & RX_DR)                                 //接收到数据
	{
	  nrf24l01_readbuf(RD_RX_PLOAD,rxbuf,12);//读取数据
	  nrf24l01_writereg(FLUSH_RX, NOP);          //清除RX FIFO寄存器
	  return 1; 
	}
	else    
		return 0;                    //没收到任何数据
}






