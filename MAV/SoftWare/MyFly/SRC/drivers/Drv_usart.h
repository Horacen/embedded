#ifndef _USART_H
#define _USART_H

#include "stm32f10x.h"

extern u8 Rx_Buf[];
void Usart1_Init(u32 br_num);
void Usart1_IRQ(void);
void Usart1_Send(unsigned char *DataToSend ,u8 data_num);


void Usart2_Init(u32 br_num);
void Usart2_IRQ(void);
void Usart2_Send(unsigned char *DataToSend ,u8 data_num);
#endif
