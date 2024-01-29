/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "lcdMenu.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

extern "C"
{
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
    #include "driver/gpio.h"
    #include "driver/i2c.h"
    #include <stdlib.h>
    //#include <inttypes.h>
}
LiquidCrystal_I2C lcd(0x27, 16, 2);
extern "C"
{
    void app_main(void);
    static void uart_init(void);
    static void rx_task(void *arg);
    static void blink_led(void);
    static void configure_led(void);
    
}



#define BLINK_GPIO  GPIO_NUM_13

// Config button
#define BUTTON_GPIO_UP      34
#define BUTTON_GPIO_DOWN    35
#define BUTTON_GPIO_ENTER   32
#define BUTTON_GPIO_EXIT    26
#define GPIO_INPUT_PIN_SEL  ((1ULL<<BUTTON_GPIO_UP ) | (1ULL<<BUTTON_GPIO_DOWN )| (1ULL<<BUTTON_GPIO_ENTER )| (1ULL<<BUTTON_GPIO_EXIT ))


static const char *BUTTON_TAG = "BUTTON";
// interrupt flag
#define ESP_INTR_FLAG_DEFAULT 0

//queue handler for button
static QueueHandle_t gpio_evt_queue = NULL;

uint64_t last_time = 0;
#define DEBOUNCE_TIME 200 // 0.2S
//CONFIG_BLINK_GPIO

/*Actuator control*/
/*
#define ACTUATOR_ON  1
#define ACTUATOR_OFF 0
*/

typedef enum{
    Relay = 0,
    Servo,
    Led,
    MAX_DEVICE
}Actuator_Type_t;


typedef enum{
    ACTUATOR_OFF = 0,
    ACTUATOR_ON
}Actuator_State_t;


typedef struct 
{
    Actuator_Type_t ActuatorId;
    Actuator_State_t ActuatorState;
    /* data */
}Actuator_Init_t;


static uint8_t s_led_state = 0;
/*
static uint8_t actuator_state = ACTUATOR_OFF;
static uint8_t device = 0;
*/




/** UART config*/
/*Pin */
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


static QueueHandle_t control_queue = NULL;
/*
Actuator_Init_t relay_actuator = {Relay,ACTUATOR_OFF};
Actuator_Init_t servo_actuator(Servo,ACTUATOR_OFF);
Actuator_Init_t led_actuator(Led,ACTUATOR_OFF);
*/
// Todo update real state of actuator everytime we have reset
/*
Exam problem
if Actuator 1 is on 
and we reset the state when reset will be off so when check condition we can turn off Acuator 1

Idea 
using uart get state of every actuators and update

*/
Actuator_Init_t actuators[MAX_DEVICE] = {   
    {Relay,ACTUATOR_OFF},
    {Servo, ACTUATOR_OFF},
    {Led,ACTUATOR_OFF},
};


//extern uint8_t MySelect ;
//uint8_t MySelect = 1;
Menu *menu = &MainMenu;
/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    unsigned int gpio_num = (unsigned int) arg;
    uint64_t cur_time =  esp_timer_get_time();
    if((cur_time - last_time)/1000 >= DEBOUNCE_TIME)
    {
        last_time = cur_time;
        xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);   
    }
}


