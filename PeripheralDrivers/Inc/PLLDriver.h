/*
 * PLLDriver.h
 *
 *  Created on: May 21, 2023
 *      Author: julian
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"
#include "SysTickDriver.h"

#define HSE_VALUE		8000000 // 8 MHz
#define SPEED_100MHz	1
#define SPEED_80MHz		2


void configPLL(int mcuspeed);
uint32_t getConfigPLL(void);

#endif /* PLLDRIVER_H_ */
