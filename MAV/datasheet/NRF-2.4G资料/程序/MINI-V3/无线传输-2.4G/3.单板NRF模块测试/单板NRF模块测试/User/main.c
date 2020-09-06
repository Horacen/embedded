/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g无线模块/nrf24l01+/单板 测试  只使用一块开发板，
	*					 对2个NRF模块进行收发测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 MINI-v3 STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_nrf.h"

/* NRF1 把模块直接插入NRF模块专用接口即可
 *
 * NRF2 用杜邦线在SPI2接口与NRF模块相连
 *
 * 模块		--开发板
 * 1			--GND
 * 2			--3.3V
 * 3_CE		--PB12
 * 4_CSN	--PB8  
 * 5_SCK	--PB13
 * 6_MOSI	--PB15
 * 7_MISO	--PB14
 * 8_IRQ	--PB9	
 */
u8 status;	//用于判断接收/发送状态
u8 txbuf[4]={0,1,2,3};	 //发送缓冲
u8 rxbuf[4];			 //接收缓冲
int i=0;

void Self_Test(void);

 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)                  
{   
	/* 初始化NRF1 */
  SPI_NRF_Init();
	
	/* 初始化NRF2 */
	SPI_NRF2_Init();

  /* 串口1初始化 */
  USART1_Config();

  printf("\r\n 这是一个 NRF24L01 无线传输实验 \r\n");
  printf("\r\n 这是无线传输 主机端 的反馈信息\r\n");
  printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

	Self_Test();	

}







 /**
  * @brief  NRF模块测试函数，NRF1和NRF2之间循环发送数据
  * @param  无
  * @retval 无
  */
void Self_Test(void)
{
  /*检测NRF模块与MCU的连接*/
  status = NRF_Check(); 

  /*判断连接状态*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF1与MCU连接成功！\r\n");  
  else	  
    printf("\r\n  NRF1与MCU连接失败，请重新检查接线。\r\n");
	
	
	  /*检测NRF模块与MCU的连接*/
  status = NRF2_Check();
	
	  /*判断连接状态*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF2与MCU连接成功！\r\n");  
  else	  
    printf("\r\n  NRF2与MCU连接失败，请重新检查接线。\r\n");

  while(1)
  {
    printf("\r\n 主机端 进入自应答发送模式\r\n"); 
    NRF_TX_Mode();
		
		printf("\r\n 从机端 进入接收模式\r\n"); 
    NRF2_RX_Mode();

    /*开始发送数据*/	
    status = NRF_Tx_Dat(txbuf);	  

    /*判断发送状态*/
    switch(status)
    {
      case MAX_RT:
        printf("\r\n 主机端 没接收到应答信号，发送次数超过限定值，发送失败。 \r\n");
      break;

      case ERROR:
        printf("\r\n 未知原因导致发送失败。 \r\n");
      break;

      case TX_DS:
        printf("\r\n 主机端 接收到 从机端 的应答信号，发送成功！ \r\n");	 		
      break;  								
    }			   	
		
		
		/*等待接收数据*/
    status = NRF2_Rx_Dat(rxbuf);

    /*判断接收状态*/
    if(status == RX_DR)
    {
      for(i=0;i<4;i++)
      {	
        printf("\r\n 从机端 接收到 主机端 发送的数据为：%d \r\n",rxbuf[i]); 
        /*把接收的数据+1后发送给主机*/
        rxbuf[i]+=1;	  
        txbuf[i] = rxbuf[i]; 
      }
    }   
		
    printf("\r\n 从机端 进入自应答发送模式\r\n"); 
    NRF2_TX_Mode();
		
		

    printf("\r\n 主机端 进入接收模式。 \r\n");	
    NRF_RX_Mode();
		
		 /*不断重发，直至发送成功*/	  
    do
    { 	  
      status = NRF2_Tx_Dat(txbuf);
    }while(status == MAX_RT);

    /*等待接收数据*/
    status = NRF_Rx_Dat(rxbuf);

    /*判断接收状态*/
    switch(status)
    {
      case RX_DR:
      for(i=0;i<4;i++)
      {					
        printf("\r\n 主机端 接收到 从机端 发送的数据为：%d \r\n",rxbuf[i]);
        txbuf[i] =rxbuf[i];
      }
      break;

      case ERROR:
        printf("\r\n 主机端 接收出错。   \r\n");
      break;  		
    }
  }// while(1)

}


/*********************************************END OF FILE**********************/
