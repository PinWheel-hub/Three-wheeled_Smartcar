#include "headfile.h"
typedef struct
{
    uint8_t current;
    uint8_t next;
    uint8_t enter;
    void (*current_operation)(void);
} Menu_table;
Menu_table  table[56] =
{
    {0,6,1,(*fun0)},
    {1,3,2,(*fun1)},
    {2,3,2,(*fun2)},
    {3,5,4,(*fun3)},
    {4,5,4,(*fun4)},
    {5,1,0,(*fun5)},
		{6,12,7,(*fun6)},
    {7,9,8,(*fun7)},
    {8,9,8,(*fun8)},
    {9,11,10,(*fun9)},
    {10,11,10,(*fun10)},
    {11,7,6,(*fun11)},
		{12,0,13,(*fun12)},
    {13,15,14,(*fun13)},
    {14,15,14,(*fun14)},
    {15,17,16,(*fun15)},
    {16,17,16,(*fun16)},
    {17,19,18,(*fun17)},
		{18,19,18,(*fun18)},
    {19,21,20,(*fun19)},
    {20,21,20,(*fun20)},
    {21,13,12,(*fun21)}
};
uint8  func_index = 0;
void  menu_key(uint8 Get_key)
{
  switch(Get_key)
  {
    case 2:func_index=table[func_index].next;
		       ips114_clear(WHITE);
           break; 
    case 3:func_index=table[func_index].enter;
		       ips114_clear(WHITE);
           break;
    default:break;
  }
	if(Get_key||func_index==0)
    (table[func_index].current_operation)();
}

void fun0(void)
{
  ips114_showstr(0,0,"adjust speed <-");
	ips114_showstr(0,1,"adjust Threshold");
	ips114_showstr(0,2,"adjust direction_pid");
}
void fun1(void)
{
  ips114_showstr(0,0,"speed+100 <-");
	ips114_showstr(0,1,"speed-100");
	ips114_showstr(0,2,"back");
	ips114_showstr(0,7,"current_speed=");
	ips114_showint16(110,7,car_speed); 
}
void fun2(void)
{
	car_speed+=100;
  ips114_showstr(0,0,"speed+100 <-");
	ips114_showstr(0,1,"speed-100");
	ips114_showstr(0,2,"back");
	ips114_showstr(0,7,"current_speed=");
	ips114_showint16(110,7,car_speed);
}
void fun3(void)
{
  ips114_showstr(0,0,"speed+100");
	ips114_showstr(0,1,"speed-100 <-");
	ips114_showstr(0,2,"back");
	ips114_showstr(0,7,"current_speed=");
	ips114_showint16(110,7,car_speed);
}
void fun4(void)
{
	car_speed-=100;
  ips114_showstr(0,0,"speed+100");
	ips114_showstr(0,1,"speed-100 <-");
	ips114_showstr(0,2,"back");
	ips114_showstr(0,7,"current_speed=");
	ips114_showint16(110,7,car_speed);
}
void fun5(void)
{
  ips114_showstr(0,0,"speed+100");
	ips114_showstr(0,1,"speed-100");
	ips114_showstr(0,2,"back <-");
	ips114_showstr(0,7,"current_speed=");
	ips114_showint16(110,7,car_speed);
}
void fun6(void)
{
  ips114_showstr(0,0,"adjust speed");
	ips114_showstr(0,1,"adjust Threshold <-");
	ips114_showstr(0,2,"adjust direction_pid");
}
void fun7(void)
{
  ips114_showstr(0,0,"Threshold+5 <-");
	ips114_showstr(0,1,"Threshold-5");
	ips114_showstr(0,2,"back");
	ips114_showstr(0,7,"current_threshold=");
	ips114_showint16(150,7,Threshold); 
}
void fun8(void)
{
  Threshold+=5;
	ips114_showstr(0,0,"Threshold+5 <-");
	ips114_showstr(0,1,"Threshold-5");
	ips114_showstr(0,2,"back");
	ips114_showstr(0,7,"current_threshold=");
	ips114_showint16(150,7,Threshold); 
}
void fun9(void)
{
  ips114_showstr(0,0,"Threshold+5");
	ips114_showstr(0,1,"Threshold-5 <-");
	ips114_showstr(0,2,"back");
	ips114_showstr(0,7,"current_threshold=");
	ips114_showint16(150,7,Threshold); 
}
void fun10(void)
{
  Threshold-=5;
	ips114_showstr(0,0,"Threshold+5");
	ips114_showstr(0,1,"Threshold-5 <-");
	ips114_showstr(0,2,"back");
	ips114_showstr(0,7,"current_threshold=");
	ips114_showint16(150,7,Threshold); 
}
void fun11(void)
{
  ips114_showstr(0,0,"Threshold+5");
	ips114_showstr(0,1,"Threshold-5");
	ips114_showstr(0,2,"back <-");
	ips114_showstr(0,7,"current_threshold=");
	ips114_showint16(150,7,Threshold); 
}
void fun12(void)
{
  ips114_showstr(0,0,"adjust speed");
	ips114_showstr(0,1,"adjust Threshold");
	ips114_showstr(0,2,"adjust direction_pid <-");
}
void fun13(void)
{
  ips114_showstr(0,0,"kp+5000 <-");
	ips114_showstr(0,1,"kp-5000");
	ips114_showstr(0,2,"kd+50000");
	ips114_showstr(0,3,"kd-50000");
	ips114_showstr(0,4,"back");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}
