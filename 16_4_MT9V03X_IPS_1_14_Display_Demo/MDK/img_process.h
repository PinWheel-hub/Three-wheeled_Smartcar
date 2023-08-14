#ifndef _img_process_h
#define _img_process_h
#include "SEEKFREE_MT9V03X.h"
#include "Ultrasonic.h"
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
图像未进行索引变换，即图像最上侧索引为0，最下侧为img_height-1
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#define img_width       MT9V03X_W
#define img_height      MT9V03X_H

//extern const uint8 track_width[img_height];
extern uint8 image[MT9V03X_H][MT9V03X_W];
extern uint8 stop_flag;
extern uint8 Threshold;
//*******************************************************标定*******************************************
void Camera_Calibration();
void Get_Track_Width();
//*******************************************************标定*******************************************

//*******************************************************绘制标识*******************************************
void Show_Line();
//*******************************************************绘制标识*******************************************

//***************************************************找边线****************************************************
extern int16 valid_row;						//有效边线范围（0:valid_row)
#define Valid_Row_Width         6			//有效行判定条件（赛道宽度）
void Get_Edge_Related();                                        //获取左右边界及中线
uint8 is_Track(uint8 Row_Index);								//判断边线范围内是否是赛道
//***************************************************找边线****************************************************

//***************************************************边线预处理****************************************************
typedef enum
{
	Right_Edge,
	Left_Edge
}EdgeSide;

void Smooth_Edge();
//***************************************************边线预处理****************************************************

//*************************************************my****************************************************
#define Default_Threshold 125  //二值化阈值
void threshold_init();
void weight_array_init();
void get_longest_col();
void get_shortest_col();
bool if_narrow_track();
void mid_point_filter();
void erzhihua();
void refresh_current_element();
uint8 find_Zebra();
int16 count_Zebra();
uint8 find_Zebra_y();
int16 count_Zebra_y();
uint8 find_Inflection_G(int16 start,int16 end);
void repair_Zebra();
void repair_car();
uint8 relay_stop();
uint8 relay_start();
void judge_relay();
void send_message(uint8 message);
uint8 receive_message();
void if_stop();
//*************************************************chen****************************************************
extern int16 Border[3][img_height];
#define LEFT 0
#define MID 1
#define RIGHT 2
#define Default_Straight_Judge 40

#define Garage_Out 0
#define Garage_In 1
typedef enum{
    Straight=0,
    Corner,
    Crossing,
    Ring,
    Branch,
    Ramp,
    Garage,
    Stop
}track_Type_Enum;
extern track_Type_Enum current_type_element;
	
void trackBorder_Get();
void specialElem_Judge();
int centre_line_get();
uint8 OTSU(uint8 *pre_image);
uint8 judge_Ring();
void repair_Ring();
uint8 is_Branch();
uint8 judge_Branch();
void repair_Branch();
uint8 judge_Garage();
void repair_Garage();
float cal_Curvature();
uint8 find_Inflection_A(int16 start,int16 end,int dir,int sign);
uint8 find_Inflection_B(int16 start,int16 end,int dir);
uint8 find_Inflection_C(int16 start,int16 end,int dir);
uint8 is_Straight(int16 start_index,int16 end_index,uint8 side);

#endif
