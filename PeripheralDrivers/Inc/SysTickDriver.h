/*
 * SysTickDriver.h
 *
 *  Created on: May 12, 2023
 *      Author: julian
 */

#ifndef SYSTICKDRIVER_H_
#define SYSTICKDRIVER_H_

#include <stm32f4xx.h>
#include "PLLDriver.h"

//#define HSI_16MHz							0
//#define HSE_16MHz							1

#define SYSTICK_LOAD_VALUE_16MHz_1ms		16000		// Numero de ciclos en 1 ms
#define SYSTICK_LOAD_VALUE_100MHz_1ms		100000		// Numero de ciclos en 1 ms
#define SYSTICK_LOAD_VALUE_80MHz_1ms		80000

void config_SysTick_ms(int systemClock);
uint64_t getTicks_ms(void);
void delay_ms(uint32_t wait_time_ms);

#endif /* SYSTICKDRIVER_H_ */
