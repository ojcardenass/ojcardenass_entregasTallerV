/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Oscar Julián Cárdenas Sosa
 * @brief          : Main program body
 ******************************************************************************

 */

#include <stdint.h>
#include "stm32f411xx_hal.h"

int main(void)
{

	//Configuracion inicial del MCU
	//Hacer una mascara para configurar el pin en 0, y luego sin modificar
	//las demas posiciones del registro, para despues poner un 1
	RCC->AHB1ENR &= ~(1 << 0); //Limpiando la posicion 0 del registro
	RCC->AHB1ENR |= (1 << 0); //Activamos la señal de reloj del GPIOA

	// Configurando el pin PA5 como salida
	GPIOA->MODER &= ~(0b11 << 10); //Limpiando las posiciones 11:10 del MODER
	GPIOA->MODER |= (0b01 << 10); //Configurando el PIN A5 como salida

	//Config OTYPE
	GPIOA->OTYPER &= ~(1 << 5); //Limpiamos la posicion, configuracion Push-Pull

	// Config OSPEED
	GPIOA->OSPEEDR &= ~(0b11 << 10); //Limpiando las posiciones 11:10
	GPIOA->OSPEEDR |= (0b10 << 10); //Velocidad de salida en FAST

	// Configuracion de las resistencias en PU-PD
	GPIOA->PUPDR &= ~(0b11 << 10); //Limpiando las posiciones 11:10, no-PUPD

	GPIOA->ODR &= ~(1 << 5);	//Limpiamos la salida PA5
	GPIOA->ODR |= (1 << 5);		//Enciende el LED verde LD2


    /* Loop forever */
	while(1){

	}
	return 0;
}
