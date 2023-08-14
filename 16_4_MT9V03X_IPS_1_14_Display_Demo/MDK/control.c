#include "headfile.h"
#include "shangweiji.h"
CAR_SPEED current_speed={0,0};
int car_speed=Default_Car_Speed;
//*****************************************************总控制******************************************************
float average_error=0;
int different=0;
int different1=0;
PID_TypeDef DifferentSpeed_PID={75000,0,800000,0,0,0,0};
//speed:3000, direction_pid:75000 0 800000,left_speed_pid:4000 500 0,right_speed_pid:3500 450 0
//speed:3500, direction_pid:105000 0 1100000,left_speed_pid:4000 500 0,right_speed_pid:3500 450 0
//摄像头转向环
void Turn_Control()
{ 
	 get_longest_col();
	 trackBorder_Get();
	 refresh_current_element();
	 average_error=centre_line_get(); 
	 if(ips_flag==1)
	 {
		 erzhihua();
		 Show_Line();
  	 ips114_displayimage032(image[0], MT9V03X_W, MT9V03X_H); 
	 }
}
   
			

//*****************************************************总控制******************************************************

//*****************************************************车速控制******************************************************
//总差速

void Different_Speed()
{
	different=PID_Location(0,average_error,&DifferentSpeed_PID);
}

//电机转速控制


PID_TypeDef Speed_Left_PID={4000,500,0,0,0,0,0};
PID_TypeDef Speed_Right_PID={3500,500,0,0,0,0,0};
int temp_length = 0;
int8 length_count = 0;

int round_length = 0;

CAR_SPEED speed_cricuit={0,0};
//*****************************************************车速控制******************************************************
void Speed_Control()
{
    CAR_SPEED target_speed;
    target_speed.right_speed=car_speed-different;
    target_speed.left_speed=car_speed+different;
		current_speed = Get_Speed(Speed_Circuit_T);      //获取速度
		if(stop_flag==2)
		{
			target_speed.left_speed=0;
		  target_speed.right_speed=0;		
		}
    speed_cricuit.left_speed=PID_Location(target_speed.left_speed,current_speed.left_speed,&Speed_Left_PID);
    speed_cricuit.right_speed=PID_Location(target_speed.right_speed,current_speed.right_speed,&Speed_Right_PID);
		//OutPut_Data(current_speed.left_speed,current_speed.right_speed,target_speed.left_speed,0);
    set_Speed(speed_cricuit);
}
//*****************************************************元素分别控制******************************************************


//*****************************************************元素分别控制******************************************************