#include "headfile.h"
//*****************************************Motor*****************************************
#define MAX_DUTY			75
#define DIR_L				A0
#define DIR_R				A2
#define PWM_TIM				TIM_5
#define PWM_L				TIM_5_CH2_A01
#define PWM_R				TIM_5_CH4_A03
#define Motor_Speed_Limit_Factor        0.70f            //电机限幅系数


typedef enum
{
  Motor_Right = 0,
  Motor_Left,
  Motor_Both
} MOTOR_SIDE;

typedef struct
{
  int left_speed;
  int right_speed;
}CAR_SPEED;

void motor_init(void);
void set_Speed(CAR_SPEED);
//*****************************************Motor*****************************************

//****************************Encoder**********************************
// **************************** 宏定义 ****************************
#define ENCODER1_TIM		TIM_3
#define ENCODER1_A			TIM_3_ENC1_B04
#define ENCODER1_B			TIM_3_ENC2_B05

#define ENCODER2_TIM		TIM_4
#define ENCODER2_A			TIM_4_ENC1_B06
#define ENCODER2_B			TIM_4_ENC2_B07
// **************************** 宏定义 ****************************

void Encoder_init(void);
CAR_SPEED Encoder_Get_Value();
void Encoder_Clean();
//****************************Encoder**********************************


//*****************************************************Car_Speed******************************************************
//#define Wheel_Circumference     210     //轮周uint:mm
//#define Pulse_Per_Round         4096    //编码器一圈脉冲数
#define Pulse 23750                     //脉冲数
#define Distance 1350//运动距离 uint:mm
#define Speed_Circuit_T         5      //uint:ms
CAR_SPEED Get_Speed(int T);
//*****************************************************Car_Speed******************************************************

//*****************************************pit_init*****************************************
void pit_init(void);
//*****************************************pit_init*****************************************