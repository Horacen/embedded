

#include "My_RC.h"
#include "Drv_nrf.h"
#include "My_FcData.h"
#include "include.h"
#include "My_Math.h"
#include "Drv_icm20602.h"
#include "My_MagProcess.h"
#include "Drv_led.h"


u8 chn_en_bit=0;

s16 CH_N[8]={0};

__remoter_cmd remoter; //

static u16 cwd_cnt[10] ;
void ch_watch_dog_feed(u8 ch_n)
{
	ch_n = LIMIT(ch_n,0,7);
	cwd_cnt[ch_n] = 0;
}
/**
	* remoter data analysis
  */
void remoter_analysis(void)
{
	u8 i, sum;
	for(i=0; i<12; i++)
		sum += remote_data[i];
	
	if(sum == remote_data[10])
	{
		remoter.angle_pitch = (char)remote_data[1];
		remoter.angle_rol   = (char)remote_data[2];
		remoter.speed_yaw   = (int)((remote_data[3]<<8) | remote_data[4]);
		remoter.speed_hight = (char)(remote_data[5]);
		remoter.unlock      = (u8)(remote_data[6]);
		remoter.mode        = (u8)(remote_data[7]);
		remoter.auto_takeoff= (u8)(remote_data[8]);
		remoter.auto_land   = (u8)(remote_data[9]);
		
		for(i=0; i<8; i++)
		{
			ch_watch_dog_feed(i);
		}
	}	
}


void ch_watch_dog(u8 dT_ms)
{
	for(u8 i = 0;i<8;i++)
	{
		if(cwd_cnt[i]<500)
		{
			cwd_cnt[i] += dT_ms;
			chn_en_bit |= 0x01<<i;
		}
		else
		{
			chn_en_bit &= ~(0x01<<i);
		}
	}
}



void unlock(u8 dT_ms)
{
	
	if( flag.power_state <=2 && para_sta.save_trig == 0)
	{
		if(sens_hd_check.acc_ok && sens_hd_check.gyro_ok)
		{
			if(sens_hd_check.baro_ok)
			{
				if(flag.sensor_imu_ok  )
				{
					flag.unlock_err = 0;	

				}
				else
				{
					flag.unlock_err = 1;

				}
			}
			else
			{
				LED_STA.errBaro = 1;
				flag.unlock_err = 2;
			}
		}
		else
		{
			LED_STA.errMpu = 1;
			flag.unlock_err = 3;
		}
	}
	else
	{
		flag.unlock_err = 4;
	}
	
	if(flag.unlock_sta == 0)
	{
		if(flag.unlock_cmd != 0)
		{		
			if(flag.unlock_err == 0)
			{
				//
				flag.unlock_sta = flag.unlock_cmd;
	
				ANO_DT_SendString("Unlock OK!");
				
			}
			else 
			{
				//reset
				flag.unlock_cmd = 0;
				//
				if(flag.unlock_err == 1)
				{
					ANO_DT_SendString("Unlock Fail!");
				}
				else if(flag.unlock_err == 2)
				{
					ANO_DT_SendString("Unlock Fail!");
				}
				else if(flag.unlock_err == 3)
				{
					ANO_DT_SendString("Unlock Fail!");
				}
				else if(flag.unlock_err == 4)
				{
					ANO_DT_SendString("Power Low,Unlock Fail!");
				}
				else
				{
				
				}
			}
		}
		else
		{
		
		}
	}
	else
	{
		if(flag.unlock_cmd == 0)
		{
			ANO_DT_SendString(" FC Output Locked! ");
		}		
		flag.unlock_sta = flag.unlock_cmd;
	}
	
	////////////////////////////////////////////
	static uint16_t	cnt_unlock;
	if(remoter.unlock)
	{
		cnt_unlock += dT_ms;
		
		if(cnt_unlock > 1500)		//press key 1.5s, unlock 
		{
			cnt_unlock=0;
			flag.unlock_cmd = ~flag.unlock_cmd;
		}
	}		
	else
			cnt_unlock = 0;
		
		flag.thr_low = 0;
}

void RC_duty_task(u8 dT_ms) //
{
	if(flag.start_ok)	
	{
		unlock(dT_ms);

		ch_watch_dog(dT_ms);
	}
}






