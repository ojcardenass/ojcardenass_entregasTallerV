/*
 * MAX7219Driver.h
 *
 *  Created on: Apr 22, 2023
 *      Author: julian
 */
#include <stdio.h>
#include "SPIxDriver.h"

#ifndef MAX7219DRIVER_H_
#define MAX7219DRIVER_H_

//ADDRESSES
#define NO_OP			0x00
//DIGITOS
#define DIGIT0			0x01
#define DIGIT1			0x02
#define DIGIT2			0x03
#define DIGIT3			0x04
#define DIGIT4			0x05
#define DIGIT5			0x06
#define DIGIT6			0x07
#define DIGIT7			0x08
//FUNCIONES
#define DECO_MODE		0x09
#define INTENSITY		0x0A // NUMERO DEL 1 AL 15
#define SCAN_LIMIT		0x0B
#define SHUTDOWN		0x0C
#define DISPLAY_TEST	0x0F
//SHUTDOWN
#define SHTDOWN			0
#define NORMAL			1
//DECODE MODE
#define	NO_DECO			0x00
// SCAN LIMIT
#define ALL				0x07 // Todos los 8 digitos
//DISPLUA TEST
#define NO_TEST			0
#define TEST			1

void send_to_MAX7219(SPI_Handler_t *ptrSPIHandler,uint8_t reg, uint8_t data);
void clearDisplay_MAX7219(SPI_Handler_t *ptrSPIHandler);

#endif /* MAX7219DRIVER_H_ */
