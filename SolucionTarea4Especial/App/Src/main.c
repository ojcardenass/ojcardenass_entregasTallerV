/**
 ******************************************************************************
 * @file           : main.c
 * @title		   : Solucion Tarea 4 Especial
 * @author         : ojcardenass
 * @Nombre         : Oscar Julian Cardenas Sosa
 * @brief          : Uso de I2C, PWM y USART
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
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"
#include "I2CxDriver.h"
#include "LCDDriver.h"

/* Elementos de funciones varias, LED de estado, banderas y demas*/
GPIO_Handler_t			handlerBlinkyPin		=	{0};
BasicTimer_Handler_t	handlerBlinkyTimer		=	{0};
BasicTimer_Handler_t	handlerSampleTimer		=	{0};
uint8_t					flag1kHz				= 	0;
uint8_t					flagSampling			=	0;
uint8_t					flag2seg				=	0;
uint16_t				counter_ms				=	0;
uint16_t				counter					=	0;

/* Frecuencia del micro*/
int 		clockSpeed = SPEED_80MHz;
int 		clock = 0;
/* Factor de conversión para pasar medidas de acelerómetro a m/s²*/
float 		factConv = 9.78f/16384.0f;


/* Elementos para la comunicacion Serial*/
GPIO_Handler_t			handlerPinTX			=	{0};
GPIO_Handler_t			handlerPinRX			=	{0};
USART_Handler_t			usart6Comm				=	{0};
uint8_t 				sendMsg 				= 	0;
uint8_t					rxData					=	0;
char					bufferData[256] = "Accel MPU-6050 Testing...";

/* Elementos para el PWM*/
GPIO_Handler_t			handlerPinPwmAxisX		=	{0};
GPIO_Handler_t			handlerPinPwmAxisY		=	{0};
GPIO_Handler_t			handlerPinPwmAxisZ		=	{0};
PWM_Handler_t			handlerSignalXPWM		= 	{0};
PWM_Handler_t			handlerSignalYPWM		= 	{0};
PWM_Handler_t			handlerSignalZPWM		= 	{0};
uint16_t				duttyValueX				= 1500;
uint16_t 				duttyValueY 			= 1500;
uint16_t 				duttyValueZ 			= 1500;

/* Configuracion para el I2C del Acelerometro*/
GPIO_Handler_t			handlerI2cSDA			= 	{0};
GPIO_Handler_t			handlerI2cSCL			= 	{0};
I2C_Handler_t			handlerAccelerometer	=	{0};
uint8_t					i2cBuffer				=	0;

#define ACCEL_ADDRESS 	0b01101000
#define ACCEL_XOUT_H 	59	//0x3B
#define ACCEL_XOUT_L 	60	//0x3C
#define ACCEL_YOUT_H 	61	//0x3D
#define ACCEL_YOUT_L 	62	//0x3E
#define ACCEL_ZOUT_H 	63	//0x3F
#define ACCEL_ZOUT_L 	64	//0x40

#define PWR_MGMT_1 		107
#define WHO_AM_I		117

/* Configuracion para el I2C de la LCD*/
GPIO_Handler_t			handlerI2cSDA2			= 	{0};
GPIO_Handler_t			handlerI2cSCL2			= 	{0};
I2C_Handler_t			handlerLCD				=	{0};
char					bufferLCDx[64] 			= {0};
char					bufferLCDy[64] 			= {0};
char					bufferLCDz[64] 			= {0};

/* Direccion de la LCD pin A0 = soldado (0), A1,A2 = libres (1,1)*/
#define LCD_ADDRESS		0b00100110

/* Arrays para guardar informacion del accelerometro*/
/* Array del muestreo constante */
float		xValue[1000] = {0};
float		yValue[1000] = {0};
float		zValue[1000] = {0};
/* Array de los 2 segundos de muestreo*/
float		xValueSample[2000] = {0};
float		yValueSample[2000] = {0};
float		zValueSample[2000] = {0};

