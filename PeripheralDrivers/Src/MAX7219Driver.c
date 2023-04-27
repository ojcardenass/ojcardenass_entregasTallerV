/*
 * MAX7219Driver.c
 *
 *  Created on: Apr 22, 2023
 *      Author: julian
 */
#include <stm32f4xx.h>
#include "MAX7219Driver.h"

#define SET_NSS_LOW		ptrSPIHandler->SPIConfig.NSS = SPI_NSS_DISABLE;
#define SET_NSS_HIGH 	ptrSPIHandler->SPIConfig.NSS = SPI_NSS_ENABLE;

//void SET_NSS_LOW(SPI_Handler_t *ptrSPIHandler){
//	ptrSPIHandler->SPIConfig.NSS = SPI_NSS_DISABLE;
//}
//void SET_NSS_HIGH(SPI_Handler_t *ptrSPIHandler){
//	ptrSPIHandler->SPIConfig.NSS = SPI_NSS_ENABLE;
//}

void send_to_MAX7219(SPI_Handler_t *ptrSPIHandler,uint8_t addr, uint8_t data)
{
	SET_NSS_LOW;
	SPI_Send(ptrSPIHandler,addr);
	SPI_Send(ptrSPIHandler,data);
	SET_NSS_HIGH;
}

void clearDisplay_MAX7219(SPI_Handler_t *ptrSPIHandler){
	uint8_t i = 8;
	while (i){
		send_to_MAX7219(ptrSPIHandler,i, 0x00);
		i--;
	}
}

