#include "stm32f10x.h"

void GPIO_Config(void);
void Delay(__IO uint32_t nCount);

int main(void)
{
	GPIO_Config();
	while(1)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
		Delay(1000000);
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
		Delay(10000000);
	}

}

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitSturcture;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitSturcture);
}	
void Delay(__IO uint32_t nCount)
{
	while(nCount--)
	{
	}
}

