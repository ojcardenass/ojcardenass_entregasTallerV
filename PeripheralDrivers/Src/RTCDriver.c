/*
 * RTCDriver.c
 *
 *  Created on: Jun 8, 2023
 *      Author: julian
 */

#include "RTCDriver.h"

void rtc_config (RTC_Handler_t *ptrHandlerRtc){
	// Encendemos la señal de reloj
	RCC -> APB1ENR |= RCC_APB1ENR_PWREN;

	// Es necesario activar Disable Backup Domain write protection   para habilitar el acceso de escritura
	PWR -> CR |= PWR_CR_DBP;

	// Activamos el LSE (Cristal de 32kHz de la board)*/
	RCC -> BDCR |= RCC_BDCR_RTCEN;    // Activar el RTC Clock
	RCC -> BDCR |= RCC_BDCR_LSEON;
	RCC -> BDCR &= ~(RCC_BDCR_RTCSEL);
	RCC -> BDCR |= RCC_BDCR_RTCSEL_0; // Seleccionamos el LSE para RTC

	// Modo inicialización
	RTC -> ISR |= RTC_ISR_INIT;

	// Escribimos las claves para habilitar el acceso de escritura a los registros del RTC
	RTC -> WPR |= KEY1;
	RTC -> WPR |= KEY2;

	// Activamos el INITF bit para permitir las actualizaciones del calendario
	while(!(RTC -> ISR & RTC_ISR_INITF)){
		__NOP();
	}

	// Preescaler síncrono, factor de división 255
	RTC -> PRER |= PRE_S << RTC_PRER_PREDIV_S_Pos;

	// Preescaler asíncrono, factor de división 128
	RTC -> PRER |= PRE_A << RTC_PRER_PREDIV_A_Pos;

	// Formato hora 24H
	RTC -> TR &= ~ RTC_TR_PM;
	RTC -> CR &= ~ RTC_CR_FMT;

	// Para omitir los registros sombra
	RTC -> CR |= RTC_CR_BYPSHAD;

	// Limpiamos los registros
	RTC -> TR = 0;
	RTC -> DR = 0;

	// Horas
	RTC -> TR |= (((ptrHandlerRtc -> hours) / 10) << RTC_TR_HT_Pos);
	RTC -> TR |= (((ptrHandlerRtc -> hours) % 10) << RTC_TR_HU_Pos);

	// Minutos
	RTC -> TR |= (((ptrHandlerRtc -> minutes) / 10) << RTC_TR_MNT_Pos);
	RTC -> TR |= (((ptrHandlerRtc -> minutes) % 10) << RTC_TR_MNU_Pos);

	// Segundos
	RTC -> TR |= ((ptrHandlerRtc -> seconds) / 10 << RTC_TR_ST_Pos);
	RTC -> TR |= ((ptrHandlerRtc -> seconds) % 10 << RTC_TR_SU_Pos);

	// Año
	RTC -> DR |= ((ptrHandlerRtc -> year) / 10 << RTC_DR_YT_Pos);
	RTC -> DR |= ((ptrHandlerRtc -> year) % 10 << RTC_DR_YU_Pos);

	// Semanas
	RTC -> DR |= (ptrHandlerRtc -> weekDay << RTC_DR_WDU_Pos);

	// Mes
	RTC -> DR |= ((ptrHandlerRtc -> month) / 10 << RTC_DR_MT_Pos);
	RTC -> DR |= ((ptrHandlerRtc -> month) % 10 << RTC_DR_MU_Pos);

	// Día
	RTC -> DR |= ((ptrHandlerRtc -> day) / 10 << RTC_DR_DT_Pos);
	RTC -> DR |= ((ptrHandlerRtc -> day) % 10 << RTC_DR_DU_Pos);


	// Salimos del modo inicialización
	RTC -> ISR &= ~ RTC_ISR_INIT;
}

