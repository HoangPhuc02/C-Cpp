
#ifndef __UART_H
#define __UART_H
/** UART config*/
/*Pin */


#include <stdio.h>
    #include <string.h>
    #include "stdint.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"
    #include "esp_system.h"
    #include "esp_log.h"
    #include "esp_timer.h"
    #include "driver/uart.h"
    #include "string.h"
    #include <stdlib.h>



#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

/* Buffer*/
static uint8_t data_print[16] = {0};
static char buffer[50] ={0};

/*Buffer size*/
static const int RX_BUF_SIZE = 1024;
static const int TX_BUF_SIZE = 1024;
static const int DHT_DATA_SIZE = 4;

/*UART channel*/
#define UART UART_NUM_2

/* Interrupt queue handler*/
#define PATTERN_CHR_NUM    (3) 
static QueueHandle_t uart2_queue = NULL;


void rx_task(void *pvParameters);
void tx_task(void *pvParameters);
int sendData(const char* logName, const char* data);
 void uart_init(void);

#endif