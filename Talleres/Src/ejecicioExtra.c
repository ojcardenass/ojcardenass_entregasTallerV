/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Oscar Julian Cardenas Sosa
 * @brief          : Main program body
 ******************************************************************************

 */

#include <stdint.h>

int main(void){
	//0 -> Masculino
	//1 -> Femenino
	//Set1
	uint16_t faceSet1 = 0b0000011011001110;
	//Set2
	uint16_t faceSet2 = 0b1101100001000111;

	//Punto A
	uint16_t faceSet1A = ~faceSet1;
	uint16_t faceSet2A = ~faceSet2;

	//Punto B
	//Mascara Set 1 Femenino Lentes
	uint16_t maskSet1B = 0b0000010000000000;
	uint16_t testMask1B = 0b0 << 9;
	//Mascara Set 2 Femenino Lentes
	uint16_t maskSet2B = 0b0101000000000000;
	uint16_t testMask1B = 0b101 << 11;

	uint16_t faceSet1B = faceSet1 & maskSet1B;
	uint16_t faceSet2B = faceSet2 & maskSet2B;

	//Punto C


	//Punto D





	while(1){

	}
