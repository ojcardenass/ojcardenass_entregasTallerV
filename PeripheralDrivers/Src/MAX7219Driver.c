/*
 * MAX7219Driver.c
 *
 *  Created on: Apr 22, 2023
 *      Author: julian
 */
#include "stm32f4xx.h"
#include "MAX7219Driver.h"

SPI_Handler_t HandlerSPI;

#define SET_NSS_LOW		HandlerSPI.SPIConfig.NSS = SPI_NSS_DISABLE;
#define SET_NSS_HIGH 	HandlerSPI.SPIConfig.NSS = SPI_NSS_ENABLE;


void send_to_MAX7219(uint8_t addr, uint8_t data){
	SET_NSS_LOW;
	SPI_Send(&HandlerSPI,addr);
	SPI_Send(&HandlerSPI,data);
	SET_NSS_HIGH;
}

void clearDisplay_MAX7219(void){
	uint8_t i = 8;
	while (i){
		send_to_MAX7219(i, 0x00);
		i--;
	}
}