static void uart_init(void) {
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

static void tx_task(void *pvParameters)
{
    static const char *TX_TASK_TAG = "TX_TASK";
     Actuator_Init_t control_device_p;
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    for(;;) {
        if(xQueueReceive(control_queue, &control_device_p, portMAX_DELAY))
        {
            memset(buffer,'\0',50);
            ESP_LOGI(TX_TASK_TAG, "Transfer command");
            sprintf(buffer,"*SET_ACTUATOR_STATE:%d,%d\r\n",control_device_p.ActuatorId,control_device_p.ActuatorState);
            
            sendData(TX_TASK_TAG, buffer);
        
            
            //vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        /*
        memset(buffer,'\0',50);
        sprintf(buffer,"*SET_ACTUATOR_STATE:%d,%d\r\n",device,actuator_state);
        sendData(TX_TASK_TAG, buffer);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        if(device == 2) 
        {
            device = 0;
            actuator_state = !actuator_state;
        }
        else device++;*/
    }
    
}


/**Receive data*/
static void rx_task(void *pvParameters)
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
                    
                    //memcpy(data_print,data,event.size);     
            }
        }
        /*
        if(strlen((char*)data_print) != 0)
        {
         
            memset(data_print,'\0',16);
        }
        */
    }
    //uint8_t* data_print = (uint8_t*) malloc(DHT_DATA_SIZE);
    //free(data_print);
    free(data);
    data = NULL;
    vTaskDelete(NULL);
}
/** Transfer Data*/


/*led function*/
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
/*=================================================*/

static void gpio_task_example(void* arg);

static void configure_button(void)
{
    //ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    //gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */

    gpio_config_t io_conf = {};
     io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

     //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreatePinnedToCore(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL,1);
     //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

     //hook isr handler for specific gpio pin
    gpio_isr_handler_add((gpio_num_t)BUTTON_GPIO_DOWN, gpio_isr_handler, (void*) BUTTON_GPIO_DOWN);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add((gpio_num_t)BUTTON_GPIO_UP, gpio_isr_handler, (void*) BUTTON_GPIO_UP);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add((gpio_num_t)BUTTON_GPIO_ENTER, gpio_isr_handler, (void*) BUTTON_GPIO_ENTER);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add((gpio_num_t)BUTTON_GPIO_EXIT, gpio_isr_handler, (void*) BUTTON_GPIO_EXIT);

    //printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());
}

/*TODO queue send to activate actuator*/

//void ActuatorCheck()

void app_main(void)
{

    /*Actuator_Init*/
    
    /**/
    configure_led();
    configure_button();
    uart_init();
    
    display.begin(i2c_Address, true); // Address 0x3C default
    display.display();
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    MenuDisplay(menu,menu->cur_cursor);
    control_queue = xQueueCreate(10, sizeof(Actuator_Init_t));
    delay(500);

  //display.clearDisplay();
    xTaskCreatePinnedToCore(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL,0);
    xTaskCreatePinnedToCore(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL,0);
}

