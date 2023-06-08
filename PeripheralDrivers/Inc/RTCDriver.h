/*
 * RTCDriver.h
 *
 *  Created on: Jun 8, 2023
 *      Author: julian
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include "stm32f4xx.h"

#define KEY1			0xCA
#define KEY2			0x53

#define PRE_A			127
#define PRE_S			255

#define FORBIDDEN	0
#define MONDAY		1
#define THURSDAY	2
#define WEDNESDAY	3
#define TUESDAY		4
#define FRIDAY		5
#define SATURDAY	6
#define SUNDAY		7


typedef struct
{
	uint8_t			hours;
	uint8_t			minutes;
	uint8_t 		seconds;
	uint8_t			year;
	uint8_t 		weekDay;
	uint8_t 		month;
	uint8_t 		day;

}RTC_Handler_t;

void rtc_config (RTC_Handler_t *ptrHandlerRtc);

uint8_t getHours (RTC_Handler_t prtHandlerRTC);
uint8_t getMinutes(RTC_Handler_t prtHandlerRTC);
uint8_t getSeconds (RTC_Handler_t prtHandlerRTC);
uint8_t getYears (RTC_Handler_t ptrHandlerRTC);
uint8_t getDays (RTC_Handler_t prtHandlerRTC);
uint8_t getMonths (RTC_Handler_t prtHandlerRTC);
uint8_t getWedn (RTC_Handler_t ptrHandlerRTC);
void updateDate (RTC_Handler_t *ptrHandlerRTC ,uint8_t newYear, uint8_t newMonth, uint8_t newDay);


#endif /* RTCDRIVER_H_ */
