/*
 * TestUSART.c
 *	Prueba USART con Mensajes
 *  Created on: Mar 24, 2023
 *      Author: ojcardenass
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"

GPIO_Handler_t			handlerBlinkyPin		=	{0};
GPIO_Handler_t			handlerUserButton		=	{0};
EXTI_Config_t			handlerUserButtonExti	= 	{0};
BasicTimer_Handler_t	handlerBlinkyTimer		=	{0};

/* Elementos para la comunicacion Serial*/
GPIO_Handler_t			handlerPinTX			=	{0};
GPIO_Handler_t			handlerPinRX			=	{0};
USART_Handler_t			usart2Comm				=	{0};
uint8_t 				sendMsg 				= 	0;
uint8_t					usart2DataReceived		=	0;

char	bufferData[64] = "Hola, voy a dominar el mundo :) .";
char	bufferMsg[64] = {0};

void init_Hardware(void);

int main(void){
    /* Inicializamos todos los elementos del sistema */
	init_Hardware();

	/* Loop forever */
	while(1){

		if(sendMsg > 4){
			writeChar(&usart2Comm, 'G');
//			writeMsg(&usart2Comm, bufferData);
//
//			// Crea el string y lo almacena en el arreglo bufferMsg
//			sprintf(bufferMsg, "Valor de sendMsg = %d \n", sendMsg);
//			writeMsg(&usart2Comm, bufferMsg);

			sendMsg = 0;
		}
		if(usart2DataReceived != '\0'){
			/* Echo, envia lo regreso lo que recibe*/
			//writeChar(&usart2Comm, usart2DataReceived);
			/* Se crea un mensaje para el caracter recibido*/
			sprintf(bufferMsg, "Recibido Char = %c \n", usart2DataReceived);
			writeMsg(&usart2Comm, bufferMsg);

			usart2DataReceived = '\0';
		}
	}
}

/* Funcion encargada de la inicializacion del sistema*/
void init_Hardware(void){

	// Configurando el pin A5 para el Led blinky
	handlerBlinkyPin.pGPIOx 								= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber     		= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode        	= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType      	= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed       	= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	//Se carga la configuracion del HandlerBlinkyPin, para el led de estado.
	GPIO_Config(&handlerBlinkyPin);

	//Se configura el Timer 2 para que funcione con el blinky
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable		= 1;
	BasicTimer_Config(&handlerBlinkyTimer);

	// Configurando el pin PC13 para que funcione como una entrada simple que funcionara con la interrupcion
	// EXTI-13
	handlerUserButton.pGPIOx								= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	handlerUserButtonExti.pGPIOHandler						= &handlerUserButton;
	handlerUserButtonExti.edgeType							= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);

	/* Configuracion para la comunicacion serial USART2*/
	handlerPinTX.pGPIOx										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx										= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX);

	/* Configuracion para la comunicacion serial USART1*/
//	handlerPinTX.pGPIOx										= GPIOA;
//	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
//	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
//	GPIO_Config(&handlerPinTX);
//
//	handlerPinRX.pGPIOx										= GPIOA;
//	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_10;
//	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
//	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx									= USART2;
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_interruptionEnableRx		= USART_RX_INTERRUPT_ENABLE;
	usart2Comm.USART_Config.USART_interruptionEnableTx		= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart2Comm);
}

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	//handlerBlinkyPin.pGPIOx -> ODR ^= GPIO_ODR_OD5;
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

/* Callback del userbutton*/
void callback_extInt13(void){
	__NOP();
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido por
 * el puerto del USART2*/
void usart2Rx_Callback (void){
	usart2DataReceived = getRxData();
}

