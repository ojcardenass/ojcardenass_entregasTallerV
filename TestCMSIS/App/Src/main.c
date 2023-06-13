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

#include "arm_math.h"

GPIO_Handler_t			handlerBlinkyPin		=	{0};
GPIO_Handler_t			handlerUserButton		=	{0};
EXTI_Config_t			handlerUserButtonExti	= 	{0};
BasicTimer_Handler_t	handlerBlinkyTimer		=	{0};

/* Elementos para la comunicacion Serial*/
GPIO_Handler_t			handlerPinTX			=	{0};
GPIO_Handler_t			handlerPinRX			=	{0};
USART_Handler_t			usart2Comm				=	{0};
uint8_t 				rxData	 				= 	0;
uint8_t					usart2DataReceived		=	0;

char	bufferData[64] = "Hola, voy a dominar el mundo :) .";
char	bufferMsg[64] = {0};

/* Configuracion para el I2C*/
GPIO_Handler_t			handlerI2cSDA			= 	{0};
GPIO_Handler_t			handlerI2cSCL			= 	{0};
I2C_Handler_t			handlerAccelerometer	=	{0};
uint8_t					i2cBuffer				=	0;
#define ACCEL_ADDRESS 	0b1101000; //0b1101001
#define ACCEL_XOUT_H 	59	//0x3B
#define ACCEL_XOUT_L 	60	//0x3C
#define ACCEL_YOUT_H 	61	//0x3D
#define ACCEL_YOUT_L 	62	//0x3E
#define ACCEL_ZOUT_H 	63	//0x3F
#define ACCEL_ZOUT_L 	64	//0x40
uint8_t					accelRegistersToRead[6]	=	{ACCEL_XOUT_H,ACCEL_XOUT_L,ACCEL_YOUT_H,ACCEL_YOUT_L,ACCEL_ZOUT_H,ACCEL_ZOUT_L};
uint8_t					dataAccelRegisters[6]	=	{0};
int16_t 				accelData[3]			= 	{0};

#define PWR_MGMT_1 		107
#define WHO_AM_I		117

void init_Hardware(void);

/* Frecuencia del micro*/
int 		clockSpeed = SPEED_100MHz;
int 		clock = 0;

#define SIZE_FFT 512
float32_t fft_input[SIZE_FFT];
float32_t fft_output[SIZE_FFT];
float32_t fft_output2[SIZE_FFT];

// Find the maximum value in the fft_output buffer
float32_t max_value;
uint32_t max_index;
float freq;

arm_rfft_fast_instance_f32 fft_instance;

float32_t samples[512] = {
    0.707, 0.924, 0.383, -0.924, -0.707, 0.000, 0.707, 0.383,
    -0.707, -0.383, 0.000, 0.383, 0.707, 0.924, 0.000, -0.924,
    -0.707, 0.383, 0.707, -0.383, -0.707, 0.000, 0.707, 0.924,
    -0.383, -0.924, 0.707, 0.000, -0.707, -0.383, 0.707, -0.924,
    -0.707, 0.383, 0.000, -0.383, 0.707, -0.924, 0.000, 0.924,
    -0.707, -0.383, -0.707, 0.383, -0.707, -0.924, 0.383, 0.000,
    0.707, -0.924, -0.383, 0.924, 0.707, 0.000, -0.707, 0.383,
    0.707, 0.924, 0.000, -0.924, 0.707, -0.383, -0.707, -0.383,
    0.707, -0.924, -0.707, -0.383, -0.000, 0.383, -0.707, 0.924,
    0.707, -0.383, 0.000, 0.383, -0.707, -0.924, 0.707, 0.383,
    -0.707, 0.383, -0.000, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, 0.383, -0.707, -0.383, -0.707, -0.924, 0.383, 0.000,
    0.707, -0.924, -0.383, 0.924, 0.707, -0.000, -0.707, 0.383,
    0.707, 0.924, 0.000, -0.924, -0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383,0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383, 0.707, -0.383,
    -0.707, -0.924, 0.383, 0.000, -0.707, 0.924, 0.383, -0.924,
    -0.707, -0.000, 0.707, -0.383, 0.707, 0.924, 0.000, -0.924,
    0.707, -0.383, -0.707, -0.383, 0.707, -0.924, -0.707, 0.383,
    -0.000, -0.383, 0.707, 0.924, -0.707, 0.383
    };

int main(void){

    /* Inicializamos todos los elementos del sistema */
	init_Hardware();
	writeMsg(&usart2Comm, bufferData);
	/* Loop forever */

	arm_rfft_fast_init_f32(&fft_instance, SIZE_FFT);
	arm_rfft_fast_f32(&fft_instance, samples, fft_output, 0);
	arm_cmplx_mag_f32(fft_output, fft_output2, SIZE_FFT/2);

	arm_max_f32(fft_output2, SIZE_FFT/2, &max_value, &max_index);
	float freq = max_index * 5000 / SIZE_FFT;


	//float frequency = (binIndex * samplingRate) / (float)fftLength;

	// Convirtiendo a rango logaritmico
	/* 20 * log_10(fft_output)*/
	/* 48 kHz sampling*/
	/* 1024 es el tamaño del muestreo*/
	/* Frecuencia a monitorear = f * (1024/(46875/2))*/

	// The max_value variable now contains the maximum value in the fft_output buffer

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

				sprintf(bufferData, "max_index = %lu \n", max_index);
				writeMsg(&usart2Comm, bufferData);
				sprintf(bufferData, "frequency = %.2f \n", freq);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
			}

			else if(rxData == 'x'){
				sprintf(bufferData, "Axis X data (r)\n");
				writeMsg(&usart2Comm, bufferData);

				i2c_readMultipleRegister(&handlerAccelerometer,ACCEL_XOUT_H,dataAccelRegisters,6);
				// Combine the high and low bytes for each axis
				for (int i = 0; i < 3; i++) {
				    accelData[i] = (int16_t)((dataAccelRegisters[i * 2] << 8) | dataAccelRegisters[i * 2 + 1]);
				}

				float AccelXVal = (accelData[0]/16384.0)*9.81;
				float AccelYVal = (accelData[1]/16384.0)*9.81;
				float AccelZVal = (accelData[2]/16384.0)*9.81;
				sprintf(bufferData, "	%.2f m/s²		; %.2f m/s²		; %.2f m/s² \n", AccelXVal, AccelYVal, AccelZVal);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
			}

			else{
				rxData = '\0';
			}
		}
	}
}

/* Funcion encargada de la inicializacion del sistema*/
void init_Hardware(void){

	// Activamos el coprocesador matematico FPU
	SCB->CPACR |= (0xF << 20);

	configPLL(clockSpeed);
	config_SysTick_ms(CLKSPEED);

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

} // Fin del init_Hardware

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido por
 * el puerto del USART2*/
void usart2Rx_Callback (void){
	rxData = getRxData();
}


