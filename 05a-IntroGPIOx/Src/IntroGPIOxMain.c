/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Oscar Julián Cárdenas Sosa
 * @brief          : Archivo principal (main)
 ******************************************************************************
 *
 *
 * Con este programa se desea mostrar el uso basico de los registros que controlan
 * al Micro (SFR) y la forma adecuada para utilizar los operadores &,|,~ y =, para
 * cambiar la configuracion de algun registro
 * Tambien es importante para entender la utilidad de los numero BIN y HEX
 *
 * Es necesario tener a la mano tanto el manual de referencia del micro como la hoja de
 * datos del micro (ambos son documentos diferentes y complementarios le uno del otro)
 *
 * HAL -> Hardware Abstraction Layer
 *
 * Este programa introduce el periferico mas simple que tiene el micro, que es el encargado
 * de manejar los pines de cada puerto
 *
 * Cada PINx de cada puerto GPIO puede desarrollar funciones basicas de  tipo entrada
 * y salida de datos digitales, ademas se les puede asignar funciones
 * especiales que generalmente estan ligadas a otro periferico adicional (se vera mas adelante).
 *
 * De igual forma, varias caracteristicas pueden ser configuradas para cada PINx
 * especifico como son:
 * - la velocidad a la que puede responder
 * - tipo de entrada (pull up, pull down, open draina ( flotante))
 * - tipo de salida (push-pull, open drain...)
 * - entrada analoga
 *
 * Debemos definir entonces todos los registros que manejan el periferico GPIOx y luego
 * crear algunas funciones para utilizar adecuadamente el equipo.
 *
 ******************************************************************************
 */

#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

uint32_t valor=0;

/* funcion principal del programa. Es aca donde se ejecuta todo */
int main(void){

	// ***************
	//Definimos el handler para el PIN que deseamos configurar
	GPIO_Handler_t handlerUserLedPin = {0};

	//Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna funcion

	//Cargamos la configuracion del PIN especifico
	GPIO_Config(&handlerUserLedPin);

	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);

	valor=GPIO_ReadPin(&handlerUserLedPin);

	//Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){
		NOP();
	}

}
