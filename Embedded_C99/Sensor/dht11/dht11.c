#include "dht11.h"


//************************** Low Level Layer ********************************************************//
void DELAY_TIM_Init(TIM_HandleTypeDef *htim)
{
	HAL_TIM_Base_Start(htim);
}

void DELAY_TIM_Us(TIM_HandleTypeDef *htim, uint16_t time)
{
	__HAL_TIM_SET_COUNTER(htim,0);
	while(__HAL_TIM_GET_COUNTER(htim)<time){}
}

static void DHT_DelayInit(DHT_Init_t* DHT)
{
	DELAY_TIM_Init(DHT->Timer);
}
static void DHT_DelayUs(DHT_Init_t* DHT, uint16_t Time)
{
	DELAY_TIM_Us(DHT->Timer, Time);
}

static void DHT_SetPinOut(DHT_Init_t* DHT)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT->Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT->PORT, &GPIO_InitStruct);
}
static void DHT_SetPinIn(DHT_Init_t* DHT)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT->Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT->PORT, &GPIO_InitStruct);
}
static void DHT_WritePin(DHT_Init_t* DHT, uint8_t Value)
{
	HAL_GPIO_WritePin(DHT->PORT, DHT->Pin, Value);
}
static uint8_t DHT_ReadPin(DHT_Init_t* DHT)
{
	return HAL_GPIO_ReadPin(DHT->PORT, DHT->Pin);
}
//********************************* Middle level Layer ****************************************************//
static uint8_t DHT_Start(DHT_Init_t* DHT)
{
	
	uint8_t Response = 0;
	DHT_SetPinOut(DHT);  
	DHT_WritePin(DHT, 0);
	DHT_DelayUs(DHT, 20000);  
	DHT_WritePin(DHT, 1);	
	DHT_DelayUs(DHT, 30);
	DHT_SetPinIn(DHT);    
	DHT_DelayUs(DHT, 40); 
	if (!DHT_ReadPin(DHT))
	{
		DHT_DelayUs(DHT, 80); 
		if(DHT_ReadPin(DHT))
		{
			Response = 1;   
		}
		else Response = 0;  
	}		
	while(DHT_ReadPin(DHT));

	return Response;
}
static uint8_t DHT_Read(DHT_Init_t* DHT)
{
	uint8_t Value = 0;
	DHT_SetPinIn(DHT);
	for(int i = 0; i<8; i++)
	{
		while(!DHT_ReadPin(DHT));
		DHT_DelayUs(DHT, 40);
		if(!DHT_ReadPin(DHT))
		{
			Value &= ~(1<<(7-i));	
		}
		else Value |= 1<<(7-i);
		while(DHT_ReadPin(DHT));
	}
	return Value;
}

//************************** High Level Layer ********************************************************//
void DHT_Init(DHT_Init_t* DHT, uint8_t DHT_Type, TIM_HandleTypeDef* Timer, GPIO_TypeDef* DH_PORT, uint16_t DH_Pin)
{
	if(DHT_Type == DHT11)
	{
		DHT->Type = DHT11_STARTTIME;
	}
	else if(DHT_Type == DHT22)
	{
		DHT->Type = DHT22_STARTTIME;
	}
	DHT->PORT = DH_PORT;
	DHT->Pin = DH_Pin;
	DHT->Timer = Timer;
	DHT_DelayInit(DHT);
}

uint8_t DHT_ReadTempHum(DHT_Init_t* DHT)
{
	
	uint8_t Temp1, Temp2, RH1, RH2,SUM = 0;
	DHT_Start(DHT);
	RH1 = DHT_Read(DHT);
	RH2 = DHT_Read(DHT);
	Temp1 = DHT_Read(DHT);
	Temp2 = DHT_Read(DHT);
	SUM = DHT_Read(DHT);
	if(SUM == RH1 + RH2 +Temp1+Temp2 )
	{
		DHT->Humi= RH1*100 + RH2;
		DHT->Temp = Temp1*100 + Temp2;
	}
	return (uint8_t)SUM;
}