static void gpio_task_example(void* arg)
{
    unsigned int io_num;   
    esp_log_level_set(BUTTON_TAG, ESP_LOG_INFO);
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            ESP_LOGI(BUTTON_TAG, "GPIO[%u] intr, val: %d\n", io_num, gpio_get_level((gpio_num_t)io_num));
            //printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
            if(io_num == BUTTON_GPIO_DOWN )
            {
                
                menu->cur_cursor = (menu->cur_cursor == menu->maxSelect) ? 1 : menu->cur_cursor + 1;
                MenuDisplay(menu,menu->cur_cursor);
            }
            else if(io_num == BUTTON_GPIO_UP )
            {
                
                menu->cur_cursor = (menu->cur_cursor == 1) ? menu->maxSelect : menu->cur_cursor - 1;
                MenuDisplay(menu,menu->cur_cursor);
            }
            else if(io_num == BUTTON_GPIO_ENTER )
            {
                uint8_t cursor = menu->cur_cursor;
                if(menu->List[cursor-1] != NULL)
                {
                    menu = menu->List[cursor -1];
                }
                else if(cursor == 1 && menu->ActivationOn != NULL && actuators[menu->pre->cur_cursor - 1].ActuatorState == ACTUATOR_OFF) // check if it cursors to activate on actuator
                {
                    actuators[menu->pre->cur_cursor - 1].ActuatorState = ACTUATOR_ON;
                    menu->ActivationOn((char)(menu->pre->cur_cursor-1),(char)ACTUATOR_ON);
                }
                else if(cursor == 2 && menu->ActivationOff != NULL && actuators[menu->pre->cur_cursor - 1].ActuatorState == ACTUATOR_ON) // check if it cursors to activate off actuator
                {
                    actuators[menu->pre->cur_cursor - 1].ActuatorState = ACTUATOR_OFF;
                    menu->ActivationOff((char)(menu->pre->cur_cursor-1),(char)ACTUATOR_OFF);
                }
                /*
                switch (menu->cur_cursor)
                {
                    case 1:
                    if(menu->List1 != NULL)
                    
                    // TOdO check state
                    // check cur_cursor of pre menu to check which actuator you chose menu->pre->cur_cursor
                    else if(menu->ActivationOn != NULL)
                    {
                        if( strstr(menu->Title,"RELAY")!=NULL && relay_actuator.ActuatorState == ACTUATOR_OFF)
                        {
                            relay_actuator.ActuatorState = ACTUATOR_ON;
                            menu->ActivationOn((char)Relay,(char)ACTUATOR_ON);
                        }
                        else if( strstr(menu->Title,"SERVO")!=NULL && servo_actuator.ActuatorState == ACTUATOR_OFF)
                        {
                            servo_actuator.ActuatorState = ACTUATOR_ON;
                            menu->ActivationOn((char)Servo,(char)ACTUATOR_ON);
                        }
                        else if( strstr(menu->Title,"LED")!=NULL && led_actuator.ActuatorState == ACTUATOR_OFF)
                        {
                            led_actuator.ActuatorState = ACTUATOR_ON;
                            menu->ActivationOn((char)Led,(char)ACTUATOR_ON);
                        }
                    }
                    break;
                    case 2:
                    if(menu->List[] != NULL)
                    {
                       menu = menu->List2;
                    }
                    // TOdO check state
                    else if(menu->ActivationOff != NULL)
                    {
                        if( strstr(menu->Title,"RELAY")!=NULL && relay_actuator.ActuatorState == ACTUATOR_ON)
                        {
                            relay_actuator.ActuatorState = ACTUATOR_OFF;
                            menu->ActivationOff((char)Relay,(char)ACTUATOR_OFF);
                        }
                        else if( strstr(menu->Title,"SERVO")!=NULL && servo_actuator.ActuatorState == ACTUATOR_ON)
                        {
                            servo_actuator.ActuatorState = ACTUATOR_OFF;
                            menu->ActivationOff((char)Servo,(char)ACTUATOR_OFF);
                        }
                        else if( strstr(menu->Title,"LED")!=NULL&& led_actuator.ActuatorState == ACTUATOR_ON)
                        {
                            led_actuator.ActuatorState = ACTUATOR_OFF;
                            menu->ActivationOff((char)Led,(char)ACTUATOR_OFF);
                        }
                    }
                    break;
                    case 3:menu = (menu->List3 == NULL)?menu : menu->List3;break;
                    case 4:menu = (menu->List4 == NULL)?menu : menu->List4;break;
                    
                    default:break;
                }*/
                //MySelect = 1;
                MenuDisplay(menu,menu->cur_cursor);
            }
            else if(io_num == BUTTON_GPIO_EXIT )
            {
                menu = (menu->pre == NULL) ? menu : menu->pre;
               // MySelect = 1;
                MenuDisplay(menu,menu->cur_cursor);
            }
        }
    }
}
// it declare in menuLCD lib as weak so you can  redeclare
void ActuatorsActivation(char Device, char State)
{
    Actuator_Init_t control ;
    switch (Device)
    {
    case Relay:
        control.ActuatorId = Relay;
        
        break;
    
    case Servo:
        control.ActuatorId = Servo;
        break;
    
    case Led:
        control.ActuatorId = Led;
        break;
    default:
        break;
    }
    control.ActuatorState = (Actuator_State_t)State;
    xQueueSend(control_queue,&control,portMAX_DELAY);
}