#include "headfile.h"
void key_init(void)
{
	gpio_init(KEY_1, GPI, GPIO_HIGH, GPI_PULL_UP);	
	gpio_init(KEY_2, GPI, GPIO_HIGH, GPI_PULL_UP);
	gpio_init(KEY_3, GPI, GPIO_HIGH, GPI_PULL_UP);
}

uint8 KEY_Read(uint8 mode)
{
    if(mode && (gpio_get(KEY_1)==0) || gpio_get(KEY_2)==0 || gpio_get(KEY_3)==0)
    {
      systick_delay_ms(200);
      if(gpio_get(KEY_1)==0) 
      {  
				return 1;    
      }
      
      else if(gpio_get(KEY_2)==0) 
      {  
				return 2;    
      }
      else if(gpio_get(KEY_3)==0) 
      {  
				return 3;    
      }
    }
    return 0;  
}
void key_ips(void)
{
	  uint8 key_get;
	  key_get=KEY_Read(1);
		if(key_get)
		{
			if(key_get==1)
			{
        ips_flag=(ips_flag+1)%3;
				ips114_clear(WHITE);
			}				
		}	
		if(ips_flag==2)
      menu_key(key_get);
}