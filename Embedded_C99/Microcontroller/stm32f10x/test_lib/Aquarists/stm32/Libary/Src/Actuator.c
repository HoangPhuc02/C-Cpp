#include "Actuator.h"

static uint8_t NumberActuator = 0;

HAL_StatusTypeDef ActuatorInit(Actuator_Init_t* Actuators,Actuator_Status_t ActuatorStatus, GPIO_TypeDef* ActuatorPort,uint16_t ActuatorPin)
{
	if(NumberActuator == MAX_ACTUATOR) return HAL_ERROR;

	Actuators[NumberActuator].ActuatorId = NumberActuator;
	Actuators[NumberActuator].ActuatorStatus = ActuatorStatus;
	Actuators[NumberActuator].ActuatorPort = ActuatorPort;
	Actuators[NumberActuator].ActuatorPin = ActuatorPin;
	
	/* Setup */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(ActuatorStatus) 
	{
		HAL_GPIO_WritePin(ActuatorPort, ActuatorPin,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(ActuatorPort, ActuatorPin,GPIO_PIN_RESET);
	}
	GPIO_InitStruct.Pin = ActuatorPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ActuatorPort, &GPIO_InitStruct);
	NumberActuator += 1;
	return HAL_OK;
}

uint8_t NumberOfActuator(void)
{
	return NumberActuator;
}

