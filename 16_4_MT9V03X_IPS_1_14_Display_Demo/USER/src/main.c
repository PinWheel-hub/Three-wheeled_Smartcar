/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				main
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32F3277
* @Taobao			https://seekfree.taobao.com/
* @date				2021-02-22
********************************************************************************************************************/

#include "headfile.h"
// *************************** 例程说明 ***************************
// 
// 测试需要准备逐飞科技 MM32F3277 核心板一块
// 
// 调试下载需要准备逐飞科技 CMSIS-DAP 调试下载器 或 ARM 调试下载器 一个
// 
// 本例程初始化了 主板上的摄像头接口 以及 IPS1.14寸屏幕模块
// 
// 烧录本例程后 请断电接线
// 需要安装 总钻风灰度数字摄像头 UART版本 模块 与 IPS1.14寸屏幕模块
// 确认连接无误 上电 请务必使用电池供电 并确保电池电量充足
// 此时 IPS1.14寸屏幕模块 上显示图像信息
// 
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完
// 
// *************************** 例程说明 ***************************

// **************************** 宏定义 ****************************

// **************************** 宏定义 ****************************

// **************************** 变量定义 ****************************

// **************************** 变量定义 ****************************

// **************************** 代码区域 ****************************
int main(void)
{
	mt9v03x_init();
  Encoder_init();
	motor_init();
	seekfree_wireless_init();
	pit_init();																// 初始化模块
	ips114_init();
	threshold_init();
	weight_array_init();
	key_init();
	Ultrasonic_init();
//	gpio_init(BUZZER_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
	while(1)
	{
		key_ips();
		judge_relay();
		if(mt9v03x_finish_flag == 1)
		{
		  Turn_Control(); 		
			mt9v03x_finish_flag = 0;
		}
		if_stop();
	}
}
// **************************** 代码区域 ****************************
