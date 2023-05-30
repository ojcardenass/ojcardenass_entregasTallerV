/*
 * TestADC.c
 *	Prueba ADC
 *  Created on: Mar 24, 2023
 *      Author: ojcardenass
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "I2CxDriver.h"

GPIO_Handler_t			handlerBlinkyPin		=	{0};
BasicTimer_Handler_t	handlerBlinkyTimer		=	{0};

/* Elementos para la comunicacion Serial*/
GPIO_Handler_t			handlerPinTX			=	{0};
GPIO_Handler_t			handlerPinRX			=	{0};
USART_Handler_t			usart2Comm				=	{0};
uint8_t 				sendMsg 				= 	0;
uint8_t					rxData					=	0;
char					bufferData[64] = "Accel MPU-6050 Testing...";
char					bufferMsg[64] = {0};

/* Configuracion para el I2C*/
GPIO_Handler_t			handlerI2cSDA			= 	{0};
GPIO_Handler_t			handlerI2cSCL			= 	{0};
I2C_Handler_t			handlerAccelerometer	=	{0};
uint8_t					i2cBuffer				=	0;

#define ACCEL_ADDRESS 	0b1101001;
#define ACCEL_XOUT_H 	59	//0x3B
#define ACCEL_XOUT_L 	60	//0x3C
#define ACCEL_YOUT_H 	61	//0x3D
#define ACCEL_YOUT_L 	62	//0x3E
#define ACCEL_ZOUT_H 	63	//0x3F
#define ACCEL_ZOUT_L 	64	//0x40

#define PWR_MGMT_1 		107
#define WHO_AM_I		117


void init_Hardware(void);

int main(void){
    /* Inicializamos todos los elementos del sistema */
	init_Hardware();
	writeMsg(&usart2Comm, bufferData);
	/* Loop forever */
	while(1){
		// Hacems un eco del valor que nos llega por serial
		if(rxData != '\0'){
			writeChar(&usart2Comm, rxData);

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&usart2Comm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
			}

			else if(rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
				writeMsg(&usart2Comm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				sprintf(bufferData, "dataRead = 0x%x \n",(unsigned int) i2cBuffer);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
			}

			else if(rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 state (w)\n");
				writeMsg(&usart2Comm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				rxData = '\0';
			}

			else if(rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 state (w)\n");
				writeMsg(&usart2Comm, bufferData);

				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
				rxData = '\0';
			}

			else if(rxData == 'x'){
				// Pendiente video min 21:36

			}
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

	/* Configurando los pines sobre los que funciona el I2C*/
	handlerI2cSCL.pGPIOx 									= GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber 			= PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_PULLUP;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2cSCL);

	handlerI2cSDA.pGPIOx 									= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_PULLUP;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2cSDA);

	handlerAccelerometer.ptrI2Cx							= I2C1;
	handlerAccelerometer.modeI2C							= I2C_MODE_FM;
	handlerAccelerometer.slaveAddres						= ACCEL_ADDRESS;
	i2c_config(&handlerAccelerometer);
}

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	//handlerBlinkyPin.pGPIOx -> ODR ^= GPIO_ODR_OD5;
	GPIOxTooglePin(&handlerBlinkyPin);
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido por
 * el puerto del USART2*/
void usart2Rx_Callback (void){
	rxData = getRxData();
}

