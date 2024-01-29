
#ifndef __DHT_H
#define __DHT_H

#include "stm32f1xx_hal.h"
#include "delay_timer.h"


#define DHT11_STARTTIME 20000
#define DHT22_STARTTIME 14000
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
	uint8_t Temp1, Temp2;
	uint8_t Humi1, Humi2;
}DHT_Name;




void DHT_Init(DHT_Name* DHT, uint8_t DHT_Type, TIM_HandleTypeDef* Timer, GPIO_TypeDef* DH_PORT, uint16_t DH_Pin);
uint8_t DHT_ReadTempHum(DHT_Name* DHT);

//uint8_t DHT_Read_Data(DHT_Name* DHT);
#endif

