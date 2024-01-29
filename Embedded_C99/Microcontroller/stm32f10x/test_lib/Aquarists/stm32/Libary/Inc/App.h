#ifndef __APP_H
#define __APP_H

#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "Actuator.h"
typedef enum 
{
   SET_DEV_ID = 1,
   SET_ACTUATOR_STATE,
   GET_DEV_ID,
   GET_ACTUATOR_STATE,
}eCommand;
 


typedef struct
{
	eCommand eCmd;
	Actuator_Init_t* ActuatorInit;
	UART_HandleTypeDef *huart;
	char* txBuffer;
	char* rxBuffer;
	uint16_t rxSize;
	uint16_t txSize;
}App_Init_t;

//char rxBuffer[50] = {0}; //Khai báo b? d?m d? ch?a d? li?u d?n


 
/* In ph?n h?i ra màn hình terminal */
void response(char *string, App_Init_t* app);
 
/* Xoá b? d?m nh?n d? li?u */
void deleteRxData(App_Init_t* app);
 
void deleteTxData(App_Init_t* app);
/* Ki?m tra d? li?u nh?n du?c là command nào */
void checkCommand(App_Init_t* app);
 
/* Hàm c?u hình ID cho thi?t b? */
//void setDeviceID(App_Init_t* app);
 
/* Hàm l?y ID c?a thi?t b? */
//void getDeviceID(App_Init_t* app);
 
/* Hàm c?u hình tr?ng thái cho dèn led */
void setActuatorState(App_Init_t* app);
 
/* Hàm l?y tr?ng thái c?a dèn led */
void getActuatorState(App_Init_t* app);
 
 
 void commandHandler(App_Init_t* app);


void appInit(App_Init_t* app,
				Actuator_Init_t* ActuatorInit, 
				UART_HandleTypeDef *huart, 
				char *rx_buffer,char *tx_buffer, 
				uint16_t rx_size, uint16_t tx_size);

#endif


