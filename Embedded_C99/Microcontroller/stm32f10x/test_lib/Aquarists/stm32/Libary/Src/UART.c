#include "UART.h"

#define Rx_Size 10
#define Tx_Size 10

typedef struct
{
	uint8_t rxBuffer[Rx_Size];
	uint8_t txBuffer[Tx_Size];
	uint8_t size;
}Uart_DataTypeDef	;



