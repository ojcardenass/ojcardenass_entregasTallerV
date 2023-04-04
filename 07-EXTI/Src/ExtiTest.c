/*
 * ExtiConfig_Main.c
 *
 *  Created on: Mar 24, 2023
 *      Author: julian
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

/*Definicion de los elementos del sistema*/
GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButton = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};
EXTI_Config_t handlerPinExti = {0};
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
	handlerBlinkyTimer.ptrTIMx					= TIM2;
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

    handlerPinExti.edgeType										= EXTERNAL_INTERRUPT_FALLING_EDGE; //flancos de bajada
    handlerPinExti.pGPIOHandler									= &handlerUserButton;
    // Cargando la configuracion del EXTI
    extInt_Config(&handlerPinExti);

}// Termina el init_Hardware
void callback_extInt13(void){
	counterExti13++;
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}
