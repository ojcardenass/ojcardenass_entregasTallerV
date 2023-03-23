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

GPIO_Handler_t handlerOnBoardLed = {0};

int main(void)
{
    /* Configurar el pin */
	handlerOnBoardLed.pGPIOx								= GPIOA;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerOnBoardLed);

	/* Configuracion del TIMER */

	// ACTIVANDO LA SEÑAL DE RELOJ USANDO MACROS
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// DIRECCION
	TIM2->CR1 &= ~TIM_CR1_DIR;

	// Preescaler
	TIM2->PSC = 16000; // 16MHz nos da un ciclo de 1 us

	// Configuraar el CNT
	TIM2->CNT = 0;

	// Configurar el ARR
	TIM2->ARR = 250; //250 ms

	// Activar el TIMER
	TIM2->CR1 |= TIM_CR1_CEN;

	while(1){
		//Resetear la bandera del TIMER y encender el LED con la funcion TogglePin
		if(TIM2->SR & TIM_SR_UIF){
			TIM2->SR &= ~TIM_SR_UIF;
			GPIOxTooglePin(&handlerOnBoardLed);
		}

	}
	return 0;
}