/* Funciones para leer los registros del acelerometro*/
int16_t readXValue(I2C_Handler_t *ptrHandlerI2C);
int16_t readYValue(I2C_Handler_t *ptrHandlerI2C);
int16_t readZValue(I2C_Handler_t *ptrHandlerI2C);


/* Funcion para guardar los datos en los array*/
void saveData(void);
/* Funcion detectar los comandos*/
void userCommand(void);
/* Funcion para inicializar la LCD y mostar los datos que son fijos*/
void staticLCD(void);
void init_Hardware(void);

/* Funcion principal del sistema*/
int main(void){
    /* Inicializamos todos los elementos del sistema */
	init_Hardware();
	/* Se guarda en la variable el valor de la velocidad del micro*/
	clock = getConfigPLL();
	staticLCD();
	writeMsg(&usart6Comm, bufferData);
	/* Loop forever */
	while(1){
		// Hacems un eco del valor que nos llega por serial
		if(rxData != '\0'){
			writeChar(&usart6Comm, rxData);
			/* Iniciamos la funcion de deteccion de caracteres enviados por serial*/
			userCommand();
		}

		// Muestreo de datos constante cada 1ms
		if(flag1kHz){
			/* Se guarda en las variables el las componentes de la aceleracion en binarios*/
			int16_t x = readXValue(&handlerAccelerometer);
			int16_t y = readYValue(&handlerAccelerometer);
			int16_t z = readZValue(&handlerAccelerometer);

			/* Se convierte de binario a decimal con el factor de conversion sacado del datasheet*/
			duttyValueX = 495 * (x * factConv) + 10000;
			duttyValueY = 495 * (y * factConv) + 10000;
			duttyValueZ = 495 * (z * factConv) + 10000;

			updateDuttyCycle(&handlerSignalXPWM, duttyValueX);
			updateDuttyCycle(&handlerSignalYPWM, duttyValueY);
			updateDuttyCycle(&handlerSignalZPWM, duttyValueZ);

			/* Si ha pasado 1 segundo, se envian datos a la LCD*/
			if(counter_ms > 1000){
				int16_t x = readXValue(&handlerAccelerometer);
				int16_t y = readYValue(&handlerAccelerometer);
				int16_t z = readZValue(&handlerAccelerometer);

				/* Se convierten los valores a caracteres y se envian a la LCD*/
				sprintf(bufferLCDx, "%.2f m/s2   ", x*factConv);
				moveCursorLCD(&handlerLCD, 0, 8);
				sendMsgLCD(&handlerLCD, bufferLCDx);
				sprintf(bufferLCDy, "%.2f m/s2   ", y*factConv);
				moveCursorLCD(&handlerLCD, 1, 8);
				sendMsgLCD(&handlerLCD, bufferLCDy);
				sprintf(bufferLCDz, "%.2f m/s2   ", z*factConv);
				moveCursorLCD(&handlerLCD, 2, 8);
				sendMsgLCD(&handlerLCD, bufferLCDz);

				counter_ms = 0;
			}
			/* Cuando hayan pasado 2 segundos, se activa la bandera para mostrar los valores y se indica por
			 * comunicacion serial, que el muestreo ha terminado*/
			if(counter > 2000){
				flagSampling = 0;
				sprintf(bufferData," Sampling complete ... \n");
				writeMsg(&usart6Comm, bufferData);
				delay_ms(1000);
				flag2seg = 1;

				counter = 0;
			}
			/* Se guardan los datos cada 1kHz*/
			saveData();
			flag1kHz = 0;
		}

		// Si se activo la bandera de 2 segundos se muestran los datos recolectados en la terminal
		while(flag2seg){
			delay_ms(30);
			sprintf(bufferData, "Accelerometer data \n");
			writeMsg(&usart6Comm, bufferData);

			sprintf(bufferData," X [m/s²]	;	Y [m/s²]	;	Z [m/s²] \n");
			writeMsg(&usart6Comm, bufferData);
			for(int i = 0; i < 2000; i++){
				sprintf(bufferData, "	%.2f m/s²		; %.2f m/s²		; %.2f m/s²		|	Data N° %d \n", xValueSample[i], yValueSample[i],zValueSample[i], i+1);
				writeMsg(&usart6Comm, bufferData);
			}
			flag2seg = 0;
			break;
		}

	}
	return 0;
}

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

	/* ------------------------------ USART6 ------------------------------ */

	/* Configuracion para la comunicacion serial USART6*/
	handlerPinTX.pGPIOx										= GPIOC;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF8;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx										= GPIOC;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF8;
	GPIO_Config(&handlerPinRX);

	usart6Comm.ptrUSARTx									= USART6;
	usart6Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usart6Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart6Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart6Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart6Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart6Comm.USART_Config.USART_interruptionEnableRx		= USART_RX_INTERRUPT_ENABLE;
	usart6Comm.USART_Config.USART_interruptionEnableTx		= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart6Comm);

	/* ------------------------------ Accelerometro ------------------------------ */

	/* Configurando los pines sobre los que funciona el I2C*/
	handlerI2cSCL.pGPIOx 									= GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber 			= PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_HIGH;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2cSCL);

	handlerI2cSDA.pGPIOx 									= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_HIGH;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2cSDA);

	handlerAccelerometer.ptrI2Cx							= I2C1;
	handlerAccelerometer.modeI2C							= I2C_MODE_FM;
	handlerAccelerometer.slaveAddress						= ACCEL_ADDRESS;
	i2c_config(&handlerAccelerometer);

	//Se configura el Timer 3 para que genere la bandera del muestro
	handlerSampleTimer.ptrTIMx 								= TIM4;
	handlerSampleTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerSampleTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerSampleTimer.TIMx_Config.TIMx_period				= 10;
	handlerSampleTimer.TIMx_Config.TIMx_interruptEnable		= 1;
	BasicTimer_Config(&handlerSampleTimer);

	/* ------------------------------ PWM ------------------------------ */

	/* Configuramos el PWM del eje X*/
	handlerPinPwmAxisX.pGPIOx								= GPIOB;
	handlerPinPwmAxisX.GPIO_PinConfig.GPIO_PinNumber		= PIN_1;
	handlerPinPwmAxisX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinPwmAxisX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmAxisX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmAxisX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinPwmAxisX.GPIO_PinConfig.GPIO_PinAltFunMode	= AF2;
	GPIO_Config(&handlerPinPwmAxisX);

	/* Configurando el timer para el que genere el PWM*/
	handlerSignalXPWM.ptrTIMx								= TIM3;
	handlerSignalXPWM.config.channel						= PWM_CHANNEL_4;
	handlerSignalXPWM.config.duttyCicle						= duttyValueX;
	handlerSignalXPWM.config.periodo						= 20000;
	handlerSignalXPWM.config.prescaler						= 80;
	pwm_Config(&handlerSignalXPWM);

	/* Configuramos el PWM del eje Y*/
	handlerPinPwmAxisY.pGPIOx								= GPIOB;
	handlerPinPwmAxisY.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerPinPwmAxisY.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinPwmAxisY.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmAxisY.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmAxisY.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinPwmAxisY.GPIO_PinConfig.GPIO_PinAltFunMode	= AF2;
	GPIO_Config(&handlerPinPwmAxisY);

	/* Configurando el timer para el que genere el PWM*/
	handlerSignalYPWM.ptrTIMx								= TIM3;
	handlerSignalYPWM.config.channel						= PWM_CHANNEL_2;
	handlerSignalYPWM.config.duttyCicle						= duttyValueY;
	handlerSignalYPWM.config.periodo						= 20000;
	handlerSignalYPWM.config.prescaler						= 80;
	pwm_Config(&handlerSignalYPWM);

	/* Configuramos el PWM del eje Z*/
	handlerPinPwmAxisZ.pGPIOx								= GPIOB;
	handlerPinPwmAxisZ.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
	handlerPinPwmAxisZ.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinPwmAxisZ.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmAxisZ.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmAxisZ.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinPwmAxisZ.GPIO_PinConfig.GPIO_PinAltFunMode	= AF2;
	GPIO_Config(&handlerPinPwmAxisZ);

	/* Configurando el timer para el que genere el PWM*/
	handlerSignalZPWM.ptrTIMx								= TIM3;
	handlerSignalZPWM.config.channel						= PWM_CHANNEL_3;
	handlerSignalZPWM.config.duttyCicle						= duttyValueZ;
	handlerSignalZPWM.config.periodo						= 20000;
	handlerSignalZPWM.config.prescaler						= 80;
	pwm_Config(&handlerSignalZPWM);

	/* Activamos la señal*/
	enableOutput(&handlerSignalXPWM);
	enableOutput(&handlerSignalYPWM);
	enableOutput(&handlerSignalZPWM);
	startPwmSignal(&handlerSignalXPWM);
	startPwmSignal(&handlerSignalYPWM);
	startPwmSignal(&handlerSignalZPWM);

	/* ------------------------------ LCD------------------------------ */

	/* Configurando los pines sobre los que funciona el I2C2*/
	handlerI2cSCL2.pGPIOx 									= GPIOB;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_PULLUP;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_HIGH;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2cSCL2);

	handlerI2cSDA2.pGPIOx 									= GPIOB;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinNumber 			= PIN_3;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_PULLUP;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_HIGH;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF9;
	GPIO_Config(&handlerI2cSDA2);

	handlerLCD.ptrI2Cx										= I2C2;
	handlerLCD.modeI2C										= I2C_MODE_SM;
	handlerLCD.slaveAddress									= LCD_ADDRESS;
	i2c_config(&handlerLCD);
} // Fin del init_Hardware

