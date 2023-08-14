#include "Ultrasonic.h"
// **************************** 变量定义 ****************************
uint16 ranging_counter = 0;															// 测距数据
uint16 ranging_counter_return = 0;															// 测距数据
uint8 ranging_flage = 0x00;															// 测距状态
uint8 uart_data;
uint8 dat[3];
uint8 num;
// **************************** 代码区域 ****************************


void uart_handler (void)
{
	uart_query(SPLIT_ULTRASONIC_UART, &uart_data);
	dat[num] = uart_data;
	if(dat[0] != 0xa5)	num = 0;													//检查第一个数据是否为0xa5
	else				num++;

	if(num == 3)																	//接收完成 开始处理数据
	{
		num = 0;
		ranging_counter = dat[1]<<8 | dat[2];										//得到超声波模块测出的距离
		ranging_flage = 0x01;
	}
}

void Ultrasonic_init()  //有来有去模块初始化
{
	gpio_init(SEND_PORT_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);							// 初始化有去模块使能引脚
	uart_init(SPLIT_ULTRASONIC_UART, SPLIT_ULTRASONIC_BAUD, SPLIT_ULTRASONIC_TX, SPLIT_ULTRASONIC_RX);
	uart_rx_irq(SPLIT_ULTRASONIC_UART, 2);
}

void Ultrasonic_send()  //有去模块发送
{
	gpio_set(SEND_PORT_PIN, GPIO_HIGH);												// 拉高使能 有去模块发送 实际接上电上拉就开始发送 程序可以不用控制
}

int Ultrasonic_receive() //返回距离信息
{
	if(ranging_flage)
	{
		ranging_flage=0x00;
		ranging_counter_return=ranging_counter;
	}
	return ranging_counter_return;
}
	