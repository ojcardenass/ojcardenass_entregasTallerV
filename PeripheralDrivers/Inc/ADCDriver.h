/*
 * AdcDriver.h
 *
 *  Created on: Month XX, 2022
 *      Author: ojcardenass
 */

#ifndef INC_ADCDRIVER_H_
#define INC_ADCDRIVER_H_

#include "stm32f4xx.h"

#define ADC_CHANNEL_0		0
#define ADC_CHANNEL_1		1
#define ADC_CHANNEL_2		2
#define ADC_CHANNEL_3		3
#define ADC_CHANNEL_4		4
#define ADC_CHANNEL_5		5
#define ADC_CHANNEL_6		6
#define ADC_CHANNEL_7		7
#define ADC_CHANNEL_8		8
#define ADC_CHANNEL_9		9
#define ADC_CHANNEL_10		10
#define ADC_CHANNEL_11		11
#define ADC_CHANNEL_12		12
#define ADC_CHANNEL_13		13
#define ADC_CHANNEL_14		14
#define ADC_CHANNEL_15		15
#define ADC_CHANNEL_16		16

#define ADC_RESOLUTION_12_BIT	0
#define ADC_RESOLUTION_10_BIT	1
#define ADC_RESOLUTION_8_BIT	2
#define ADC_RESOLUTION_6_BIT	3

#define ADC_ALIGNMENT_RIGHT		0
#define ADC_ALIGNMENT_LEFT		1

#define ADC_SAMPLING_PERIOD_3_CYCLES	0b000;
#define ADC_SAMPLING_PERIOD_15_CYCLES	0b001;
#define ADC_SAMPLING_PERIOD_28_CYCLES	0b010;
#define ADC_SAMPLING_PERIOD_56_CYCLES	0b011;
#define ADC_SAMPLING_PERIOD_84_CYCLES	0b100;
#define ADC_SAMPLING_PERIOD_112_CYCLES	0b101;
#define ADC_SAMPLING_PERIOD_144_CYCLES	0b110;
#define ADC_SAMPLING_PERIOD_480_CYCLES	0b111;

#define EXTERNAL_INTERRUPT_FALLING_EDGE 0
#define EXTERNAL_INTERRUPT_RISING_EDGE 	1

#define TIM1_CH1						0b0000;
#define TIM1_CH2						0b0001;
#define TIM1_CH3						0b0010;
#define TIM2_CH2						0b0011;
#define TIM2_CH3						0b0100;
#define TIM2_CH4						0b0101;
#define TIM2_TRGO						0b0110;
#define TIM3_CH1						0b0111;
#define TIM1_TRGO						0b1000;
#define TIM4_CH4						0b1001;
#define TIM5_CH1						0b1010;
#define TIM5_CH2						0b1011;
#define TIM5_CH3						0b1100;
#define EXTI_11		 					0b1111;

typedef struct
{
	uint8_t		channel;		// Canal ADC que será utilizado para la conversión ADC
	uint8_t		resolution;		// Precisión con la que el ADC hace la adquisición del dato
	uint16_t	samplingPeriod;	// Tiempo deseado para hacer la adquisición del dato
	uint8_t		dataAlignment;	// Alineación a la izquierda o a la derecha
	uint16_t	adcData;			//Dato de la conversión
	uint16_t	eventExt;
	uint8_t		flank;
	uint8_t		channels[];

}ADC_Config_t;

void adc_Config(ADC_Config_t *adcConfig);
void configAnalogPin(uint8_t adcChannel);
void adcComplete_Callback(void);
void startSingleADC(void);
void startContinousADC(void);
uint16_t getADC(void);
void adcExternalConfig(ADC_Config_t *adcConfig);
void multiChannelConfig(ADC_Config_t *adcConfig, uint8_t numCan);

//void ADC_ConfigMultichannel (ADC_Config_t *adcConfig, uint8_t numeroDeCanales);

#endif /* INC_ADCDRIVER_H_ */
