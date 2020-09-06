/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WXY_CTRL_H
#define __WXY_CTRL_H
/* Includes ------------------------------------------------------------------*/
#include "My_FcData.h"
#include "My_Filter.h"
#include "My_Math.h"
#include "My_Pid.h"
/* Exported types ------------------------------------------------------------*/

typedef struct
{
	float exp[VEC_XYZ];
	s32 fb[VEC_XYZ];

	
	float out[VEC_XYZ];
}_loc_ctrl_st;// loc_ctrl;
extern _loc_ctrl_st loc_ctrl_1;
extern _loc_ctrl_st loc_ctrl_2;
/* Exported constants --------------------------------------------------------*/

extern _PID_arg_st loc_arg_1[] ; 
extern _PID_val_st loc_val_1[] ; 
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Loc_1level_PID_Init(void);
void Loc_1level_Ctrl(u16 dT_ms,s16 *CH_N);
#endif
