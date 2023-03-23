/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************

 */

#include <stdint.h>

int main(void)
{
	while(1){
		// Operaciones básicas
			uint16_t varOperadores = 12;

			//Convertir decimal a binario
			uint8_t count = 60;
			uint8_t valor = 0;
			valor = (count & 0b111100);

			// Operaciones
			varOperadores = varOperadores +1;
			varOperadores ++;
			varOperadores += 1;

			// Modulo
			varOperadores = varOperadores % 2;
			varOperadores %= 2;

			//SHIFT

			uint8_t testVar = 0b1;

			testVar >>= 2;
			testVar <<= 1;

			testVar = testVar << 4;

			// Operadores Bitwise ( &,|,^ )

			uint32_t variableA = 0b10100001001;
			uint32_t variableB = 0b1;

			//OR
			uint8_t resultado = variableA | variableB;

			//AND
			resultado = variableA & variableB;
			//NOT
			resultado = ~resultado;
			//XOR
			resultado = variableA ^ variableB;

			//Varias Operaciones
			// << 2 = 00011100
			// ~ = 11100011
			resultado = ~(0b111 << 2);

			variableA &= resultado;

			variableA |= (0b11 << 6);

		//	Setting and Clearing of bits
		//


	}

	return 0;
}
