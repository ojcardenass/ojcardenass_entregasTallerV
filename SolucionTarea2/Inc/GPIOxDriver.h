/*
 * GPIOxDriver.h
 *
 *  Created on: Mar 10, 2023
 *      Author: julian
 *
 * Este programa se encarga de las definiciones relacionadas unicamente con el debido manejo y control del
 * periferico GPIOx (funciones del controlador)
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

// Incluyendo este archivo estamos incluyendo el correspondiente al GPIOx
#include "stm32f411xx_hal.h"

typedef struct
{
	uint8_t GPIO_PinNumber;		//PIN con el que deseamos trabajar
	uint8_t GPIO_PinMode;			//Modo de la configuracion: entrada, salida, analogo, f. alternativa
	uint8_t GPIO_PinSpeed;			//Velocidad de respuesta del PIN (solo para digital)
	uint8_t GPIO_PinPuPdControl;	//Seleccionamos si deseamos una salida Pull-up, Pull-down o "libre"
	uint8_t GPIO_PinOPType;		//Trabaja de la mano con el anterior, selecciona salida PUPD o OpenDrain
	uint8_t GPIO_PinAltFunMode;	//Selecciona cual es la funcion alternativa que se esta configurando

}GPIO_PinConfig_t;

/*Esta es una estructura que contiene dos elementos:
 * -La direccion del puerto que se esta utilizando(la referencia al puerto)
 * -La configuracion especifica del PIN que se esta utilizando
 *  */

typedef struct
{
	GPIOx_RegDef_t		*pGPIOx;		/*!<Direccion del puerto al que el PIN corresponde>*/
	GPIO_PinConfig_t	GPIO_PinConfig;	/*!<Configuracion del PIN>*/
}GPIO_Handler_t;

/*Definicion de las cabeceras de las funciones del GPIOxDriver*/
void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);
void GPIOxTooglePin(GPIO_Handler_t *pPinHandler);


#endif /* GPIOXDRIVER_H_ */