int16_t readXValue(I2C_Handler_t *ptrHandlerI2C){
	uint8_t AccelX_low = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_XOUT_L);
	uint8_t AccelX_high = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_high << 8 | AccelX_low;
	return AccelX;
}

int16_t readYValue(I2C_Handler_t *ptrHandlerI2C){
	uint8_t AccelY_low = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_YOUT_L);
	uint8_t AccelY_high = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_YOUT_H);
	int16_t AccelY = AccelY_high << 8 | AccelY_low;
	return AccelY;
}

int16_t readZValue(I2C_Handler_t *ptrHandlerI2C){
	uint8_t AccelZ_low = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
	return AccelZ;
}

void saveData(void){
	int16_t AccelX = readXValue(&handlerAccelerometer);
	int16_t AccelY = readYValue(&handlerAccelerometer);
	int16_t AccelZ = readZValue(&handlerAccelerometer);

	if(counter_ms>= 0){
		xValue[counter_ms] = AccelX*factConv;
		yValue[counter_ms] = AccelY*factConv;
		zValue[counter_ms] = AccelZ*factConv;
	}

	if( flagSampling && counter >= 0){
		xValueSample[counter] = AccelX*factConv;
		yValueSample[counter] = AccelY*factConv;
		zValueSample[counter] = AccelZ*factConv;
	}
}

