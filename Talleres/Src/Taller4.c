/*
 * Taller4.c
 *
 *  Created on: Mar 2, 2023
 *      Author: julian
 */

#include <stdint.h>

char var1 = 0;
int var2 = 0;
short var3 = 0;
long var4 = 0;

//Unsigned int 8 bits
uint8_t var5 = 0;
//Signed int 16 bits
int16_t var6 = 0;
//Unsigned int 32 bits
uint32_t var7 = 0;
//Int 64 bits
int64_t var8 = 0;
uint64_t var9 = 0;

int main(void){

	uint16_t testShift = 0b000011010110101;
	uint16_t testMask = 0b0000000000111111;

	while(1){
//			testShift = testShift >> 1;
//			testShift >>= 1;
//			testShift &= testMask;
//			testShift = testShift & testMask;
		testMask = testMask <<3;
		testMask = ~testMask;
		testShift = testShift & testMask;

	}

	return 0;
}



