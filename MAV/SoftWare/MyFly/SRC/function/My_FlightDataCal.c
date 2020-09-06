#include "My_FlightDataCal.h"
#include "My_Imu.h"
#include "Drv_icm20602.h"
#include "My_MagProcess.h"
#include "Drv_spl06.h"
#include "Drv_ak8975.h"
#include "My_MotionCal.h"
#include "Drv_led.h"
#include "My_OF.h"

u16 test_time_cnt;
void Fc_Sensor_Get()//1ms
{
	static u8 cnt;
	if(flag.start_ok)
	{
		/*��ȡ�����Ǽ��ٶȼ�����*/
		Drv_Icm20602_Read();
		
		cnt ++;
		cnt %= 20;
		if(cnt==0)
		{
			/*��ȡ�������̴���������*/
			Drv_AK8975_Read();
			/*��ȡ��ѹ������*/
			baro_height = (s32)Drv_Spl0601_Read();
		}
	}	
	test_time_cnt++;

}

extern s32 sensor_val_ref[];

static u8 reset_imu_f;
void IMU_Update_Task(u8 dT_ms)
{


	
////////////////////////////////////////////////////////////////////////		
			/*���׼�����У���λ������λ��Ǻʹ����Ƹ�λ���*/
				if(flag.unlock_sta )
				{
					imu_state.G_reset = imu_state.M_reset = 0;
					reset_imu_f = 0;
				}
				else 
				{
					if(flag.motionless == 0)
					{
//						imu_state.G_reset = 1;//�Զ���λ
						//sensor.gyr_CALIBRATE = 2;
					}	
					
					if(reset_imu_f==0 )//&& flag.motionless == 1)
					{
						imu_state.G_reset = 1;//�Զ���λ	
						sensor.gyr_CALIBRATE = 2;//У׼�����ǣ�������
						reset_imu_f = 1;     //�Ѿ���λ��λ���
					}
								
				}
									
				if(0) 
				{
					imu_state.gkp = 0.0f;
					imu_state.gki = 0.0f;
					
				}
				else
				{
					if(0)
					{
						imu_state.gkp = 0.2f;
					}
					else
					{
						/*�������������ں�����kpϵ��*/
						imu_state.gkp = 0.2f;//0.4f;
					}
					
					/*�������������ں�����kiϵ��*/
					imu_state.gki = 0.01f;
					
					/*�������̻����ں�����kiϵ��*/
					imu_state.mkp = 0.1f;
				}
				
				imu_state.M_fix_en = sens_hd_check.mag_ok;		//����������ʹ��
				
//				imu_state.obs_en = 1;
//				imu_data.obs_acc_w[X] = flow_decoupling.w_ref_acc[X];
//				imu_data.obs_acc_w[Y] = flow_decoupling.w_ref_acc[Y];
				
				/*��̬���㣬���£��ں�*/
				IMU_update(dT_ms *1e-3f, &imu_state,sensor.Gyro_rad, sensor.Acc_cmss, mag.val,&imu_data);//x3_dT_1[2] * 0.000001f
//////////////////////////////////////////////////////////////////////	
}

static s16 mag_val[3];
void Mag_Update_Task(u8 dT_ms)
{

	Mag_Get(mag_val);
	
	Mag_Data_Deal_Task(dT_ms,mag_val,imu_data.z_vec[Z],sensor.Gyro_deg[X],sensor.Gyro_deg[Z]);
	
}


s32 baro_height,baro_h_offset,ref_height_get_1,ref_height_get_2,ref_height_used;
s32 baro2tof_offset,tof2baro_offset;

float baro_fix1,baro_fix2,baro_fix;

static u8 wcz_f_pause;
float wcz_acc_use;			

void WCZ_Acc_Get_Task()//��С����
{
	wcz_acc_use += 0.1f *(imu_data.w_acc[Z] - wcz_acc_use);
}

//void Baro_Get_Task()
//{
////			ref_height_get += LIMIT((s32)user_spl0601_get() - ref_height_get,-20,20 );
////	baro_height =(s32)user_spl0601_get();
//}

u16 ref_tof_height;
static u8 baro_offset_ok,tof_offset_ok;
/*
		==========��ʹ����ѹ����Ϊ�߶�������Դ================
	============���߲�����Ҫ���㵱ǰ�ĸ߶Ⱥ͸߶�����============
							�߶Ȼ��ķ�����			wcz_hei_fus.out;
							�߶��ٶȻ��ķ����� 	wcz_spe_fus.out
*/
void WCZ_Fus_Task(u8 dT_ms)
{

	if(sens_hd_check.of_ok && baro_offset_ok) //TOF����OFӲ������������ѹ�Ƽ�¼���ֵ�Ժ�
	{					
		ref_height_used = ref_height_get_1 ;
	}
	else
	{
		ref_height_used = ref_height_get_1;
	}
	
	WCZ_Data_Calc(dT_ms,wcz_f_pause,(s32)wcz_acc_use,(s32)(ref_height_used));

}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////



