/*
 * Main3.c
 *	Prueba USART con Mensajes
 *  Created on: Mar 24, 2023
 *      Author: ojcardenass
 */
#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"

USART_Handler_t           handlerUSART2      		     = {0} ;  //Handler para configuracion del USART1
BasicTimer_Handler_t      handlerTimer2                  = {0} ;  //Handler para el Timer
GPIO_Handler_t            handlerBlinkyPin               = {0} ;  //Handler para la configuracion del led de estado
GPIO_Handler_t            handlerButton       			 = {0} ;  //Handler para la configuracion del buton azul del micro.
GPIO_Handler_t            handlerTxPin                   = {0} ;  //handler para comunicacion de USART

char 					  bufferData[64] = "¡Hola! Soy el USART del STM32 y estoy funcionando.";

void init_Hardware(void);

int main(void){
    /* Loop forever */
	init_Hardware();

	while(1){
		writeMsg(&handlerUSART2, bufferData);
		writeChar(&handlerUSART2, '\n');
	}
}

void init_Hardware(void){
	//Se enciende el GPIOC para el PIN_5 que es el led 2 de la tarjeta del microcontrolador.
	handlerBlinkyPin.pGPIOx =GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_MEDIUM;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	//Se carga la configuracion del HandlerBlinkyPin, para el led de estado.
	GPIO_Config(&handlerBlinkyPin);

	//Configuramos el puerto GPIO para el módulo serial, necesario para la transmisión de informacion,
	//es decir el caracter.
	handlerTxPin.pGPIOx =GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber        = PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode    = AF7;
	//Se carga la configuracion del GPIO para el puerto serial.
	GPIO_Config(&handlerTxPin);

	//Se configura el Timer 2 con el periodo correspondiente al exigido en la tarea
	handlerTimer2.ptrTIMx = TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode               = BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed              = BTIMER_SPEED_1ms;
	handlerTimer2.TIMx_Config.TIMx_period             = 250;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable    = 1;
	BasicTimer_Config(&handlerTimer2);

	//Se configura el USART1
	handlerUSART2.ptrUSARTx = USART2;
	handlerUSART2.USART_Config.USART_mode             = USART_MODE_TX;
	handlerUSART2.USART_Config.USART_baudrate         = USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize         = USART_DATASIZE_8BIT;   //Se configura el tamaño de bit a comunicar
	handlerUSART2.USART_Config.USART_parity           = USART_PARITY_NONE;     //Se configura sin paridad
	handlerUSART2.USART_Config.USART_stopbits         = USART_STOPBIT_1;
	//Se carga la configuracion del USART2
	USART_Config(&handlerUSART2);
}

void BasicTimer2_Callback(void){
	handlerBlinkyPin.pGPIOx -> ODR ^= GPIO_ODR_OD5;
}
