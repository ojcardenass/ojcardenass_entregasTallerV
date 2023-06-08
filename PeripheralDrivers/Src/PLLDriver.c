/*
 * PLLDriver.c
 *
 *  Created on: May 21, 2023
 *      Author: julian
 */

#include "PLLDriver.h"

uint32_t systemClock = 0;

void configPLL(int mcuspeed){
	switch(mcuspeed){
	case 1:
		/* MCU Speed 100MHz*/
		// Activamos la señal de reloj del Power Controller
		RCC -> APB1ENR |= RCC_APB1ENR_PWREN;
		// Regulator voltage scaling output selection
		// Para trabajar con velocidades >= 84 MHz y <= 100MHz
		PWR -> CR &= ~ PWR_CR_VOS;
		PWR -> CR |= PWR_CR_VOS;
		// Se modifica la latencia de la memoria para 100 MHz, 4 ciclos de reloj
		// Y se activan las configuraciones relacionadas
		FLASH -> ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
		FLASH -> ACR &= ~ FLASH_ACR_LATENCY;
		FLASH -> ACR |= FLASH_ACR_LATENCY_3WS;
		/* Por defecto la fuente para el PLL es el HSI de 16MHz*/
		// m = 8, 16 MHz / 8 = 2 MHz
		RCC -> PLLCFGR &= ~ RCC_PLLCFGR_PLLM;
		RCC -> PLLCFGR |= RCC_PLLCFGR_PLLM_3;
		// Se multiplica por un factor y sale 200 MHz
		RCC -> PLLCFGR &= ~ RCC_PLLCFGR_PLLN;
		RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_2;
		RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_5;
		RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_6;
		// p = 2, 200 MHz / 2 = 100 MHz
		RCC -> PLLCFGR &= ~ RCC_PLLCFGR_PLLP;

		// AHB prescaler, no se divide, ya que puede trabajar a 80 MHz
		RCC -> CFGR &= ~ RCC_CFGR_HPRE;
		RCC -> CFGR |= RCC_CFGR_HPRE_DIV1;
		// APB1 prescaler, se divide por 2 ya que máximo trabaja a 50 MHz, queda en 40 MHz
		RCC -> CFGR &= ~ RCC_CFGR_PPRE1;
		RCC -> CFGR |= RCC_CFGR_PPRE1_DIV2;
		// APB2 prescaler, no se divide, ya que puede trabajar a 80 MHz
		RCC -> CFGR &= ~ RCC_CFGR_PPRE2;
		RCC -> CFGR |= RCC_CFGR_PPRE2_DIV1;
//		// PLL clock selected
//		RCC -> CFGR |= RCC_CFGR_MCO1;
		// HSI clock selected
		RCC -> CFGR &= ~RCC_CFGR_MCO1;
		// Factor de división (prescaler) = 5, señal de salida 20 MHz, para poder observar en osciloscopio
		RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
		RCC -> CFGR |= RCC_CFGR_MCO1PRE;
		// Main PLL enable
		RCC -> CR |= RCC_CR_PLLON;
		// Bandera del PLL, mientras está desbloqueado no haga nada
		while (!(RCC -> CR & RCC_CR_PLLRDY)){
			__NOP();
		}
		// Trabajamos con PLL
		RCC -> CFGR &= ~ RCC_CFGR_SW;
		RCC -> CFGR |= RCC_CFGR_SW_PLL;
		break;
	case 2:
		/* MCU Speed 80MHz*/
		// Activamos la señal de reloj del Power Controller
		RCC -> APB1ENR |= RCC_APB1ENR_PWREN;
		// Regulator voltage scaling output selection
		// Para trabajar con velocidades <= 84 MHz
		PWR -> CR &= ~ PWR_CR_VOS;
		PWR -> CR |= ((0x2) << PWR_CR_VOS_Pos);
		// Se modifica la latencia de la memoria para 80 MHz, 3 ciclos de reloj
		// Y se activan las configuraciones relacionadas
		FLASH -> ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
		FLASH -> ACR &= ~ FLASH_ACR_LATENCY;
		FLASH -> ACR |= FLASH_ACR_LATENCY_2WS;
		// m = 8, 16 MHz / 8 = 2 MHz
		RCC -> PLLCFGR &= ~ RCC_PLLCFGR_PLLM;
		RCC -> PLLCFGR |= RCC_PLLCFGR_PLLM_3;
		// Se multiplica por un factor y sale 160 MHz
		RCC -> PLLCFGR &= ~ RCC_PLLCFGR_PLLN;
		RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_4;
		RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_6;
		// p = 2, 160 MHz / 2 = 80 MHz
		RCC -> PLLCFGR &= ~ RCC_PLLCFGR_PLLP;

		// AHB prescaler, no se divide, ya que puede trabajar a 100 MHz
		RCC -> CFGR &= ~ RCC_CFGR_HPRE;
		RCC -> CFGR |= RCC_CFGR_HPRE_DIV1;
		// APB1 prescaler, se divide por 2 ya que máximo trabaja a 50 MHz
		RCC -> CFGR &= ~ RCC_CFGR_PPRE1;
		RCC -> CFGR |= RCC_CFGR_PPRE1_DIV2;
		// APB2 prescaler, no se divide, ya que puede trabajar a 100 MHz
		RCC -> CFGR &= ~ RCC_CFGR_PPRE2;
		RCC -> CFGR |= RCC_CFGR_PPRE2_DIV1;
		// PLL clock selected
		RCC -> CFGR |= RCC_CFGR_MCO1;
		// Factor de división (prescaler) = 5, señal de salida 16 MHz, para poder observar en osciloscopio
		RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
		RCC -> CFGR |= RCC_CFGR_MCO1PRE;

		// Main PLL enable
		RCC -> CR |= RCC_CR_PLLON;
		// Bandera del PLL, mientras está desbloqueado no haga nada
		while (!(RCC -> CR & RCC_CR_PLLRDY)){
			__NOP();
		}
		// Configuramos el PLL como clock del sistema
		RCC -> CFGR &= ~ RCC_CFGR_SW;
		RCC -> CFGR |= RCC_CFGR_SW_PLL;
		break;
	default:
		__NOP();
	}
}

uint32_t getConfigPLL(void) {
    // Se leen y se extraen los valores de configuracion del
    uint32_t pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
    uint32_t plln = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos;

    // Se calcula el valor actual del clock
    if(pllm == 16){
    	systemClock = (HSI_VALUE);
    }
    else{
    	systemClock = ((HSI_VALUE / pllm) * plln) / 2;
    }
    return systemClock;
}
