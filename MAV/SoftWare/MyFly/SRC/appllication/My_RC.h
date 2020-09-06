#ifndef __MY_RC_H
#define __MY_RC_H

#include "stm32f10x.h"


typedef struct
{
	char angle_pitch;
	char angle_rol;
	int  speed_yaw;
	char speed_hight;
	u8   unlock;
	u8   mode;
	u8   auto_takeoff;
	u8   auto_land;
}__remoter_cmd;
extern __remoter_cmd remoter; 


extern u8 chn_en_bit;

extern s16 CH_N[8];


extern void remoter_analysis(void);

#endif /* __MY_RC_H */

