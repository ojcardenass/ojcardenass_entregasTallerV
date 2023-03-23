/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Oscar Julián Cárdenas Sosa
 * @brief          : Main program body
 ******************************************************************************

 */

#include <stdint.h>
#include <stm32f4xx.h>

#include "GPIOxDriver.h"

//Definicion de un elemento
GPIO_Handler_t handlerLed2 = {0};// PA5

int main(void){

	//Configurando el LED2 -> PA5
	handlerLed2.pGPIOx							= GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerLed2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Cargamos la confguracion del PINA5
	GPIO_Config(&handlerLed2);

	GPIO_WritePin(&handlerLed2,SET);
	while(1){
		GPIOxTooglePin(&handlerLed2);

		for(int i= 0; i < 2000000; i ++){
			__NOP();
		}
	}
	return 0;
}
