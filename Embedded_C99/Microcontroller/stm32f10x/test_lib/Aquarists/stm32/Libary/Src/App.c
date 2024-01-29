#include "App.h"


void appInit(App_Init_t* app, 
	Actuator_Init_t* ActuatorInit, 
	UART_HandleTypeDef *huart, 
	char *rx_buffer,char *tx_buffer,
	uint16_t rx_size, uint16_t tx_size)
{
	app->eCmd = 0;
	app->ActuatorInit = ActuatorInit;
	app->huart = huart;
	app->rxBuffer = rx_buffer;
	app->txBuffer = tx_buffer;
	app->rxSize = rx_size;
	app->txSize = tx_size;
}

 
/* In ph?n h?i ra màn hình terminal */
void response(char *string,App_Init_t* app)
{
   HAL_UART_Transmit_DMA(app->huart, (uint8_t *)string, strlen(string));
}
 
/* Delete rxBuffer - Receiver Buffer*/
void deleteRxData(App_Init_t* app)
{
   memset(app->rxBuffer, '\0', app->rxSize);
	
}

void deleteTxData(App_Init_t* app)
{
   memset(app->txBuffer, '\0', app->txSize);
	
}
 
/* Ki?m tra d? li?u nh?n du?c là command nào */
void checkCommand(App_Init_t* app)
{
   if( strstr(app->rxBuffer, "*SET_DEV_ID:") != NULL)
      app->eCmd = SET_DEV_ID;
   else if(strstr(app->rxBuffer, "*GET_DEV_ID") != NULL)
      app->eCmd = GET_DEV_ID;
   else if(strstr(app->rxBuffer, "*SET_ACTUATOR_STATE:") != NULL)
      app->eCmd = SET_ACTUATOR_STATE;
   else if(strstr(app->rxBuffer, "*GET_ACTUATOR_STATE:") != NULL)
      app->eCmd = GET_ACTUATOR_STATE;
   else
      app->eCmd = 0;
}
 

/* function set state of actuator */
void setActuatorState(App_Init_t* app)
{
	Actuator_Init_t* Actuator = {0};
   /* check the length of command *SET_ACTUATOR_STATE:n,s\r\n (n = 1, 2), (s = 0, 1)*/
   if(strlen(app->rxBuffer) == 25)
   {
      /* Get the number id of Actuator*/
      char *token = NULL;
      token = strtok(app->rxBuffer, ":"); //+SET_ACTUATOR_STATE
      token = strtok(NULL, ","); //n or id of device
      int id = atoi(token) ;
		if(id >= MAX_ACTUATOR)
		{
			response("Wrong device",app);
			return;
		}
		else Actuator = &app->ActuatorInit[id];
      /* Get the state we want to set for device */
      token = strtok(NULL, "\r"); //s or state
      int state = atoi(token);
      token = NULL;
		
      if (state == Actuator_On || state == Actuator_Off)
		{
			GPIO_PinState pinState = (state == Actuator_On) ? GPIO_PIN_SET : GPIO_PIN_RESET;
			HAL_GPIO_WritePin(Actuator->ActuatorPort, Actuator->ActuatorPin, pinState);
		
			sprintf(app->txBuffer, "ACTUATOR %d %s", (uint32_t)Actuator->ActuatorId, (state == Actuator_On) ? "ON" : "OFF");
			response(app->txBuffer, app);
		
			Actuator->ActuatorStatus = state;
		}
      else
      {
         response("\r\n+ERROR:undefined state\r\n",app);
      }
   }
   else
   {
      response("\r\n+ERROR:error command\r\n",app);
   }
}
 
/* function get state of actuator */
void getActuatorState(App_Init_t* app)
{
	Actuator_Init_t* Actuator = {0};
   /* check the length of command *GET_ACTUATOR_STATE:n\r\n (n = 1, 2) id*/
   if(strlen(app->rxBuffer) == 23)
   {
      /* Get the number id of Actuator*/
      char *token = NULL;
      token = strtok(app->rxBuffer, ":"); //+SET_ACTUATOR_STATE
      token = strtok(NULL, "\r"); //n or id of device
      int id = atoi(token) ;
		if(id >= MAX_ACTUATOR)
		{
			response("\r\n+ERROR:wrong device\r\n",app);
			return;
		}
		else 
		{
			Actuator = &app->ActuatorInit[id];
			sprintf(app->txBuffer, "ACTUATOR %d %s", (uint32_t)Actuator->ActuatorId, (Actuator->ActuatorStatus == Actuator_On) ? "ON" : "OFF");
			response(app->txBuffer, app);
		}
	}
   else
   {
      response("\r\n+ERROR:error command\r\n",app);
   }
}

 
/* Hàm x? lý chính */
void commandHandler(App_Init_t* app)
{
   switch(app->eCmd)
   {
		/*
      case SET_DEV_ID:
      setDeviceID(app);
      break;
      case GET_DEV_ID:
      getDeviceID(app);
      break;
		*/
		case GET_ACTUATOR_STATE:
      getActuatorState(app);
      break;
		
      case SET_ACTUATOR_STATE:
      setActuatorState(app);
      break;
      
      default:
      break;
   }
   app->eCmd = 0;
   deleteRxData(app);
	//deleteTxData(app);
}
 
