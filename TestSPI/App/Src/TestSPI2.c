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

// Definición de los handlers necesarios
GPIO_Handler_t 				handlerLED2		 			=	{0};
GPIO_Handler_t				handlerCLK				 	=	{0};
GPIO_Handler_t				handlerCS					=	{0};
GPIO_Handler_t				handlerMOSI					=	{0};

// Timer encargado del Blinky
BasicTimer_Handler_t 		handlerBlinkyTimer 			=	{0};

//SPI
SPI_Handler_t				handlerSPI					=	{0};

/*Prototipo de las funciones del main*/
void SPI2_SendData(uint8_t data);
void init_Hardware(void);
uint8_t TxBuffer = 0x0;

int main(void){
	/*Inicializacion de todos los elementos del sistema*/
	init_Hardware();

	while(1){
		SPI_Send(&handlerSPI, TxBuffer);
	} // FIN CICLO INFINITO
} // FIN DEL MAIN


void init_Hardware(void){
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

	// Configuracion SPI
	handlerSPI.ptrSPIx										= SPI2;
	handlerSPI.SPIConfig.DeviceMode							= SPI_DEVICE_MODE_MASTER;
	handlerSPI.SPIConfig.DirectionBusConfig					= SPI_BUS_CONFIG_TX;
	handlerSPI.SPIConfig.BaudRatePrescaler					= SPI_SCLK_SPEED_DIV4;
	handlerSPI.SPIConfig.DataSize							= SPI_DATA_SIZE_8BITS;
	handlerSPI.SPIConfig.SPIMode							= SPI_MODE0;
	handlerSPI.SPIConfig.FirstBit							= SPI_MSBFIRST;
	handlerSPI.SPIConfig.NSS								= SPI_NSS_ENABLE;
	handlerSPI.SPIConfig.TIMode								= SPI_TIMODE_DISABLE;
	handlerSPI.SPIConfig.CRCCalculation						= SPI_CRC_DISABLE;
	handlerSPI.SPIConfig.FrameFormat						= SPI_MOTOROLA_MODE;
	handlerSPI.SPIConfig.State								= SPI_ENABLE;

	SPI_Config(&handlerSPI);

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

//	// Configuración del botón del encoder PA12
//	handlerCS.pGPIOx									= GPIOB;
//	handlerCS.GPIO_PinConfig.GPIO_PinNumber				= PIN_12;
//	handlerCS.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerCS.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
//	handlerCS.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
//	handlerCS.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	handlerCS.GPIO_PinConfig.GPIO_PinAltFunMode			= AF5;
//    //Cargando la configuracion en los registros
//	GPIO_Config(&handlerCS);

}// Termina el init_Hardware

// Blinky del led de estado
void BasicTimer2_Callback(void){
	handlerLED2.pGPIOx -> ODR ^= GPIO_ODR_OD5;
}


//void SPI2_SendData(uint8_t data)
//{
//    while (!(SPI2->SR & SPI_SR_TXE)) {} // Wait for TXE flag to be set
//    SPI2->DR = data;                    // Write data to DR register
//    while (SPI2->SR & SPI_SR_BSY) {}    // Wait for BSY flag to be reset
//}
