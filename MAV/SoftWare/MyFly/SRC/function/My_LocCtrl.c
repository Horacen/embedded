#include "My_LocCtrl.h"
#include "My_Imu.h"
#include "My_FlightCtrl.h"
#include "My_OF.h"
#include "My_Parameter.h"


//λ���ٶȻ����Ʋ���
_PID_arg_st loc_arg_1[2] ; 

//λ���ٶȻ���������
_PID_val_st loc_val_1[2] ; 

//λ���ٶȻ��������Ʋ���
_PID_arg_st loc_arg_1_fix[2] ; 

//λ���ٶȻ�������������
_PID_val_st loc_val_1_fix[2] ; 

static u8 mode_f[2];

/*�ǶȻ�PID������ʼ��*/
void Loc_1level_PID_Init()
{
	//GPS
	if(mode_f[1] == 2)
	{
		//normal
		loc_arg_1[X].kp = My_Parame.set.pid_gps_loc_1level[KP];//0.22f  ;
		loc_arg_1[X].ki = 0  ;
		loc_arg_1[X].kd_ex = 0.00f ;
		loc_arg_1[X].kd_fb = My_Parame.set.pid_gps_loc_1level[KD];
		loc_arg_1[X].k_ff = 0.02f;
		
		loc_arg_1[Y] = loc_arg_1[X];
		//fix	
		loc_arg_1_fix[X].kp = 0.0f  ;
		loc_arg_1_fix[X].ki = My_Parame.set.pid_loc_1level[KI] ;
		loc_arg_1_fix[X].kd_ex = 0.00f;
		loc_arg_1_fix[X].kd_fb = 0.00f;
		loc_arg_1_fix[X].k_ff = 0.0f;
		
		loc_arg_1_fix[Y] = loc_arg_1_fix[X];	
	}
	//OF
	else if(mode_f[1] == 1)
	{
		//normal
		loc_arg_1[X].kp = My_Parame.set.pid_loc_1level[KP];//0.22f  ;
		loc_arg_1[X].ki = 0.0f  ;
		loc_arg_1[X].kd_ex = 0.00f ;
		loc_arg_1[X].kd_fb = My_Parame.set.pid_loc_1level[KD];
		loc_arg_1[X].k_ff = 0.02f;
		
		loc_arg_1[Y] = loc_arg_1[X];
		//fix	
		loc_arg_1_fix[X].kp = 0.0f  ;
		loc_arg_1_fix[X].ki = My_Parame.set.pid_loc_1level[KI] ;
		loc_arg_1_fix[X].kd_ex = 0.00f;
		loc_arg_1_fix[X].kd_fb = 0.00f;
		loc_arg_1_fix[X].k_ff = 0.0f;
		
		loc_arg_1_fix[Y] = loc_arg_1_fix[X];	
	}
	else
	{
		//null
	}
	

}

_loc_ctrl_st loc_ctrl_1;
static float fb_speed_fix[2];

float vel_fb_d_lpf[2];
/*λ���ٶȻ�*/
void Loc_1level_Ctrl(u16 dT_ms,s16 *CH_N)
{
	
	if(switchs.of_flow_on && (!switchs.gps_on))
	{
		mode_f[1] = 1;
		if(mode_f[1] != mode_f[0])
		{
			Loc_1level_PID_Init();
			mode_f[0] = mode_f[1];
		}
		////
		loc_ctrl_1.exp[X] = fs.speed_set_h[X];
		loc_ctrl_1.exp[Y] = fs.speed_set_h[Y];
		//
		LPF_1_(5.0f,dT_ms*1e-3f,imu_data.h_acc[X],vel_fb_d_lpf[X]);
		LPF_1_(5.0f,dT_ms*1e-3f,imu_data.h_acc[Y],vel_fb_d_lpf[Y]);		
		
		loc_ctrl_1.fb[X] = of_rdf.gnd_vel_est_h[X] + 0.03f *vel_fb_d_lpf[X];
		loc_ctrl_1.fb[Y] = of_rdf.gnd_vel_est_h[Y] + 0.03f *vel_fb_d_lpf[Y];
		
		fb_speed_fix[0] = of_rdf.gnd_vel_est_h[X];
		fb_speed_fix[1] = of_rdf.gnd_vel_est_h[Y];		
		
		for(u8 i =0;i<2;i++)
		{
			PID_calculate( dT_ms*1e-3f,            //���ڣ���λ���룩
										loc_ctrl_1.exp[i] ,				//ǰ��ֵ
										loc_ctrl_1.exp[i] ,				//����ֵ���趨ֵ��
										loc_ctrl_1.fb[i] ,			//����ֵ����
										&loc_arg_1[i], //PID�����ṹ��
										&loc_val_1[i],	//PID���ݽṹ��
										50,//��������޷�
										10 *flag.taking_off			//integration limit�������޷�
										 )	;	
			
			//fix
			PID_calculate( dT_ms*1e-3f,            //���ڣ���λ���룩
										loc_ctrl_1.exp[i] ,				//ǰ��ֵ
										loc_ctrl_1.exp[i] ,				//����ֵ���趨ֵ��
										fb_speed_fix[i] ,			//����ֵ����
										&loc_arg_1_fix[i], //PID�����ṹ��
										&loc_val_1_fix[i],	//PID���ݽṹ��
										50,//��������޷�
										10 *flag.taking_off			//integration limit�������޷�
										 )	;	
			
			loc_ctrl_1.out[i] = loc_val_1[i].out + loc_val_1_fix[i].out;	//(PD)+(I)	
		}		
	}
	else
	{
		mode_f[1] = 3;
		if(mode_f[1] != mode_f[0])
		{
			Loc_1level_PID_Init();
			mode_f[0] = mode_f[1];
		}
		////
		loc_ctrl_1.out[X] = (float)MAX_ANGLE/MAX_SPEED *fs.speed_set_h[X] ;
		loc_ctrl_1.out[Y] = (float)MAX_ANGLE/MAX_SPEED *fs.speed_set_h[Y] ;
	}
}

_loc_ctrl_st loc_ctrl_2;

