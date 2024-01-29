#ifndef __ACTUATOR_H
#define __ACTUATOR_H

#include"stm32f1xx_hal.h"


/* Define MAX_ACTUATOR if any change*/
#define MAX_ACTUATOR 3

/** Control status ON - OFFF 
  * this just for test
  * replace with relay and servo status
*/

typedef enum
{
	Actuator_Off = 0u,
	Actuator_On
}Actuator_Status_t;

/** Actuator Init Typedef
* Actuator Id : ID of Actuator
* ActuatorStatus : Status on or off
* ActuatorPort
* ActuatorPin
*/
typedef struct
{
	
	uint8_t ActuatorId;
	Actuator_Status_t ActuatorStatus;
	GPIO_TypeDef* ActuatorPort;
	uint16_t ActuatorPin;
	
}Actuator_Init_t;

/*Id must be different*/




HAL_StatusTypeDef ActuatorInit(Actuator_Init_t* Actuators,Actuator_Status_t ActuatorStatus, GPIO_TypeDef* ActuatorPort,uint16_t ActuatorPin);
uint8_t NumberOfActuator(void);



#endif