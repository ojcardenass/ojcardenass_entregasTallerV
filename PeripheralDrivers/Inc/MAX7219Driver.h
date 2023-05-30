/*
 * MAX7219Driver.h
 *
 *  Created on: Apr 22, 2023
 *      Author: julian
 */
#include <stdio.h>
#include "stm32f4xx.h"
#include "SPIxDriver.h"
#include "GPIOxDriver.h"

#ifndef MAX7219DRIVER_H_
#define MAX7219DRIVER_H_
/* Definicion de las direcciones y valores posibles*/
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
#define NO_TEST			0x00
#define TEST			0x01

void send_to_MAX7219(uint8_t reg, uint8_t data);
void clearDisplay(void);
void MAX7219_Init(void);
void displayTest(void);
void displayNOTest(void);
void noDeco(void);
void setIntensity(uint8_t intensity);
void scanLimit(void);
void shutdown(int normal);

void sendMatrix1(uint8_t addr, uint8_t data);
void sendMatrix2(uint8_t addr, uint8_t data);
void sendMatrix3(uint8_t addr, uint8_t data);
void sendMatrix4(uint8_t addr, uint8_t data);
void sendNO_OP(void);

void rowOut(uint8_t* values, uint8_t* ledBuffer);
void setColumnsModx(uint8_t* values, uint8_t module);

#endif /* MAX7219DRIVER_H_ */
