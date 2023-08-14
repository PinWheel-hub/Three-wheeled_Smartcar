#include "headfile.h"
#define KEY_1				D0														// 定义主板上按键对应引脚
#define KEY_2				D1														// 定义主板上按键对应引脚
#define KEY_3				D2														// 定义主板上按键对应引脚
#define KEY_4				D3														// 定义主板上按键对应引脚
void key_init(void);
uint8 KEY_Read(uint8);
void key_ips(void);