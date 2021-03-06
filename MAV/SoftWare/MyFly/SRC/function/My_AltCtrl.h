/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WZ_CTRL_H
#define __WZ_CTRL_H
/* Includes ------------------------------------------------------------------*/
#include "My_FcData.h"
#include "My_Filter.h"
#include "My_Math.h"
#include "My_Pid.h"
/* Exported types ------------------------------------------------------------*/



/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Alt_1level_Ctrl(float dT_s);
void Alt_1level_PID_Init(void);

void Alt_2level_PID_Init(void);
void Alt_2level_Ctrl(float dT_s);

void Auto_Take_Off_Land_Task(u8 dT_ms);
#endif