void fun14(void)
{
  DifferentSpeed_PID.Kp+=5000;
	ips114_showstr(0,0,"kp+5000 <-");
	ips114_showstr(0,1,"kp-5000");
	ips114_showstr(0,2,"kd+50000");
	ips114_showstr(0,3,"kd-50000");
	ips114_showstr(0,4,"back");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}
void fun15(void)
{
  ips114_showstr(0,0,"kp+5000");
	ips114_showstr(0,1,"kp-5000 <-");
	ips114_showstr(0,2,"kd+50000");
	ips114_showstr(0,3,"kd-50000");
	ips114_showstr(0,4,"back");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}
void fun16(void)
{
  DifferentSpeed_PID.Kp-=5000;
	ips114_showstr(0,0,"kp+5000");
	ips114_showstr(0,1,"kp-5000 <-");
	ips114_showstr(0,2,"kd+50000");
	ips114_showstr(0,3,"kd-50000");
	ips114_showstr(0,4,"back");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}
void fun17(void)
{
  ips114_showstr(0,0,"kp+5000");
	ips114_showstr(0,1,"kp-5000");
	ips114_showstr(0,2,"kd+50000 <-");
	ips114_showstr(0,3,"kd-50000");
	ips114_showstr(0,4,"back");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}
void fun18(void)
{
  DifferentSpeed_PID.Kd+=50000;
	ips114_showstr(0,0,"kp+5000");
	ips114_showstr(0,1,"kp-5000");
	ips114_showstr(0,2,"kd+50000 <-");
	ips114_showstr(0,3,"kd-50000");
	ips114_showstr(0,4,"back");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}
void fun19(void)
{
  ips114_showstr(0,0,"kp+5000");
	ips114_showstr(0,1,"kp-5000");
	ips114_showstr(0,2,"kd+50000");
	ips114_showstr(0,3,"kd-50000 <-");
	ips114_showstr(0,4,"back");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}
void fun20(void)
{
	DifferentSpeed_PID.Kd-=50000;
	ips114_showstr(0,0,"kp+5000");
	ips114_showstr(0,1,"kp-5000");
	ips114_showstr(0,2,"kd+50000");
	ips114_showstr(0,3,"kd-50000 <-");
	ips114_showstr(0,4,"back");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}
void fun21(void)
{
	ips114_showstr(0,0,"kp+5000");
	ips114_showstr(0,1,"kp-5000");
	ips114_showstr(0,2,"kd+50000");
	ips114_showstr(0,3,"kd-50000");
	ips114_showstr(0,4,"back <-");
	ips114_showstr(0,6,"current_pd=");
	ips114_showint32(0,7,DifferentSpeed_PID.Kp,8);
	ips114_showint32(100,7,DifferentSpeed_PID.Kd,8);
}