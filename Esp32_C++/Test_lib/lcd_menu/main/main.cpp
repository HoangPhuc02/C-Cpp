/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "Arduino.h"
#include "LiquidCrystal_I2C.h"

extern "C"
{
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include "esp_log.h"
    #include "driver/uart.h"
    #include "string.h"
    #include "driver/gpio.h"
    #include "driver/i2c.h"
}
LiquidCrystal_I2C lcd(0x27, 16, 2);
extern "C"
{
    void app_main(void);
    void init(void);
    static void rx_task(void *arg);
    static void blink_led(void);
    static void configure_led(void);
}
static const int RX_BUF_SIZE = 1024;
static const int DHT_DATA_SIZE = 4;
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

#define BLINK_GPIO  GPIO_NUM_13
#define BUTTON_GPIO GPIO_NUM_12
//CONFIG_BLINK_GPIO

static uint8_t s_led_state = 1;
static uint8_t actuator_state = 1;

static uint8_t device = 0;

static uint8_t data_print[16] = {0};
static char buffer[50] ={0};
#define UART UART_NUM_2

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART, data, len);
    ESP_LOGI(logName, "Wrote %d bytes : %s", txBytes,data);
    return txBytes;
}

static void tx_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1) {
        memset(buffer,'\0',50);
        sprintf(buffer,"%s%d%c%d%s","*SET_ACTUATOR_STATE:",device,',',actuator_state,"\r\n");
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

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    //uint8_t* data_print = (uint8_t*) malloc(DHT_DATA_SIZE);
    while (1) {
        const int rxBytes = uart_read_bytes(UART, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        
            blink_led();
            lcd.clear();
            memcpy(data_print,data,rxBytes);
            lcd.print((char*)data_print);
            s_led_state = !s_led_state;
            memset(data_print,'\0',16);
        }
    }
    free(data_print);
    free(data);
}
static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

static void configure_led(void)
{
    //ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

static void configure_button(void)
{
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
}
void app_main(void)
{
    configure_led();
    lcd.begin();
	// Turn on the blacklight and print a message.
	lcd.backlight();
    init();
    lcd.print("H");
    lcd.print("e");
    lcd.print("l");
    lcd.print("l");
    lcd.print("o");

    delay(2000);
    xTaskCreatePinnedToCore(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL,0);
    xTaskCreatePinnedToCore(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL,1);
}