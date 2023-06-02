/*
 * SPIxDriver.h
 *
 *  Created on: Apr 19, 2023
 *      Author: julian
 */

#include <stdio.h>
#include "GPIOxDriver.h"

#ifndef SPIXDRIVER_H_
#define SPIXDRIVER_H_

// spi configuration macros
	// @SPI_devicemode
#define SPI_DEVICE_MODE_MASTER				1
#define SPI_DEVICE_MODE_SLAVE				0
	// @SPI_busconfig
#define SPI_BUS_CONFIG_FD					1 //Full Duplex
#define SPI_BUS_CONFIG_HD					2 // Half Duplex
#define SPI_BUS_CONFIG_TX					3 // Tx Only
	//@SPI_ClockConfig
#define SPI_SCLK_SPEED_DIV2					0
#define SPI_SCLK_SPEED_DIV4					1
#define SPI_SCLK_SPEED_DIV8					2
#define SPI_SCLK_SPEED_DIV16				3
#define SPI_SCLK_SPEED_DIV32				4
#define SPI_SCLK_SPEED_DIV64				5
#define SPI_SCLK_SPEED_DIV128				6
#define SPI_SCLK_SPEED_DIV256				7
	//@SPI_DATA_SIZE
#define SPI_DATA_SIZE_8BITS					0
#define SPI_DATA_SIZE_16BITS				1

//SPI MODE CONTROL
/* 		CLKPol	CLKPhase
MODO0	0		0
MODO1	0		1
MODO2	1		0
MODO3	1		1
*/
#define SPI_MODE0							0
#define SPI_MODE1							1
#define SPI_MODE2							2
#define SPI_MODE3							3
// Frame format LSB o MSB transmitted first
#define SPI_LSBFIRST						1
#define SPI_MSBFIRST						0
	//@SPI_Status MODE
#define SPI_DISABLE							0
#define SPI_ENABLE							1
	//SPI_NSS
#define SPI_NSS_ENABLE						1
#define SPI_NSS_DISABLE						0
	//SPI_CRC
#define SPI_CRC_ENABLE						1
#define SPI_CRC_DISABLE						0
	//SPI_TIMode
#define SPI_TIMODE_ENABLE					1
#define SPI_TIMODE_DISABLE					0
	//SPI_FrameFormat
#define SPI_MOTOROLA_MODE					0
#define SPI_TI_MODE							1

/*
 * Configuration structure for spi peripheral
 * */

typedef struct{
	uint8_t	DeviceMode;			// Master o Slave
	uint8_t	DirectionBusConfig;	// Specifies the SPI bidirectional mode state. @SPI_BusConfiguration
	uint8_t BaudRatePrescaler;	// Clock Speed
	uint8_t DataSize;			// Data Frame Format
	uint8_t FrameFormat;		// SPI Frame Format Motorola / TI
	uint8_t SPIMode;			// SPI Mode
	uint8_t FirstBit;			// LSB o MSB transmitted first
	uint8_t NSS;				// Specifies whether the NSS signal is managed by hardware (NSS pin) or by software using the SSI bit.
	uint8_t State;				// SPI Enable / Disable
	uint8_t TIMode;				// TI Mode Enable / Disable (Error when NSS pulse occurs in transmission)
	uint8_t CRCCalculation;		// CRC Enable / Disable
	uint8_t CRCPolinomial;		// Polynomial used for CRC, This parameter must be an odd number between Min_Data = 1 and Max_Data = 65535
}SPI_Config_t ;

/*
 * Handle structure for SPIx peripheral
 * */

typedef struct{
	SPI_TypeDef    *ptrSPIx	 ;	// this holds the base address of spix(1 ,2,3,4) peripheral
	SPI_Config_t   SPIConfig ;	// SPI communication parameters
	GPIO_Handler_t	NSS_Pin	 ;  // NSS Pin status selection
	uint8_t		   *pTxBuffer;	// To store the app tx buffer address
	uint8_t		   *pRxBuffer;	// To store the app rx buffer address
	uint32_t	    TxSize   ;	// To store the Tx Size
	uint32_t	    RxSize   ;	// To store the Rx Size
	uint8_t	    	TxState  ;	// To store the Tx state
	uint8_t	    	RxState  ;	// To store the rx state
}SPI_Handler_t;

/* Definicion de los prototipos para las funciones del SPI */
void SPI_Config(SPI_Handler_t *ptrSPIHandler);
// Spi send and receive apis
void SPI_Send(SPI_Handler_t *ptrSPIHandler, uint8_t TxBuffer) ;
void SPI_Read(SPI_Handler_t *ptrSPIHandler , uint8_t RxBuffer ) ;

void NSS_LOW(SPI_Handler_t *ptrSPIHandler);
void NSS_HIGH(SPI_Handler_t *ptrSPIHandler);


#endif /* SPIXDRIVER_H_ */
