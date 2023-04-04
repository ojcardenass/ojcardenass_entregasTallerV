/*
 * ExtiConfig_Main.c
 *
 *  Created on: Mar 24, 2023
 *      Author: julian
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"

/*Definicion de los elementos del sistema*/
GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButton = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};
uint32_t counterExti13 = 0;

/*Prototipo de las funciones del main*/
void init_Hardware(void);
void callback_exti13(void);

int main(void){
	/*Inicializacion de todos los elementos del sistema*/
	init_Hardware();
	while(1){

	}

	return 0;
}

void init_Hardware(void){
	/*Configuracion del LED2 - PA5*/
	handlerLED2.pGPIOx 								= GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed  		= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*Cargar la configuracion del LED*/
	GPIO_Config(&handlerLED2);

	GPIO_WritePin(&handlerLED2, SET);

	// Configuracion del TIM2 para que haga un blinky cada 300 ms
	handlerBlinkyTimer.ptrTIMx					= TIM3;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed	= BTIMER_SPEED_1ms; //Incremento de 1 ms en el contador
	handlerBlinkyTimer.TIMx_Config.TIMx_period	= 300; //Lanza una interrupcion cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;

	// Cargando la configuracion del TIM2 en los registros
	BasicTimer_Config(&handlerBlinkyTimer);

	//Configuracion del USER BOTTON (azul) - PC13.
	// Configurar PIN como una entrada digital
	handlerUserButton.pGPIOx 	= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber              = PIN_13;
    handlerUserButton.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN;
    handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING;

    //Cargando la configuracion en los registros
    GPIO_Config(&handlerUserButton);

    /* 2  Activando la señal de reloj de SYSCFG*/
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Activar la señal de reloj

    // 3 Configurar el MUX13 para que utilice el puerto C - 0xF -> 0b1111
    SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos);
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; // Seleccionando al puerto C para el MUX13

    // 4 Configurar el tipo de flanco
    EXTI->FTSR = 0; // Desactivamos todos los posibles flancos de bajada
    EXTI->RTSR = 0; // Llevando el registro a un valor conocido
    EXTI->RTSR |= EXTI_RTSR_TR13;
    /*4.b Activar la interrupcion*/
    EXTI->IMR = 0;
    EXTI->IMR |= EXTI_IMR_IM13; // Activamos la interrupcion 13

    /* 5.a Desactivar todas las interrupciones*/
    __disable_irq();

    /*5.b Matricular la interrupcon en el NVIC*/
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    /*5.c Crear ISR*/
    /*5.d Crear el callback*/
    /*5.e Activar las interrupciones*/
    __enable_irq();



}// Termina el init_Hardware
void callback_exti13(void){
	counterExti13++;
}


void EXTI15_10_IRQHandler(void){
	if((EXTI->PR & EXTI_PR_PR13) != 0){
		EXTI->PR |= EXTI_PR_PR13;  // Limpiar la bandera del EXTI 13
		callback_exti13();
	}
}

void BasicTimer3_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}
