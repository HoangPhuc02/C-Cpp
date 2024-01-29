#include "stm32f10x.h"


static GPIO_InitTypeDef gpio_init;

void portc_init(void);
void delay(int time);

int main(void)
{
	portc_init();
	while(1)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
		delay(1000000);
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
		delay(1000000);
		
	}
}

void gpioc_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	gpio_init.GPIO_Pin = GPIO_Pin_13;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &gpio_init);
}
void delay(int time)
{
	while(time)
		time--;
}

