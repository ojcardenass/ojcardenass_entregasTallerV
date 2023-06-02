/**
 ******************************************************************************
 * @file           : main.c
 * @title		   : Test SPI
 * @author         : ojcardenass
 * @Nombre         : Oscar Julian Cardenas Sosa
 * @brief          : Test SPI con MAX7219
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
#include "SysTickDriver.h"
#include "PLLDriver.h"
#include "SPIxDriver.h"
#include "MAX7219Driver.h"


// Definición de los handlers necesarios
GPIO_Handler_t 				handlerLED2		 			=	{0};
GPIO_Handler_t				handlerCLK				 	=	{0};
//GPIO_Handler_t				handlerCS					=	{0};
GPIO_Handler_t				handlerMOSI					=	{0};

// Timer encargado del Blinky
BasicTimer_Handler_t 		handlerBlinkyTimer 			=	{0};

//SPI
//SPI_Handler_t				handlerSPI					=	{0};

/* Frecuencia del micro*/
int 		clockSpeed = SPEED_16MHz;
int 		clock = 0;

/*Prototipo de las funciones del main*/
void init_Hardware(void);
uint8_t values[8] = {6,7,3,2,0,1,8,1};
uint8_t values2[8] = {2,3,4,5,7,5,2,0};

uint8_t test1[8] = {6,8,7,4,4,5,4,2};
uint8_t test2[8] = {6,7,6,1,4,2,4,5};
uint8_t test3[8] = {3,2,3,3,1,4,3,1};
uint8_t test4[8] = {2,4,4,2,1,0,0,0};

/* Array de las magnitudes interpoladas a 8 de 32 frecuencias seleccionadas*/
uint8_t freqMagArray1[32] =  {6,8,7,4,4,5,4,2,6,7,6,1,4,2,4,5,3,2,3,3,1,4,3,1,2,4,4,2,1,0,0,0};
uint8_t freqMagArray2[32] = {2,6,4,8,3,5,0,7,1,2,3,5,6,1,4,7,8,0,3,6,2,1,4,8,5,7,0,2,3,4,1,6};
uint8_t freqMagArray3[32] = {4,1,3,7,0,8,5,2,6,4,7,1,2,5,6,3,0,8,1,4,2,6,3,0,7,8,5,4,2,1,6,3};
uint8_t freqMagArray4[32] = {7,2,8,4,3,0,1,6,5,3,4,7,0,2,5,1,8,6,4,3,2,7,0,1,8,5,6,3,2,4,1,7};
uint8_t freqMagArray5[32] = {1,3,0,8,6,4,2,5,7,1,6,0,4,5,3,2,7,8,1,6,2,4,5,3,0,7,8,1,2,3,4,7};
uint8_t freqMagArray6[32] = {5,7,2,3,6,4,0,1,8,5,2,7,3,1,6,0,4,8,2,5,3,7,0,6,1,4,8,7,2,3,5,0};
uint8_t freqMagArray7[32] = {3,0,4,5,1,7,6,2,8,3,6,0,2,4,5,7,1,8,3,6,1,5,7,2,4,8,0,3,1,5,7,2};
uint8_t freqMagArray8[32] = {6,8,5,2,0,4,7,1,3,6,4,8,2,7,1,0,3,5,6,4,1,3,7,0,2,8,5,6,4,1,2,7};
uint8_t freqMagArray9[32] = {0,4,1,7,6,2,5,3,8,0,7,2,3,4,5,1,6,8,0,4,2,1,5,3,7,8,6,0,1,3,4,7};
uint8_t freqMagArray10[32] = {8,2,7,4,3,6,0,1,5,8,7,3,4,6,1,0,2,5,8,7,6,1,0,4,2,3,5,8,7,4,0,2};
uint8_t freqMagArray11[32] = {2,1,6,0,5,4,8,7,3,2,0,5,1,4,3,6,8,7,2,0,6,1,8,5,7,4,3,2,0,1,6,8};

uint8_t* freqMagArrays[11] = {
		freqMagArray1,freqMagArray2,freqMagArray3,freqMagArray4,freqMagArray5,
		freqMagArray6,freqMagArray7,freqMagArray8,freqMagArray9,freqMagArray10,
		freqMagArray11};

/* Array separado de arrays de 8x4, 8 Filas para 4 Modulos*/
uint8_t array2MAX7219[4][8] = {0};

