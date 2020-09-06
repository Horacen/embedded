/******************** (C) COPYRIGHT 2017 My Tech ********************************
 * 作者    ：匿名科创
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
 * 描述    ：串口驱动
**********************************************************************************/
#include "Drv_usart.h"
#include "My_DT.h"
#include "My_OF.h"
#include "include.h"
#include "Drv_UP_Flow.h"

void Usart1_Init ( u32 br_num )
{
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_USART1, ENABLE ); //开启USART2时钟
    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE );

    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART1_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART1_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );

    //配置PA9作为USART1　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );
    //配置PA10作为USART1　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );

    //配置USART2
    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    //配置USART2时钟
    USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;  //时钟低电平活动
    USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  //SLCK引脚上时钟输出的极性->低电平
    USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;  //时钟第二个边沿进行数据捕获
    USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; //最后一位数据的时钟脉冲不从SCLK输出

    USART_Init ( USART1, &USART_InitStructure );
    USART_ClockInit ( USART1, &USART_ClockInitStruct );

    //使能USART2接收中断
    USART_ITConfig ( USART1, USART_IT_RXNE, ENABLE );
    //使能USART2
    USART_Cmd ( USART1, ENABLE );
//	//使能发送（进入移位）中断
//	if(!(USART2->CR1 & USART_CR1_TXEIE))
//	{
//		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//	}


}

u8 TxBuffer[256];
u8 TxCounter = 0;
u8 count = 0;

u8 Rx_Buf[256];	//串口接收缓存

void Usart1_IRQ ( void )
{
    u8 com_data;

    if ( USART1->SR & USART_SR_ORE ) //ORE中断
    {
        com_data = USART1->DR;
    }

    //接收中断
    if ( USART_GetITStatus ( USART1, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( USART1, USART_IT_RXNE ); //清除中断标志

        com_data = USART1->DR;
//        ANO_DT_Data_Receive_Prepare ( com_data );
			//数据接收
    }
    //发送（进入移位）中断
    if ( USART_GetITStatus ( USART1, USART_IT_TXE ) )
    {

        USART1->DR = TxBuffer[TxCounter++]; //写DR清除中断标志
        if ( TxCounter == count )
        {
            USART1->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }

        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }



}

void Usart1_Send ( unsigned char *DataToSend , u8 data_num )
{
    u8 i;
    for ( i = 0; i < data_num; i++ )
    {
        TxBuffer[count++] = * ( DataToSend + i );
    }

    if ( ! ( USART1->CR1 & USART_CR1_TXEIE ) )
    {
        USART_ITConfig ( USART1, USART_IT_TXE, ENABLE ); //打开发送中断
    }

}



void Uart2_Init ( u32 br_num )
{
    USART_InitTypeDef USART_InitStructure;
    //USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_USART2, ENABLE ); //开启USART2时钟
    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE );

    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART2_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART2_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );


    GPIO_PinRemapConfig ( GPIO_Remap_USART2, ENABLE );

    //配置PA2作为UART2　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );
    //配置PA3作为UART2　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );

    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    USART_Init ( USART2, &USART_InitStructure );



    //使能UART5接收中断
    USART_ITConfig ( USART2, USART_IT_RXNE, ENABLE );
    //使能USART5
    USART_Cmd ( USART2, ENABLE );
//	//使能发送（进入移位）中断
//	if(!(USART2->CR1 & USART_CR1_TXEIE))
//	{
//		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//	}

}
u8 Tx2Buffer[256];
u8 Tx2Counter = 0;
u8 count2 = 0;

void Uart2_IRQ ( void )
{
    //接收中断
    if ( USART_GetITStatus ( USART2, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( USART2, USART_IT_RXNE ); //清除中断标志

        u8 com_data = USART2->DR;

        OFGetByte( com_data );
    }

    //发送（进入移位）中断
    if ( USART_GetITStatus ( USART2, USART_IT_TXE ) )
    {

        USART2->DR = Tx2Buffer[Tx2Counter++]; //写DR清除中断标志

        if ( Tx2Counter == count2 )
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }

    }

}

void Uart2_Send ( unsigned char *DataToSend , u8 data_num )
{
    u8 i;
    for ( i = 0; i < data_num; i++ )
    {
        Tx2Buffer[count2++] = * ( DataToSend + i );
    }

    if ( ! ( USART2->CR1 & USART_CR1_TXEIE ) )
    {
        USART_ITConfig ( USART2, USART_IT_TXE, ENABLE ); //打开发送中断
    }

}