/** Función encargada ejecutar los comandos presionados en el terminal */
void userCommand(void){
	/* Tecla 'i' para mostrar la informacion del sistema y los comandos*/
	if(rxData == 'i'){
		sprintf(bufferData, "|||	Hi, the STM32F411 is running at %d MHz	|||\n",clock/1000000);
		writeMsg(&usart6Comm, bufferData);
		delay_ms(200);
		sprintf(bufferData, "\n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "Command Reference and Actions: \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "Commands marked with (*) are essential for correct operation \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "\n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "w	: Retrieves the MPU-6050 address \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "r	: Performs a system reset on the MPU-6050 (*) \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "o	: Initialize the MPU-6050 (*) \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "p	: Retrieves the power state of the MPU-6050 \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "s	: Initiate a sampling process that collects and shows accelerometer data of 2 seconds \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "d	: Displays the data of the accelerometer every 1kHz \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "f	: Stops the display of the data, and returns the maximum and minimum values \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "x	: Retrive and Display one value of the X-axis from the accelerometer \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "y	: Retrive and Display one value of the Y-axis from the accelerometer \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "z	: Retrive and Display one value of the Z-axis from the accelerometer \n");
		writeMsg(&usart6Comm, bufferData);
		rxData = '\0';
	}
	/* Tecla 'o' para inicializar el MPU-6050*/
	else if(rxData == 'o'){
		sprintf(bufferData, "	... Initializing MPU-6050 ... \n");
		writeMsg(&usart6Comm, bufferData);
		sprintf(bufferData, "WHO_AM_I? (r)\n");
		writeMsg(&usart6Comm, bufferData);

		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
		sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
		writeMsg(&usart6Comm, bufferData);

		i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
		int16_t AccelX = readXValue(&handlerAccelerometer);
		int16_t AccelY = readYValue(&handlerAccelerometer);
		int16_t AccelZ = readZValue(&handlerAccelerometer);
		if(AccelX != 0 && AccelY != 0 && AccelZ != 0 ){
			sprintf(bufferData, "	Accelerometer is working OK \n");
			writeMsg(&usart6Comm, bufferData);
			sprintf(bufferData, "	%.2f m/s²		; %.2f m/s²		; %.2f m/s² \n", AccelX*factConv, AccelY*factConv, AccelZ*factConv);
			writeMsg(&usart6Comm, bufferData);
		}
		rxData = '\0';
	}
	/* Tecla 'w' para mostrar la address del MPU-6050*/
	else if(rxData == 'w'){
		sprintf(bufferData, "WHO_AM_I? (r)\n");
		writeMsg(&usart6Comm, bufferData);

		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
		sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
		writeMsg(&usart6Comm, bufferData);
		rxData = '\0';
	}

	else if(rxData == 'p'){
		sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
		writeMsg(&usart6Comm, bufferData);

		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
		sprintf(bufferData, "dataRead = 0x%x \n",(unsigned int) i2cBuffer);
		writeMsg(&usart6Comm, bufferData);
		rxData = '\0';
	}
	/* Tecla 'r' para resetear el MPU-6050*/
	else if(rxData == 'r'){
		sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
		writeMsg(&usart6Comm, bufferData);

		i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
		rxData = '\0';
	}
	/* Tecla 's' para realizar muestreo de 2 segundos y mostrar en terminal*/
	else if(rxData == 's'){
		sprintf(bufferData, "...2 seconds Accelerometer sampling in progress... \n");
		writeMsg(&usart6Comm, bufferData);
		flagSampling = 1;
		rxData = '\0';
	}
	/* Tecla 's' para mostrar en terminal datos tomados cada 1kHz indefinidamente*/
	else if(rxData == 'd'){
		sprintf(bufferData, "Accelerometer data \n");
		writeMsg(&usart6Comm, bufferData);

		sprintf(bufferData," X [m/s²]	;	Y [m/s²]	;	Z [m/s²] \n");
		writeMsg(&usart6Comm, bufferData);
		uint8_t flagConstantSam = 1;
		int16_t maxminXVal[2] = {0};
		int16_t maxminYVal[2] = {0};
		int16_t maxminZVal[2] = {0};
		while(flagConstantSam){
			int16_t AccelXVal = readXValue(&handlerAccelerometer);
			int16_t AccelYVal = readYValue(&handlerAccelerometer);
			int16_t AccelZVal = readZValue(&handlerAccelerometer);

			if(AccelXVal > maxminXVal[0]){
				maxminXVal[0] = AccelXVal;
			}
			if(AccelYVal > maxminYVal[0]){
				maxminYVal[0] = AccelYVal;
			}
			if(AccelZVal > maxminYVal[0]){
				maxminZVal[0] = AccelZVal;
			}
			if(AccelXVal < maxminXVal[1]){
				maxminXVal[1] = AccelXVal;
			}
			if(AccelYVal < maxminYVal[1]){
				maxminYVal[1] = AccelYVal;
			}
			if(AccelZVal < maxminZVal[1]){
				maxminZVal[1] = AccelZVal;
			}

			sprintf(bufferData, "	%.2f m/s²		;		%.2f m/s²		;		%.2f m/s² \n", AccelXVal*factConv, AccelYVal*factConv, AccelZVal*factConv);
			writeMsg(&usart6Comm, bufferData);
			// Tecla para detener la muestra de datos
			if(rxData == 'f'){
				flagConstantSam = 0;
				sprintf(bufferData, "\n");
				writeMsg(&usart6Comm, bufferData);
				sprintf(bufferData, "Max X = %.2f ; Max Y = %.2f ; Max Z = %.2f \n", maxminXVal[0]*factConv, maxminYVal[0]*factConv, maxminZVal[0]*factConv);
				writeMsg(&usart6Comm, bufferData);
				sprintf(bufferData, "Min X = %.2f ; Min Y = %.2f ; Min Z = %.2f \n", maxminXVal[1]*factConv, maxminYVal[1]*factConv, maxminZVal[1]*factConv);
				writeMsg(&usart6Comm, bufferData);
				sprintf(bufferData, "\n");
				writeMsg(&usart6Comm, bufferData);
				rxData = '\0';
				break;
			}
		}
		rxData = '\0';
	}

	else if(rxData == 'x'){
		sprintf(bufferData, "Axis X data (r)\n");
		writeMsg(&usart6Comm, bufferData);
		int16_t AccelX = readXValue(&handlerAccelerometer);
		float AccelXVal = (AccelX/16384.0)*9.81;
		sprintf(bufferData, "AccelX = %.2f \n", AccelXVal);
		writeMsg(&usart6Comm, bufferData);
		rxData = '\0';
	}

	else if(rxData == 'y'){
		sprintf(bufferData, "Axis Y data (r)\n");
		writeMsg(&usart6Comm, bufferData);
		int16_t AccelY = readYValue(&handlerAccelerometer);
		float AccelYVal = (AccelY/16384.0)*9.81;
		sprintf(bufferData, "AccelY = %.2f \n", AccelYVal);
		writeMsg(&usart6Comm, bufferData);
		rxData = '\0';
	}

	else if(rxData == 'z'){
		sprintf(bufferData, "Axis Z data (r)\n");
		writeMsg(&usart6Comm, bufferData);
		int16_t AccelZ = readZValue(&handlerAccelerometer);
		float AccelZVal = (AccelZ/16384.0)*9.81;
		sprintf(bufferData, "AccelZ = %.2f \n", AccelZVal);
		writeMsg(&usart6Comm, bufferData);
		rxData = '\0';
	}

	else{
		rxData = '\0';
	}

}

void staticLCD(void){
	ResetScreenLCD(&handlerLCD);
	InitLCD(&handlerLCD);
	delay_ms(10);
	cleanLCD(&handlerLCD);
	delay_ms(10);
	moveCursorLCD(&handlerLCD, 0, 1);
	sendMsgLCD(&handlerLCD, "Ax = ");
	moveCursorLCD(&handlerLCD, 1, 1);
	sendMsgLCD(&handlerLCD, "Ay = ");
	moveCursorLCD(&handlerLCD, 2, 1);
	sendMsgLCD(&handlerLCD, "Az = ");
	moveCursorLCD(&handlerLCD, 3, 1);
	sendMsgLCD(&handlerLCD, "Sensi = 16834 bit/g");
}

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

/* Callback para hacer el muestreo de los datos cada 1kHz*/
void BasicTimer4_Callback(void){
	flag1kHz = 1;
	counter_ms++;
	if(flagSampling){
		counter ++;
	}
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido por
 * el puerto del USART2*/
void usart6Rx_Callback (void){
	rxData = getRxData();
}


