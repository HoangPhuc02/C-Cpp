
/**
*   @file    demo.c
*   @brief   demo
*   @details 
*/

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "stm32f10x.h"

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef enum
{
	NO_CLICK 	  = 0x00,
	SINGLE_CLICK  = 0x01,
	DOUBLE_CLICK  = 0x02,
	LONGCLICK_1S = 0x03
}ButtonState_Typedef;

typedef struct
{
	GPIO_TypeDef* Button_Port;
	uint16_t Button_Pin;
	ButtonState_Typedef State;
	uint8_t isPress;              /* Check if button is pressed */
	uint16_t timePress;				/* Check if it is hold*/
	uint16_t timeDouble;				/* Check if it is double click */
	
}ButtonName_Typedef;
/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
/*
	** Button 
	Button 1  - B5
	Button 2  - B6
	** Led 
	Led Red   - B3
	Led Green - B4
*/

#define DEBOUND_TIME 50					/* Avoid debound when click button */
#define SINGLE_CLICK_TIME 1000		/* Define single click or long click*/
#define DOUBLE_CLICK_TIME 300			/* Time between 2 click */
#define PRESSED 0;
#define RELEASED 1;


/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/
//static const int sc4l_variable1;

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/
static GPIO_InitTypeDef gpio_init;
static ButtonName_Typedef button1;
static ButtonName_Typedef button2;

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/
//const int sc4g_variable1;

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/
//int s4l_variable1;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void DelayInit(void);
static void DelayUs(uint32_t us) ;


/*==================================================================================================
*                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void gpiob_init(void);

void button_init(void);

ButtonState_Typedef Button_Read(ButtonName_Typedef* Button);
unsigned char Button_ReadPin(ButtonName_Typedef* Button);



int main(void)
{
	
	ButtonState_Typedef status = NO_CLICK ;
	DelayInit();
	gpiob_init();
	button_init();
	while(1)
	{
		status = Button_Read(&button1);
		if(status == DOUBLE_CLICK)
		{
			
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			status = NO_CLICK;
		}
		if(status == SINGLE_CLICK)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
			
			status = NO_CLICK;
		}
		if(status == LONGCLICK_1S )
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
			DelayUs(1000000);
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			DelayUs(1000000);
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
			DelayUs(1000000);
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			DelayUs(1000000);
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
			DelayUs(1000000);
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			DelayUs(1000000);
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
			DelayUs(1000000);
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			DelayUs(1000000);
			status = NO_CLICK;
		}
	}
}
/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
/**
* @brief          
* @details        
*
* @param[in]      
*                             
*
* @return         void
*
*
* @pre            
* @post           
*
* @api
*
*/
void gpiob_init(void)
{
	/* Step 1 : Enable Clock ABP2 for GPIOC */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* Step 2 : Fill in GPIO_InitTypeDef gpio_init for output */
	gpio_init.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	/* Step 3 : Init the struct to enable GPIOC output pin */
	GPIO_Init(GPIOB, &gpio_init);
	/* Step 4 : Fill in GPIO_InitTypeDef gpio_init for input */
	gpio_init.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	gpio_init.GPIO_Mode = GPIO_Mode_IPU; /* Input put up , auto high when not click button */
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	/* Step 3 : Init the struct to enable GPIOC pin */
	GPIO_Init(GPIOB, &gpio_init);	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/* Step 2 : Fill in GPIO_InitTypeDef gpio_init for output */
	gpio_init.GPIO_Pin = GPIO_Pin_13;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	/* Step 3 : Init the struct to enable GPIOC output pin */
	GPIO_Init(GPIOC, &gpio_init);
}

void button_init(void)
{
	button1.Button_Port = GPIOB;
	button1.Button_Pin = GPIO_Pin_6;
	button1.isPress = 0;              
	button1.timePress = 0;				
	button1.timeDouble = 0;	
	
	button1.Button_Port = GPIOB;
	button2.Button_Pin = GPIO_Pin_5;
	button2.isPress = 0;             
	button2.timePress = 0;				
	button2.timeDouble = 0;	
}	

/* Simple DelayUs */

unsigned char Button_ReadPin(ButtonName_Typedef* Button)
{
	return GPIO_ReadInputDataBit(Button->Button_Port,Button->Button_Pin);
}
ButtonState_Typedef Button_Read(ButtonName_Typedef* Button)
{
	Button->State = NO_CLICK;
	while(Button_ReadPin(Button) == 0) /*Check if button is clicked */
	{
		Button->timePress++;
		Button->isPress = 1;
		DelayUs(1000);
	}
	if(Button->isPress)
	{
		while(Button_ReadPin(Button) == 1) /*Check if button is released */
		{
			Button->timeDouble++;
			DelayUs(1000);
			if(Button->timeDouble > DOUBLE_CLICK_TIME)
			{
				if(Button->timePress > DEBOUND_TIME && Button->timePress <= SINGLE_CLICK_TIME)
				{
					Button->isPress = 0;
					Button->timePress = 0;
					Button->timeDouble = 0;
					Button->State = SINGLE_CLICK;
					return Button->State;
				}
				else if(Button->timePress > SINGLE_CLICK_TIME)
				{
					Button->isPress = 0;
					Button->timePress = 0;
					Button->timeDouble = 0;
					Button->State = LONGCLICK_1S;
					return Button->State;
				}
			}
		}
		while(Button_ReadPin(Button) == 0)
		{
			Button->isPress = 0;
			Button->timePress = 0;
			Button->timeDouble = 0;
			Button->State = DOUBLE_CLICK;
			return Button->State;
		}
	}
	return NO_CLICK;
}

static void DelayInit(void)
{
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk;

  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

  DWT->CYCCNT = 0;

  /* 3 NO OPERATION instructions */
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");
}

static void DelayUs(uint32_t us) {
  uint32_t cycles = (SystemCoreClock/1000000L)*us;
  uint32_t start = DWT->CYCCNT;
  volatile uint32_t cnt;

  do
  {
    cnt = DWT->CYCCNT - start;
  } while(cnt < cycles);
}
