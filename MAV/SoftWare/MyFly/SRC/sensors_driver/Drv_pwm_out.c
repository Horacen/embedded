/******************** (C) COPYRIGHT 2017 My Tech ********************************
 * 作者    ：匿名科创
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
 * 描述    ：PWM输出
**********************************************************************************/
#include "Drv_pwm_out.h"
#include "include.h"
#include "My_Math.h"

//21分频到 84000000/21 = 4M   0.25us

/*初始化高电平时间1000us（4000份）*/
#define INIT_DUTY 4000 //u16(1000/0.25)
/*频率400hz*/
#define HZ        400
/*精度10000，每份0.25us*/
#define ACCURACY 10000 //u16(2500/0.25) //accuracy	// 32 000 000 / 4 000 000
/*设置飞控控制信号转换比例为4*/
#define PWM_RADIO 4//(8000 - 4000)/1000.0

u8 PWM_Out_Init () //400hz
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    uint16_t PrescalerValue = 0;
    u32 hz_set = ACCURACY * HZ;

	GPIO_StructInit(&GPIO_InitStructure);
    TIM_TimeBaseStructInit ( &TIM_TimeBaseStructure );
    TIM_OCStructInit ( &TIM_OCInitStructure );

    hz_set = LIMIT ( hz_set, 1, 32000000 );

    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_TIM4, ENABLE );
    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOB, ENABLE );

/////////////////////////////////////////////////////////////////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; //GPIO_Pin_0 | GPIO_Pin_1 |
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );

    /* Compute the prescaler value */
    PrescalerValue = ( uint16_t ) ( ( SystemCoreClock / 2 ) / hz_set ) - 1;
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = ACCURACY;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit ( TIM4, &TIM_TimeBaseStructure );


    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = INIT_DUTY;
    TIM_OC3Init ( TIM4, &TIM_OCInitStructure );
    TIM_OC3PreloadConfig ( TIM4, TIM_OCPreload_Enable );

    /* PWM1 Mode configuration: Channel4 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = INIT_DUTY;
    TIM_OC4Init ( TIM4, &TIM_OCInitStructure );
    TIM_OC4PreloadConfig ( TIM4, TIM_OCPreload_Enable );

    TIM_ARRPreloadConfig ( TIM4, ENABLE );
    TIM_Cmd ( TIM4, ENABLE );
/////////////////////////////////////////////////////////////////////////////

    if ( hz_set > 32000000 )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


void SetPwm ( int16_t pwm[MOTORSNUM] )
{


    TIM1->CCR4 = PWM_RADIO * ( pwm[0] ) + INIT_DUTY;				//1
    TIM1->CCR3 = PWM_RADIO * ( pwm[1] ) + INIT_DUTY;				//2
    TIM1->CCR2 = PWM_RADIO * ( pwm[2] ) + INIT_DUTY;				//3
    TIM1->CCR1 = PWM_RADIO * ( pwm[3] ) + INIT_DUTY;				//4

// 	TIM5->CCR4 = PWM_RADIO *( pwm_tem[CH_out_Mapping[4]] ) + INIT_DUTY;				//5
// 	TIM5->CCR3 = PWM_RADIO *( pwm_tem[CH_out_Mapping[5]] ) + INIT_DUTY;				//6
// 	TIM8->CCR4 = PWM_RADIO *( pwm_tem[CH_out_Mapping[6]] ) + INIT_DUTY;				//7
// 	TIM8->CCR3 = PWM_RADIO *( pwm_tem[CH_out_Mapping[7]] ) + INIT_DUTY;				//8

}


/******************* (C) COPYRIGHT 2014 My TECH *****END OF FILE************/
