#include "headfile.h"

// **************************** 宏定义 ****************************
#define SPLIT_ULTRASONIC_UART		UART_3
#define SPLIT_ULTRASONIC_BAUD		115200
#define SPLIT_ULTRASONIC_TX			UART3_TX_B10
#define SPLIT_ULTRASONIC_RX			UART3_RX_B11

#define SEND_PORT_PIN				B9												// 设置对应端口 有去模块发送 这里其实可以不用控制
// **************************** 宏定义 ****************************

// **************************** 变量定义 ****************************


void uart_handler (void);
void Ultrasonic_init();  //有来有去模块初始化
void Ultrasonic_send();  //有去模块发送
int Ultrasonic_receive(); //返回距离信息