uint8_t getHours(RTC_Handler_t prtHandlerRTC){
	uint8_t hour = 0;
	uint32_t hrsT = 0, hrsU = 0;

	hrsT = RTC -> TR & RTC_TR_HT;
	hrsT >>= RTC_TR_HT_Pos;

	hrsU = RTC -> TR & RTC_TR_HU;
	hrsU >>= RTC_TR_HU_Pos;

	hour = (hrsT * 10) + hrsU;
	return hour;
}

uint8_t getMinutes(RTC_Handler_t prtHandlerRTC){
	uint8_t minute = 0;
	uint32_t mntT = 0, mntU = 0;

	mntT = RTC -> TR & RTC_TR_MNT;
	mntT >>= RTC_TR_MNT_Pos;

	mntU = RTC -> TR & RTC_TR_MNU;
	mntU >>= RTC_TR_MNU_Pos;

	minute = (mntT * 10) + mntU;
	return minute;
}

uint8_t getSeconds(RTC_Handler_t prtHandlerRTC){
	uint8_t second = 0;
	uint32_t scdT = 0, scdU = 0;

	scdT = RTC -> TR & RTC_TR_ST;
	scdT >>= RTC_TR_ST_Pos;

	scdU = RTC -> TR & RTC_TR_SU;
	scdU >>= RTC_TR_SU_Pos;

	second = (scdT * 10) + scdU;
	return second;

}


uint8_t getYears (RTC_Handler_t ptrHandlerRTC){
	uint8_t year = 0;
	uint32_t yrT = 0, yrU = 0;

	yrT = RTC -> DR & RTC_DR_YT;
	yrT >>= RTC_DR_YT_Pos;

	yrU = RTC -> DR & RTC_DR_YU;
	yrU >>= RTC_DR_YU_Pos;

	year = (yrT * 10) + yrU;
	return year;
}


uint8_t getMonths(RTC_Handler_t prtHandlerRTC){
	uint8_t month = 0;
	uint32_t mthT = 0, mthU = 0;

	mthT = RTC -> DR & RTC_DR_MT;
	mthT >>= RTC_DR_MT_Pos;

	mthU = RTC -> DR & RTC_DR_MU;
	mthU >>= RTC_DR_MU_Pos;

	month = (mthT * 10) + mthU;
	return month;
}


uint8_t getDays(RTC_Handler_t prtHandlerRTC){
	uint8_t day = 0;
	uint32_t dyT = 0, dyU = 0;

	dyT = RTC -> DR & RTC_DR_DT;
	dyT >>= RTC_DR_DT_Pos;

	dyU = RTC -> DR & RTC_DR_DU;
	dyU >>= RTC_DR_DU_Pos;

	day = (dyT * 10) + dyU;
	return day;
}


uint8_t getWedn (RTC_Handler_t ptrHandlerRTC){
	uint8_t wed = 0;
	uint32_t wd = 0;

	wd = RTC -> DR & RTC_DR_WDU;
	wed = wd >>= RTC_DR_WDU_Pos;

	return wed;
}


void updateDate (RTC_Handler_t *ptrHandlerRtc ,uint8_t newYear, uint8_t newMonth, uint8_t newDay){

	ptrHandlerRtc -> year = newYear;
	ptrHandlerRtc -> month = newMonth;
	ptrHandlerRtc -> day = newDay;

	// Año
	RTC -> DR |= ((ptrHandlerRtc -> year) / 10 << RTC_DR_YT_Pos);
	RTC -> DR |= ((ptrHandlerRtc -> year) % 10 << RTC_DR_YU_Pos);
	// Mes
	RTC -> DR |= ((ptrHandlerRtc -> month) / 10 << RTC_DR_MT_Pos);
	RTC -> DR |= ((ptrHandlerRtc -> month) % 10 << RTC_DR_MU_Pos);
	// Día
	RTC -> DR |= ((ptrHandlerRtc -> day) / 10 << RTC_DR_DT_Pos);
	RTC -> DR |= ((ptrHandlerRtc -> day) % 10 << RTC_DR_DU_Pos);

}


