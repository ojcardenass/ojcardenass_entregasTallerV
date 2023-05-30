/*
 * @file		: LCDDriver.h
 *
 */


#ifndef LCDDRIVER_H_
#define LCDDRIVER_H_

#include <stm32f4xx.h>
#include "I2CxDriver.h"
#include "SysTickDriver.h"

void InitLCD(I2C_Handler_t *ptrHandlerI2C);
void CMD_toLCD(I2C_Handler_t *ptrHandlerI2C, char cmd);
void sendDataLCD(I2C_Handler_t *ptrHandlerI2C, char data);
void sendMsgLCD(I2C_Handler_t *ptrHandlerI2C, char *str);
void cleanLCD(I2C_Handler_t *ptrHandlerI2C);
void moveCursorLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);
void ResetScreenLCD(I2C_Handler_t *ptrHandlerI2C);
void writeLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);


#endif /* LCDDRIVER_H_ */

