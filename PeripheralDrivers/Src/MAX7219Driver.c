/*
 * MAX7219Driver.c
 *
 *  Created on: Apr 22, 2023
 *      Author: julian
 */
#include "stm32f4xx.h"
#include "MAX7219Driver.h"
#include "SysTickDriver.h"

SPI_Handler_t handlerSPI		= {0};
GPIO_Handler_t handlerCS		= {0};


void MAX7219_Init(void){

	/* Configuramos el SysTick*/
	config_SysTick_ms(SYSTICK_LOAD_VALUE_16MHz_1ms);

	// Configuración del pin NSS
	handlerCS.pGPIOx									= GPIOB;
	handlerCS.GPIO_PinConfig.GPIO_PinNumber				= PIN_12;
	handlerCS.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerCS.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	// Configuracion normal
	handlerCS.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerCS.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	// PRUEBA SSOE
//	handlerCS.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerCS.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
//	handlerCS.GPIO_PinConfig.GPIO_PinAltFunMode			= AF5;
    //Cargando la configuracion en los registros
	GPIO_Config(&handlerCS);

	// Configuracion SPI
	handlerSPI.ptrSPIx										= SPI2;
	handlerSPI.NSS_Pin										= handlerCS;
	handlerSPI.SPIConfig.DeviceMode							= SPI_DEVICE_MODE_MASTER;
	handlerSPI.SPIConfig.DirectionBusConfig					= SPI_BUS_CONFIG_TX;
	handlerSPI.SPIConfig.BaudRatePrescaler					= SPI_SCLK_SPEED_DIV4;
	handlerSPI.SPIConfig.DataSize							= SPI_DATA_SIZE_8BITS;
	handlerSPI.SPIConfig.SPIMode							= SPI_MODE0;
	handlerSPI.SPIConfig.FirstBit							= SPI_MSBFIRST;
	handlerSPI.SPIConfig.NSS								= SPI_NSS_ENABLE;
	handlerSPI.SPIConfig.TIMode								= SPI_TIMODE_DISABLE;
	handlerSPI.SPIConfig.CRCCalculation						= SPI_CRC_DISABLE;
	handlerSPI.SPIConfig.FrameFormat						= SPI_MOTOROLA_MODE;
	handlerSPI.SPIConfig.State								= SPI_ENABLE;
	SPI_Config(&handlerSPI);

	displayTest();
	delay_ms(500);
	displayNOTest();
	noDeco();
	setIntensity(0x00);
	scanLimit();
	shutdown(NORMAL);
	clearDisplay();
	delay_ms(50);
}

void sendNO_OP(void){
	SPI_Send(&handlerSPI,0x00);
	SPI_Send(&handlerSPI,0x0);
}


void send_to_MAX7219(uint8_t addr, uint8_t data){
	NSS_LOW(&handlerSPI);
	SPI_Send(&handlerSPI,addr);
	SPI_Send(&handlerSPI,data);
	NSS_HIGH(&handlerSPI);
	delay_ms(2);
}

void sendMatrix1(uint8_t addr, uint8_t data){
	NSS_LOW(&handlerSPI);
	sendNO_OP();
	sendNO_OP();
	sendNO_OP();
	SPI_Send(&handlerSPI,addr);
	SPI_Send(&handlerSPI,data);
	NSS_HIGH(&handlerSPI);
	//delay_ms(1);
}

void sendMatrix2(uint8_t addr, uint8_t data){
	NSS_LOW(&handlerSPI);
	sendNO_OP();
	sendNO_OP();
	SPI_Send(&handlerSPI,addr);
	SPI_Send(&handlerSPI,data);
	sendNO_OP();
	NSS_HIGH(&handlerSPI);
	//delay_ms(1);
}

void sendMatrix3(uint8_t addr, uint8_t data){
	NSS_LOW(&handlerSPI);
	sendNO_OP();
	SPI_Send(&handlerSPI,addr);
	SPI_Send(&handlerSPI,data);
	sendNO_OP();
	sendNO_OP();
	NSS_HIGH(&handlerSPI);
	//delay_ms(1);
}

void sendMatrix4(uint8_t addr, uint8_t data){
	NSS_LOW(&handlerSPI);
	SPI_Send(&handlerSPI,addr);
	SPI_Send(&handlerSPI,data);
	sendNO_OP();
	sendNO_OP();
	sendNO_OP();
	NSS_HIGH(&handlerSPI);
	//delay_ms(1);
}
void setRow(uint8_t row){
	switch(row){
	case 0:
		__NOP();
	default:
		__NOP();
	}
}

void setColumn(uint8_t col){
	switch(col){
	case 1:
		__NOP();
	default:
		__NOP();

	}

}


void clearDisplay(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(DIGIT0, 0x00);
		send_to_MAX7219(DIGIT1, 0x00);
		send_to_MAX7219(DIGIT2, 0x00);
		send_to_MAX7219(DIGIT3, 0x00);
		send_to_MAX7219(DIGIT4, 0x00);
		send_to_MAX7219(DIGIT5, 0x00);
		send_to_MAX7219(DIGIT6, 0x00);
		send_to_MAX7219(DIGIT7, 0x00);
		delay_ms(50);
		i--;
	}
}
void displayTest(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(DISPLAY_TEST, TEST);
		i--;
	}
}
void displayNOTest(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(DISPLAY_TEST, NO_TEST);
		i--;
	}
}

void noDeco(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(DECO_MODE, NO_DECO);
		i--;
	}
}

void setIntensity(uint8_t intensity){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(INTENSITY, intensity);
		i--;
	}
}

void scanLimit(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(SCAN_LIMIT,ALL);
		i--;
	}
}

void shutdown(int normal){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(SHUTDOWN, normal);
		i--;
	}
}






