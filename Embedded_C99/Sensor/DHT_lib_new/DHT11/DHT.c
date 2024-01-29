#include "DHT.h"
#include "delay.h"
#include "includes.h"

unsigned char table[10][8] =
{
	{0,	0,	1,	1,	1,	1,	1,	1},			//0
	{0,	0,	0,	0,	0,	1,	1,	0},			//1
	{0,	1,	0,	1,	1,	0,	1,	1},			//2
	{0,	1,	0,	0,	1,	1,	1,	1},			//3
	{0,	1,	1,	0,	0,	1,	1,	0},			//4
	{0,	1,	1,	0,	1,	1,	0,	1},			//5
	{0,	1,	1,	1,	1,	1,	0,	1},			//6
	{0,	0,	0,	0,	0,	1,	1,	1},			//7
	{0,	1,	1,	1,	1,	1,	1,	1},			//8
	{0,	1,	1,	0,	1,	1,	1,	1}			//9
};

void GPIO_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_DeInit();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14
                                | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11
                                | GPIO_Pin_12;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0
                                | GPIO_Pin_1
                                | GPIO_Pin_2
                                | GPIO_Pin_3
                                | GPIO_Pin_4
                                | GPIO_Pin_5
                                | GPIO_Pin_6
                                | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void digit_choose(int index){
    BitAction v[4];
    int i;
    for (i=0; i<4; i++){
        if (index == i){
            v[i] = Bit_SET;
        }else{
            v[i] = Bit_RESET;
        }
    }
		
		GPIO_WriteBit(GPIOA, GPIO_Pin_11, v[0]);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, v[1]);
    GPIO_WriteBit(GPIOC, GPIO_Pin_14, v[2]);
    GPIO_WriteBit(GPIOC, GPIO_Pin_15, v[3]);
}

void digit_output(int digit){   
		
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)table[digit][0]);
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)table[digit][1]);
    GPIO_WriteBit(GPIOA, GPIO_Pin_2, (BitAction)table[digit][2]);
    GPIO_WriteBit(GPIOA, GPIO_Pin_3, (BitAction)table[digit][3]);
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)table[digit][4]);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)table[digit][5]);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, (BitAction)table[digit][6]);
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)table[digit][7]);
}


void display(int digit){
	static int index=-1;
	int i;
	int base=1000;
	index=(index+1)%4;
	for (i=0;i<index;i++){
		base/=10;
	}
	digit=(digit/base)%10;
	digit_choose(index);//????
	digit_output(digit);//????????????????
}

//count=0;
//while(1)
//{
//	count++;
//	for (int i=0;i<4;i++){
//		display(count/10);//??????????
//		Delay_ms(5);
//	}
//}
//////////////////////////////////////////////////////////////////////////////////////////////////



void DHT11_Set(int state){//
    BitAction s;
    if (state){
        s = Bit_SET;
    }else{
        s = Bit_RESET;
    }
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, s);
}

void DHT11_Pin_OUT(){//
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    DHT11_Set(1);
}

uint8_t DHT11_Check(){
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);
}

void DHT11_Wait(int state){
	while(DHT11_Check()!=state){

	}
}

void DHT11_Pin_IN(){//
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    DHT11_Set(1);
}

void DHT11_Rst(){
    DHT11_Pin_OUT();
    DHT11_Set(0);
    delay_us(25000);//18ms
    DHT11_Set(1);
    delay_us(30);
    DHT11_Set(0);
    DHT11_Pin_IN();
}



u8 DHT11_Init(void)
{			 		    
	DHT11_Rst(); 
	if (DHT11_Check()==0){
		DHT11_Wait(1);
		DHT11_Wait(0);		
	}
	return 0;
}

uint8_t DHT11_Read_Byte(){
    int i, cnt;
    uint8_t data = 0;
    for (i=0; i<8; i++){
        cnt = 0;
        data <<= 1;//
        DHT11_Wait(1);
        while (DHT11_Check() > 0){//
            delay_us(1);
            cnt++;
        }
        data |= cnt > 5;
    }
    return data;
}

uint8_t DHT11_Read_Data(u8 *temp,u8 *humi){
    int i;
		u8 buf[5];
    int cpu_sr;
    //OS_ENTER_CRITICAL();//
    DHT11_Rst();
    if (DHT11_Check() == 0){
        DHT11_Wait(1);
        DHT11_Wait(0);
        for (i=0; i<5; i++){
            buf[i] = DHT11_Read_Byte();
        }
        DHT11_Pin_OUT();//
        //OS_EXIT_CRITICAL();
        if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4]){//
						*humi=buf[0];
						*temp=buf[2];
            return 1;
        }else{
            return 0;
        }
    }else{
        //OS_EXIT_CRITICAL();
				return 0;
    }
}
