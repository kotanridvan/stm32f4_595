#include "stm32f4xx.h"                  // Device header
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

uint16_t Read_ADC(void){
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_56Cycles);
	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET){}
	return ADC_GetConversionValue(ADC1);
	
}

void USART_Puts(USART_TypeDef* USARTx, volatile char *s){
	
	while(*s){
		while(!(USARTx ->SR & 0x00000040));
		USART_SendData(USARTx, *s);
		*s++;
	}
	
}

void delayUS(uint32_t time){
	
	time=time*4;
	
	while(time){
		time--;
	}
	
}

void initBoard(void){
	
	GPIO_InitTypeDef gpio_def;
	GPIO_InitTypeDef gpio_def2;
	ADC_InitTypeDef adc_def;
	ADC_CommonInitTypeDef adc_com_def;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	gpio_def2.GPIO_Mode  = GPIO_Mode_OUT;
	gpio_def2.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	gpio_def2.GPIO_OType = GPIO_OType_PP;
	gpio_def2.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &gpio_def2);
	
	gpio_def.GPIO_Mode  = GPIO_Mode_AN;
	gpio_def.GPIO_Pin   = GPIO_Pin_0;
	gpio_def.GPIO_OType = GPIO_OType_PP;
	gpio_def.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio_def);
	
	adc_com_def.ADC_Mode      = ADC_Mode_Independent;
	adc_com_def.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInit(&adc_com_def);
	
	adc_def.ADC_Resolution = ADC_Resolution_12b;
	ADC_Init(ADC1, &adc_def);
	ADC_Cmd(ADC1, ENABLE);

}

void sendToSHR(uint8_t b){
	
	uint8_t i;
	uint8_t bits[8];
	GPIOD->BSRRH = (1<<15);
	GPIOD->BSRRH = (1<<13);
	
	for(i = 0; i < 8; i++){
			bits[i] = (b & ((uint8_t)pow(2,i)))/((uint8_t)pow(2,i)); // getting bit values of a number
	}
	
	for(i = 0; i < 8; i++){
		if(bits[i])
			GPIOD->BSRRL = (1<<15);	// send bits to shift register one-by-one
		else
			GPIOD->BSRRH = (1<<15);	// send bits to shift register one-by-one
		
		delayUS(1);
		GPIOD->BSRRL = (1<<14);	// st_cp of shift register 
		delayUS(1);
		GPIOD->BSRRH = (1<<14);	// st_cp of shift register
	}
	
	delayUS(1);
	GPIOD->BSRRL = (1<<13);	// sh_cp of shift register
	delayUS(1);
	GPIOD->BSRRH = (1<<13);	// sh_cp of shift register
	delayUS(1);
	
}


uint16_t adc_data[8];
uint8_t j;
int main(){
	
	initBoard();
	
	while(1){
		
		for(j=0; j<8; j++){
			sendToSHR((uint8_t)pow(2,j));
			delayUS(10);
			adc_data[j] = Read_ADC();
			delayUS(1000000);
		}
		//motor hareket
	}
	
}
