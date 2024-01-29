#include <stdio.h>
#include "UART.h"

 void uart_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, 20,&uart2_queue, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(UART, '+', PATTERN_CHR_NUM, 9, 0, 0);
    //Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(UART, 20);
    
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART, data, len);
    ESP_LOGI(logName, "Wrote %d bytes : %s", txBytes,data);
    return txBytes;
}

void tx_task(void *pvParameters)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    for(;;) {
        memset(buffer,'\0',50);
        sprintf(buffer,"*SET_ACTUATOR_STATE:%d,%d\r\n",device,actuator_state);
        sendData(TX_TASK_TAG, buffer);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        if(device == 2) 
        {
            device = 0;
            actuator_state = !actuator_state;
        }
        else device++;
    }
    
}


/**Receive data*/
void rx_task(void *pvParameters)
{
    uart_event_t event;
   // size_t buffered_size;
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE);

    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    for(;;)
    {
        if(xQueueReceive(uart2_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(data, RX_BUF_SIZE);
            ESP_LOGI(RX_TASK_TAG, "uart[%d] event:", UART);
            if(event.type == UART_DATA) {
                
                    ESP_LOGI(RX_TASK_TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(UART, data, event.size, portMAX_DELAY);
                    ESP_LOGI(RX_TASK_TAG, "[DATA EVT]: %s", data);
                    
                    memcpy(data_print,data,event.size);
                    
            }
        }
        if(strlen((char*)data_print) != 0)
        {
            blink_led();
            lcd.clear();
            lcd.print((char*)data_print);
            s_led_state = !s_led_state;
            memset(data_print,'\0',16);
        }
    }
    //uint8_t* data_print = (uint8_t*) malloc(DHT_DATA_SIZE);
    //free(data_print);
    free(data);
    data = NULL;
    vTaskDelete(NULL);
}