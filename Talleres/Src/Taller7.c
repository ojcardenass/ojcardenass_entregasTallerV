/*
 * Taller7.c
 *
 *  Created on: Mar 22, 2023
 *      Author: julian
 */

/* Punteros
 *
 *Son como vectores
 *1-siempre son de 32 bits
 *
 *
 *Castin: pasar de un dato a otro
 *uint8_t var = 15;
 *uint16_t newVar = (uint16_t) var;
 *
 * */

#include <stdint.h>

int main(void){
	// ejemplo 1
	// Definimos varibles
	uint8_t dato = 124;
	// creamos un puntero con la direccion de memoria de la variable
	uint8_t *pDato = &dato;

	//Guardar el valor que esta en esa direccion de memoria
	uint8_t valorDato = *pDato;

	//Casteo basico
	uint16_t casteoDato = (uint16_t) dato;

	// Crear un puntero en una direccion especifica
	uint16_t *punteroDireccion = (uint16_t *) 0x20000001;

	// Cambiar direccion de memoria del puntero

	punteroDireccion = (uint16_t *) 0x20000002;

	// Guardar la direccion de memoria de un puntero
	uint32_t direccionDato = (uint32_t) pDato;

	// Cambiar el valor almacenado en el puntero
	*pDato = 200;

	// Aumentar 8 bits el puntero
	pDato ++;

	/* Arreglos
	 * Listas de datos del mismo tipo
	 *
	 * char
	 * Los arreglos son punteros al primer elemento del arreglo
	 * El puntero siempre esta en la posicion 0
	 *
	 * */
	#define sizeOfArray 4

	uint8_t miPrimerArreglo[sizeOfArray] = {5, 0xAE, 'a', 254};

	// 2.1 Recorrer un arreglo con ciclos
	uint8_t contenido = 0;

	for(uint8_t i=0; i<sizeOfArray; i++){
		contenido = miPrimerArreglo[i];
	}

	for(uint8_t i=0; i<sizeOfArray; i++){
		contenido = *(miPrimerArreglo+i);
	}

	// Cambiar elementos de un arreglo

	miPrimerArreglo[1] = 12;

	*(miPrimerArreglo + 1) = 12;

	// Estructuras

	// Definicion (plantilla de estructura)
	typedef struct{
		uint8_t elemento1;
		uint16_t elemento2;
		uint8_t arreglo[sizeOfArray];
	} miPrimeraEstructura;

	miPrimeraEstructura estructuraEjemplo = {0};

	// Acceder a los elementos de la estructura
	estructuraEjemplo.elemento1 = 'F';
	estructuraEjemplo.arreglo[2] = 10;

	// Crear un puntero que apunte a la estructura

	miPrimeraEstructura *ptrMiPrimeraEstructura = &estructuraEjemplo;
	// ->
	ptrMiPrimeraEstructura->elemento1 = 9;
	ptrMiPrimeraEstructura->arreglo[2] = 15;
	*((ptrMiPrimeraEstructura->arreglo)+2) = 15;


}
