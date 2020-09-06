#ifndef _PWM_OUT_H_
#define _PWM_OUT_H_

#include "stm32f10x.h"

u8 PWM_Out_Init(void);
void SetPwm(int16_t pwm[]);

#endif

