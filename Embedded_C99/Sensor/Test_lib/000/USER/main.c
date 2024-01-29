#include "stm32f10x.h"
#include "LED.h"
#include "delay.h"


/************************************************
www.stm32vn.com
************************************************/


 
 int main(void)
 {	
	 LED_init();
	 delay_init();
 
  while(1)
	{
	    LED0 =0;//GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	    GPIO_SetBits(GPIOE,GPIO_Pin_5);
		delay_ms(300);
			LED0 =1;GPIO_SetBits(GPIOB,GPIO_Pin_5);
			GPIO_ResetBits(GPIOE,GPIO_Pin_5);
		delay_ms(300);
	}
 }
