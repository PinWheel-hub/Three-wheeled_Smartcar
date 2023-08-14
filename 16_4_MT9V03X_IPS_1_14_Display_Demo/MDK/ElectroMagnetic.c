#include "headfile.h"
float mySqrt(float x)
{
    float a = x;
    unsigned int i = *(unsigned int *)&x;
    i = (i + 0x3f76cf62) >> 1;
    x = *(float *)&i;
    x = (x + a / x) * 0.5;
    return x;
}
void ElectroMagnetic_init()
{
    adc_init(ADC_1, ADC_CH1, ADC_10BIT);											// 初始化 ADC 已经对应通道引脚 10位精度
	  adc_init(ADC_1, ADC_CH2, ADC_10BIT);											// 初始化 ADC 已经对应通道引脚 10位精度
	  adc_init(ADC_1, ADC_CH3, ADC_10BIT);											// 初始化 ADC 已经对应通道引脚 10位精度
	  adc_init(ADC_1, ADC_CH4, ADC_10BIT);											// 初始化 ADC 已经对应通道引脚 10位精度
}
int right_value1,left_value1,right_value2,left_value2;
float error=0;
float error1=0;
float pre_error=0;
float pre_error1=0;

float ElectroMagnetic_GetValue()
{
	  right_value1=0,left_value1=0;
    right_value2=0,left_value2=0;
    for(int i=0;i<10;i++)
    {
        left_value1+=adc_convert(ADC_1, ADC_CH1);
        right_value1+=adc_convert(ADC_1, ADC_CH2);
        left_value2+=adc_convert(ADC_1, ADC_CH3);
        right_value2+=adc_convert(ADC_1, ADC_CH4);
    }
    right_value1=right_value1/10;
    left_value1=left_value1/10;
    right_value2=right_value2/10;
    left_value2=left_value2/10;

    if(right_value1+left_value1>10){
    	error=400*(right_value1-left_value1)/(right_value1+left_value1);
        error=error*(myabs(error))/400;
    }
    pre_error=0.16*pre_error+0.16*error;
    if((right_value1+left_value1)>30)
        error1=400*(right_value2-left_value2)/(right_value1+left_value1-right_value2+left_value2);
    else
        error1=400*(right_value2-left_value2)/myabs(50-right_value2+left_value2);
    pre_error1=0.3*pre_error1+0.25*error1;

    oled_putpixel(0, 2, pre_error+pre_error1);
    oled_putpixel(0, 5, pre_error);oled_putpixel(64, 5, pre_error1);
    oled_putpixel(0,6,left_value1);oled_putpixel(64,6,right_value1);
    oled_putpixel(0,7,left_value2);oled_putpixel(64,7,right_value2);
    return pre_error+pre_error1;
}
