

#include "include.h"
#include "Drv_pwm_out.h"
#include "Drv_led.h"
#include "Drv_spi.h"
#include "Drv_icm20602.h"
#include "drv_ak8975.h"
#include "drv_spl06.h"
#include "Drv_w25qxx.h"
#include "Drv_UP_Flow.h"

#include "My_FlightCtrl.h"
#include "Drv_adc.h"



u8 All_Init()
{
	NVIC_PriorityGroupConfig(NVIC_GROUP);		//中断优先级组别设置

	SysTick_Configuration(); 		//滴答时钟
	
	Delay_ms(100);					//延时
	Drv_LED_Init();					//LED功能初始化
	
	
	Drv_SPI1_init();          		//spi_2初始化，用于读取飞控板上所有传感器，都用SPI读取
	Flash_Init();             		//板载FLASH芯片驱动初始化
	
	Para_Data_Init();              		//参数数据初始化
	
//	Remote_Control_Init();
	Drv_SPI2_init();							//SPI2初始化
	/* 初始化NRF24L01 */
	
	

	Drv_Icm20602CSPin_Init(); 		//spi片选初始化
	Drv_AK8975CSPin_Init();   		//spi片选初始化

	Drv_SPL06CSPin_Init();    		//spi片选初始化
	
	sens_hd_check.gyro_ok = sens_hd_check.acc_ok = 
	Drv_Icm20602Reg_Init();   		//icm陀螺仪加速度计初始化，若初始化成功，则将陀螺仪和加速度的初始化成功标志位赋值
	sens_hd_check.mag_ok = 1;       //标记罗盘OK	
	sens_hd_check.baro_ok = Drv_Spl0601_Init();       		//气压计初始化
	
	PWM_Out_Init();					//初始化电调输出功能	
	Delay_ms(100);					//延时
	
	Usart1_Init(500000);			//串口1初始化，函数参数为波特率
	Delay_ms(10);					//延时	
	Usart2_Init(500000);					//串口2初始化，接光流 
	Delay_ms(10);					//延时

	Drv_OFInit();						//
	Delay_ms(100);				//延时

	Drv_AdcInit();
	Delay_ms(100);					//延时

	All_PID_Init();               		//PID初始化

	Delay_ms(50);					//延时		
	
	
	ANO_DT_SendString("SYS init OK!");
	return (1);
}
/******************* (C) COPYRIGHT 2014 My TECH *****END OF FILE************/