int main(void){
	/*Inicializacion de todos los elementos del sistema*/
	init_Hardware();
	/* Se guarda en la variable el valor de la velocidad del micro*/
	clock = getConfigPLL();
	/* Inicializamos el MAX7219 */
	MAX7219_Init();

	while(1){
//		sendMatrix1(DIGIT1,0x3C);
//		sendMatrix1(DIGIT2,0x18);
//		sendMatrix1(DIGIT3,0x18);
//		sendMatrix1(DIGIT4,0x18);
//		sendMatrix1(DIGIT5,0x18);
//		sendMatrix1(DIGIT6,0x3C);
//
//		sendMatrix2(DIGIT1,0x80);
//		sendMatrix2(DIGIT2,0xC0);
//		sendMatrix2(DIGIT3,0xE0);
//		sendMatrix2(DIGIT4,0xF0);
//		sendMatrix2(DIGIT5,0xF8);
//		sendMatrix2(DIGIT6,0x70);
//
//		sendMatrix3(DIGIT1,0x01);
//		sendMatrix3(DIGIT2,0x03);
//		sendMatrix3(DIGIT3,0x07);
//		sendMatrix3(DIGIT4,0x0F);
//		sendMatrix3(DIGIT5,0x1F);
//		sendMatrix3(DIGIT6,0x0E);
//
//		sendMatrix4(DIGIT1,0x7E);
//		sendMatrix4(DIGIT2,0x7E);
//		sendMatrix4(DIGIT3,0x66);
//		sendMatrix4(DIGIT4,0x66);
//		sendMatrix4(DIGIT5,0x66);
//		sendMatrix4(DIGIT6,0x66);

		//8142241818244281
//		sendMatrix1(DIGIT0,0x81);
//		sendMatrix1(DIGIT1,0x42);
//		sendMatrix1(DIGIT2,0x24);
//		sendMatrix1(DIGIT3,0x18);
//		sendMatrix1(DIGIT4,0x18);
//		sendMatrix1(DIGIT5,0x24);
//		sendMatrix1(DIGIT6,0x42);
//		sendMatrix1(DIGIT7,0x81);
//
//		sendMatrix2(DIGIT1,0x04);
//		sendMatrix2(DIGIT2,0x04);
//		sendMatrix2(DIGIT3,0x1C);
//		sendMatrix2(DIGIT4,0x04);
//		sendMatrix2(DIGIT5,0x04);
//		sendMatrix2(DIGIT6,0x7C);
//
//		sendMatrix3(DIGIT0,0x81);
//		sendMatrix3(DIGIT1,0x42);
//		sendMatrix3(DIGIT2,0x24);
//		sendMatrix3(DIGIT3,0x18);
//		sendMatrix3(DIGIT4,0x18);
//		sendMatrix3(DIGIT5,0x24);
//		sendMatrix3(DIGIT6,0x42);
//		sendMatrix3(DIGIT7,0x81);
//
//		sendMatrix4(DIGIT1,0x04);
//		sendMatrix4(DIGIT2,0x04);
//		sendMatrix4(DIGIT3,0x1C);
//		sendMatrix4(DIGIT4,0x04);
//		sendMatrix4(DIGIT5,0x04);
//		sendMatrix4(DIGIT6,0x7C);

//		setColumnsModx(test1, 1);
//		setColumnsModx(test2, 2);
//		setColumnsModx(test3, 3);
//		setColumnsModx(test4, 4);

	    /* Se extraen 4 fragmentos de la lista de frecuencias y sus magnitudes
	     * dentro del for [i * 8] regresa la posicion inicial del fragmento, y desde
	     * alli la funcion setColumnsModx, recorre desde esa posicion 8 veces, y
	     * asi enviar el valor de la magnitud a la columna correspondiente. i+1 retorna
	     * el numero del modulo que va desde 1 hasta 4 */
//		for (int i = 0; i < 4; i++) {
//		    setColumnsModx(&freqMagArray1[i * 8], i + 1);
//		}
//		delay_ms(10);

		/* Prueba de stream con 11 arrays*/
		// Loop para cada array
		for (int arrIndex = 0; arrIndex < 11; arrIndex++) {
		    uint8_t* currentArray = freqMagArrays[arrIndex];
		    for (int i = 0; i < 4; i++) {
		        setColumnsModx(&currentArray[i * 8], i + 1);
		    }
		    delay_ms(150);
		}

		//clearDisplay();
	} // FIN CICLO INFINITO
} // FIN DEL MAIN

/* Funcion encargada de la inicializacion del sistema*/
void init_Hardware(void){

	// Activamos el coprocesador matematico FPU
	SCB->CPACR |= (0xF << 20);

	configPLL(clockSpeed);
	config_SysTick_ms(CLKSPEED);

	/* ------------------------------ LED de estado ------------------------------ */

	/*Configuracion del LED2 - PA5*/
	handlerLED2.pGPIOx 										= GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed  				= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	/*Cargar la configuracion del LED*/
	GPIO_Config(&handlerLED2);
	// Se inicia con el LED2 encendido
	GPIO_WritePin(&handlerLED2, SET);

	// Configuracion del TIM2 para que haga un blinky cada 250 ms
	handlerBlinkyTimer.ptrTIMx								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 250; //Lanza una interrupcion cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	// Cargando la configuracion del TIM2 en los registros
	BasicTimer_Config(&handlerBlinkyTimer);

	/* ------------------------------ SPI ------------------------------ */

	handlerCLK.pGPIOx									= GPIOB;
	handlerCLK.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerCLK.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerCLK.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerCLK.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerCLK.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerCLK.GPIO_PinConfig.GPIO_PinAltFunMode		= AF5;

	//Cargando la configuracion en los registros
	GPIO_Config(&handlerCLK);

	// Configuración del puerto para el data del encoder, se configura como
	// una entrada digital simple, el encoder ya es pull-up [PA11]
	handlerMOSI.pGPIOx									= GPIOB;
	handlerMOSI.GPIO_PinConfig.GPIO_PinNumber			= PIN_15;
	handlerMOSI.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerMOSI.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerMOSI.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerMOSI.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerMOSI.GPIO_PinConfig.GPIO_PinAltFunMode		= AF5;
	//Cargando la configuracion en los registros
	GPIO_Config(&handlerMOSI);

} // Fin del init_Hardware

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}
