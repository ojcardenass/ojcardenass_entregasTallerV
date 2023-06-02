/**
 ******************************************************************************
 * @file           : main.c
 * @title		   : Test ADC
 * @author         : ojcardenass
 * @Nombre         : Oscar Julian Cardenas Sosa
 * @brief          : Prueba de uso ADC
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"
#include "ADCDriver.h"
#include "USARTxDriver.h"

/* Elementos de funciones varias, LED de estado, banderas y demas*/
GPIO_Handler_t			handlerBlinkyPin		=	{0};
BasicTimer_Handler_t	handlerBlinkyTimer		=	{0};
BasicTimer_Handler_t	handlerSampler		=	{0};

/* Elementos para la comunicacion Serial*/
GPIO_Handler_t			handlerPinTX			=	{0};
GPIO_Handler_t			handlerPinRX			=	{0};
USART_Handler_t			usart2Comm				=	{0};
uint8_t 				sendMsg 				= 	0;
uint8_t					rxData					=	0;
char					bufferData[256] = "Accel MPU-6050 Testing...";

/* Elementos para el ADC*/
ADC_Config_t			adcConfig				=	{0};
uint16_t adcData	= 0;
uint8_t adcIsComplete = 0;


/* Frecuencia del micro*/
int 		clockSpeed = SPEED_100MHz;
int 		clock = 0;

void init_Hardware(void);

/* Funcion principal del sistema*/
int main(void){
    /* Inicializamos todos los elementos del sistema */
	init_Hardware();
	/* Se guarda en la variable el valor de la velocidad del micro*/
	clock = getConfigPLL();

	while(1){

		// Hacems un eco del valor que nos llega por serial
		if(rxData != '\0'){
			if(rxData == 's'){
				startSingleADC();
			}
			else if(rxData == 'p'){

			}
			rxData = '\0';
		}

		/* Conversion de bit a voltaje*/
		/*(3.3V * adcData)/4096 */

		if(adcIsComplete){
			sprintf(bufferData,"Data: %u \n",adcData);
			writeMsg(&usart2Comm, bufferData);

			adcIsComplete = 0;
		}

	}
	return 0;
}
/* La velocidad maxima del ADC es 36 MHz*/
/* Funcion encargada de la inicializacion del sistema*/
void init_Hardware(void){

	// Activamos el coprocesador matematico FPU
	SCB->CPACR |= (0xF << 20);

	configPLL(clockSpeed);
	config_SysTick_ms(CLKSPEED);

	/* ------------------------------ LED de estado ------------------------------ */

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
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable		= 1;
	BasicTimer_Config(&handlerBlinkyTimer);

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

	usart2Comm.ptrUSARTx									= USART2;
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_interruptionEnableRx		= USART_RX_INTERRUPT_ENABLE;
	usart2Comm.USART_Config.USART_interruptionEnableTx		= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart2Comm);

	/* ------------------------------ ADC ------------------------------ */

	/* Configurando los pines sobre los que funciona el ADC*/
	adcConfig.channel										= ADC_CHANNEL_0;
	adcConfig.dataAlignment									= ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution									= ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod								= ADC_SAMPLING_PERIOD_84_CYCLES;
	adc_Config(&adcConfig);

	//Configuracion del timer del muestreo
	handlerSampler.ptrTIMx 									= TIM5;
	handlerSampler.TIMx_Config.TIMx_mode					= BTIMER_MODE_UP;
	handlerSampler.TIMx_Config.TIMx_speed					= BTIMER_SPEED_10us;
	handlerSampler.TIMx_Config.TIMx_period					= 50;
	handlerSampler.TIMx_Config.TIMx_interruptEnable			= 1;
	BasicTimer_Config(&handlerSampler);

} // Fin del init_Hardware

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
}

void BasicTimer5_Callback(void){
	startSingleADC();
}
/* Esta funcion se ejecuta cada vez que un caracter es recibido por
 * el puerto del USART2*/
void usart2Rx_Callback (void){
	rxData = getRxData();
}


void adcComplete_Callback(void){
	adcData = getADC();
	adcIsComplete = SET;

//	dataADC[adcCounter] = getADC();
//	if(adcCounter < (numberOfChannels-1)){
//		adcCounter ++;
//	}
//	else{
//		adcIsComplete = 1;
//		adcCounter = 0;
//	}
}


