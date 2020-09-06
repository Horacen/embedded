/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g����ģ��/nrf24l01+/���� ����  ֻʹ��һ�鿪���壬
	*					 ��2��NRFģ������շ�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� MINI-v3 STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_nrf.h"

/* NRF1 ��ģ��ֱ�Ӳ���NRFģ��ר�ýӿڼ���
 *
 * NRF2 �öŰ�����SPI2�ӿ���NRFģ������
 *
 * ģ��		--������
 * 1			--GND
 * 2			--3.3V
 * 3_CE		--PB12
 * 4_CSN	--PB8  
 * 5_SCK	--PB13
 * 6_MOSI	--PB15
 * 7_MISO	--PB14
 * 8_IRQ	--PB9	
 */
u8 status;	//�����жϽ���/����״̬
u8 txbuf[4]={0,1,2,3};	 //���ͻ���
u8 rxbuf[4];			 //���ջ���
int i=0;

void Self_Test(void);

 /**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)                  
{   
	/* ��ʼ��NRF1 */
  SPI_NRF_Init();
	
	/* ��ʼ��NRF2 */
	SPI_NRF2_Init();

  /* ����1��ʼ�� */
  USART1_Config();

  printf("\r\n ����һ�� NRF24L01 ���ߴ���ʵ�� \r\n");
  printf("\r\n �������ߴ��� ������ �ķ�����Ϣ\r\n");
  printf("\r\n   ���ڼ��NRF��MCU�Ƿ��������ӡ�����\r\n");

	Self_Test();	

}







 /**
  * @brief  NRFģ����Ժ�����NRF1��NRF2֮��ѭ����������
  * @param  ��
  * @retval ��
  */
void Self_Test(void)
{
  /*���NRFģ����MCU������*/
  status = NRF_Check(); 

  /*�ж�����״̬*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF1��MCU���ӳɹ���\r\n");  
  else	  
    printf("\r\n  NRF1��MCU����ʧ�ܣ������¼����ߡ�\r\n");
	
	
	  /*���NRFģ����MCU������*/
  status = NRF2_Check();
	
	  /*�ж�����״̬*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF2��MCU���ӳɹ���\r\n");  
  else	  
    printf("\r\n  NRF2��MCU����ʧ�ܣ������¼����ߡ�\r\n");

  while(1)
  {
    printf("\r\n ������ ������Ӧ����ģʽ\r\n"); 
    NRF_TX_Mode();
		
		printf("\r\n �ӻ��� �������ģʽ\r\n"); 
    NRF2_RX_Mode();

    /*��ʼ��������*/	
    status = NRF_Tx_Dat(txbuf);	  

    /*�жϷ���״̬*/
    switch(status)
    {
      case MAX_RT:
        printf("\r\n ������ û���յ�Ӧ���źţ����ʹ��������޶�ֵ������ʧ�ܡ� \r\n");
      break;

      case ERROR:
        printf("\r\n δ֪ԭ���·���ʧ�ܡ� \r\n");
      break;

      case TX_DS:
        printf("\r\n ������ ���յ� �ӻ��� ��Ӧ���źţ����ͳɹ��� \r\n");	 		
      break;  								
    }			   	
		
		
		/*�ȴ���������*/
    status = NRF2_Rx_Dat(rxbuf);

    /*�жϽ���״̬*/
    if(status == RX_DR)
    {
      for(i=0;i<4;i++)
      {	
        printf("\r\n �ӻ��� ���յ� ������ ���͵�����Ϊ��%d \r\n",rxbuf[i]); 
        /*�ѽ��յ�����+1���͸�����*/
        rxbuf[i]+=1;	  
        txbuf[i] = rxbuf[i]; 
      }
    }   
		
    printf("\r\n �ӻ��� ������Ӧ����ģʽ\r\n"); 
    NRF2_TX_Mode();
		
		

    printf("\r\n ������ �������ģʽ�� \r\n");	
    NRF_RX_Mode();
		
		 /*�����ط���ֱ�����ͳɹ�*/	  
    do
    { 	  
      status = NRF2_Tx_Dat(txbuf);
    }while(status == MAX_RT);

    /*�ȴ���������*/
    status = NRF_Rx_Dat(rxbuf);

    /*�жϽ���״̬*/
    switch(status)
    {
      case RX_DR:
      for(i=0;i<4;i++)
      {					
        printf("\r\n ������ ���յ� �ӻ��� ���͵�����Ϊ��%d \r\n",rxbuf[i]);
        txbuf[i] =rxbuf[i];
      }
      break;

      case ERROR:
        printf("\r\n ������ ���ճ���   \r\n");
      break;  		
    }
  }// while(1)

}


/*********************************************END OF FILE**********************/
