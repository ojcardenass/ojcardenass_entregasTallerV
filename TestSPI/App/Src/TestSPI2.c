/*
 * Main.c
 *
 *  Created on: Mar 24, 2023
 *      Author: ojcardenass
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "SPIxDriver.h"
#include "MAX7219Driver.h"
#include "SysTickDriver.h"

// Definición de los handlers necesarios
GPIO_Handler_t 				handlerLED2		 			=	{0};
GPIO_Handler_t				handlerCLK				 	=	{0};
//GPIO_Handler_t				handlerCS					=	{0};
GPIO_Handler_t				handlerMOSI					=	{0};

// Timer encargado del Blinky
BasicTimer_Handler_t 		handlerBlinkyTimer 			=	{0};

//SPI
//SPI_Handler_t				handlerSPI					=	{0};

/*Prototipo de las funciones del main*/
void init_Hardware(void);
uint8_t values[8] = {2,3,4,5,6,7,1,0};


int main(void){
	/*Inicializacion de todos los elementos del sistema*/
	init_Hardware();
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
		sendMatrix1(DIGIT0,0x81);
		sendMatrix1(DIGIT1,0x42);
		sendMatrix1(DIGIT2,0x24);
		sendMatrix1(DIGIT3,0x18);
		sendMatrix1(DIGIT4,0x18);
		sendMatrix1(DIGIT5,0x24);
		sendMatrix1(DIGIT6,0x42);
		sendMatrix1(DIGIT7,0x81);

		sendMatrix2(DIGIT1,0x04);
		sendMatrix2(DIGIT2,0x04);
		sendMatrix2(DIGIT3,0x1C);
		sendMatrix2(DIGIT4,0x04);
		sendMatrix2(DIGIT5,0x04);
		sendMatrix2(DIGIT6,0x7C);

		sendMatrix3(DIGIT0,0x81);
		sendMatrix3(DIGIT1,0x42);
		sendMatrix3(DIGIT2,0x24);
		sendMatrix3(DIGIT3,0x18);
		sendMatrix3(DIGIT4,0x18);
		sendMatrix3(DIGIT5,0x24);
		sendMatrix3(DIGIT6,0x42);
		sendMatrix3(DIGIT7,0x81);

		sendMatrix4(DIGIT1,0x04);
		sendMatrix4(DIGIT2,0x04);
		sendMatrix4(DIGIT3,0x1C);
		sendMatrix4(DIGIT4,0x04);
		sendMatrix4(DIGIT5,0x04);
		sendMatrix4(DIGIT6,0x7C);
//		setColumn(8,values,1);

		delay_ms(10);
		//clearDisplay();
	} // FIN CICLO INFINITO
} // FIN DEL MAIN


void init_Hardware(void){
	/* Configuramos el SysTick*/
	config_SysTick_ms(SYSTICK_LOAD_VALUE_16MHz_1ms);

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

	// Configuración del puerto para el clock del encoder, se configura como
	// una entrada digital simple, el encoder ya es pull-up [PB12]
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

}// Termina el init_Hardware

// Blinky del led de estado
void BasicTimer2_Callback(void){
	//handlerBlinkyPin.pGPIOx -> ODR ^= GPIO_ODR_OD5;
	GPIOxTooglePin(&handlerLED2);
}
