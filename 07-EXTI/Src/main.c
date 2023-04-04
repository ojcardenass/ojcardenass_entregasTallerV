/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Oscar Julián Cárdenas Sosa
 * @brief          : Main program body
 ******************************************************************************

 */

#include <stdint.h>
#include <stm32f4xx.h>

#include "GPIOxDriver.h"

//Definicion de elementos
uint32_t counterG2 = 0;
uint32_t auxVariableG2 = 0;

uint32_t *ptr_CounterG2;

uint8_t byteVariableG2;
uint8_t *ptr_ByteVariableG2;


int main(void){

	/* Trabajando con las variables y los punteros */
	counterG2 = 3456789; 	//Asignar un valor a la variable
	auxVariableG2 = counterG2;	// C es pasado por valor

	ptr_CounterG2 = &counterG2;	//Pasando la posicion de memoria del counterG2

	*ptr_CounterG2 = 9876543;	//Escribimos

	ptr_CounterG2++;			// Movemos una posicion , es decir 4 bytes
	*ptr_CounterG2 = 9876543;	// Escribimos de nuevo en la nueva posicion

	byteVariableG2 = 234;
	ptr_ByteVariableG2 = &byteVariableG2;
	*ptr_ByteVariableG2 = 87;

	//ptr_ByteVariableG2 = &counterG2;
	ptr_CounterG2 = &counterG2;
	auxVariableG2 = (uint32_t) ptr_CounterG2;

	ptr_ByteVariableG2 = (uint8_t *)auxVariableG2;
	*ptr_ByteVariableG2 = 2;


	while(1){

	}
	return 0;
}
