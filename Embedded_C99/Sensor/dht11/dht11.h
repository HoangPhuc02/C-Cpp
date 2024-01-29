#ifndef __DHT11_H
#define __DHT11_H 


#include "stm32f1xx_hal.h"


#define DHT11_STARTTIME 18000
#define DHT22_STARTTIME 12000
#define DHT11 0x01
#define DHT22 0x02

typedef enum
{
	DHT_OK ,
	DHT_ERROR,
	DHT_TIMEOUT
}DHT_Status_t;

typedef struct
{	
	uint16_t Type;
	uint16_t Pin;
	TIM_HandleTypeDef* Timer;
	GPIO_TypeDef* PORT;
	uint16_t Temp;
	uint16_t Humi;
}DHT_Init_t;

void DHT_Init(DHT_Init_t* DHT, uint8_t DHT_Type, TIM_HandleTypeDef* Timer, GPIO_TypeDef* DH_PORT, uint16_t DH_Pin);
uint8_t DHT_ReadTempHum(DHT_Init_t* DHT);

//uint8_t DHT_Read_Data(DHT_Name* DHT);
#endif


















