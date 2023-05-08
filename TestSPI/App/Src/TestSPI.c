/*
 * Main.c
 *
 *  Created on: Mar 24, 2023
 *      Author: ojcardenass
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"

// Definición de los handlers necesarios
GPIO_Handler_t 				handlerLED2		 			=	{0};
BasicTimer_Handler_t handlerBlinkyTimer					= {0};
GPIO_Handler_t handlerUserButton = {0};		//USER_B1

void SPI2_Init(void);
void SPI2_SendData(uint8_t data);
void init_Hardware(void);

int main(void){
	/*Inicializacion de todos los elementos del sistema*/
	init_Hardware();
	SPI2_Init();


	while(1){
		SPI2_SendData(0x2);


//        SPI2_SendData(0x1);
	} // FIN CICLO INFINITO
} // FIN DEL MAIN

void SPI2_Init(void)
{
    // Enable GPIOB and SPI2 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    // Configure PB12 (NSS), PB13 (SCK), PB14 (MISO), and PB15 (MOSI) as alternate function mode
    GPIOB->MODER &= ~(GPIO_MODER_MODE12 | GPIO_MODER_MODE13 | GPIO_MODER_MODE14 | GPIO_MODER_MODE15);
    GPIOB->MODER |= (GPIO_MODER_MODE12_1 | GPIO_MODER_MODE13_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE15_1);
    GPIOB->AFR[1] |= (5 << GPIO_AFRH_AFSEL12_Pos) | (5 << GPIO_AFRH_AFSEL13_Pos) |
                     (5 << GPIO_AFRH_AFSEL14_Pos) | (5 << GPIO_AFRH_AFSEL15_Pos);

    // Configure SPI2
    SPI2->CR1 = 0;
    SPI2->CR1 |= SPI_CR1_MSTR; // Master mode
    SPI2->CR1 |= SPI_CR1_BR_1; // Baud rate = fPCLK/4
    SPI2->CR1 &= ~SPI_CR1_CPOL; // Clock polarity = 0
    SPI2->CR1 &= ~SPI_CR1_CPHA; // Clock phase = 0
    SPI2->CR1 |= SPI_CR1_SSM;  // Software slave management enabled
    SPI2->CR1 |= SPI_CR1_SSI;  // Internal slave select enabled
    SPI2->CR2 |= SPI_CR2_SSOE; // SS output enabled

    // Enable SPI2
    SPI2->CR1 |= SPI_CR1_SPE;
}

void SPI2_SendData(uint8_t data)
{
    while (!(SPI2->SR & SPI_SR_TXE)) {} // Wait for TXE flag to be set
    SPI2->DR = data;                    // Write data to DR register
    while (SPI2->SR & SPI_SR_BSY) {}    // Wait for BSY flag to be reset
}


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

	//Deseamos trabajar con el puerto GPIOC, Pin del USER BOTTON (azul).
	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber              = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLUP;
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_FAST;
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;
}// Termina el init_Hardware


// Blinky del led de estado
void BasicTimer2_Callback(void){
	handlerLED2.pGPIOx -> ODR ^= GPIO_ODR_OD5;
}
