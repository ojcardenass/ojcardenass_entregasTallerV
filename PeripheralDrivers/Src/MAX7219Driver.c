/*
 * MAX7219Driver.c
 *
 *  Created on: Apr 22, 2023
 *      Author: julian
 */
#include "stm32f4xx.h"
#include "MAX7219Driver.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"

SPI_Handler_t handlerSPI		= {0};
GPIO_Handler_t handlerCS		= {0};


void MAX7219_Init(void){
	/* Configuramos el SysTick*/
	config_SysTick_ms(CLKSPEED);

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
	handlerSPI.ptrSPIx									= SPI2;
	handlerSPI.NSS_Pin									= handlerCS;
	handlerSPI.SPIConfig.DeviceMode						= SPI_DEVICE_MODE_MASTER;
	handlerSPI.SPIConfig.DirectionBusConfig				= SPI_BUS_CONFIG_TX;
	handlerSPI.SPIConfig.BaudRatePrescaler				= SPI_SCLK_SPEED_DIV16;
	handlerSPI.SPIConfig.DataSize						= SPI_DATA_SIZE_8BITS;
	handlerSPI.SPIConfig.SPIMode						= SPI_MODE0;
	handlerSPI.SPIConfig.FirstBit						= SPI_MSBFIRST;
	handlerSPI.SPIConfig.NSS							= SPI_NSS_ENABLE;
	handlerSPI.SPIConfig.TIMode							= SPI_TIMODE_DISABLE;
	handlerSPI.SPIConfig.CRCCalculation					= SPI_CRC_DISABLE;
	handlerSPI.SPIConfig.FrameFormat					= SPI_MOTOROLA_MODE;
	handlerSPI.SPIConfig.State							= SPI_ENABLE;
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
/* Funcion que envia codigos de No Operacion, para seleccionar el modulo*/
void sendNO_OP(void){
	SPI_Send(&handlerSPI,0x00);
	SPI_Send(&handlerSPI,0x0);
}

/* Funcion para enviar datos en una direccion especifica del MAX7219*/
void send_to_MAX7219(uint8_t addr, uint8_t data){
	NSS_LOW(&handlerSPI);
	SPI_Send(&handlerSPI,addr);
	SPI_Send(&handlerSPI,data);
	NSS_HIGH(&handlerSPI);
	//delay_ms(2);
}

/* Funcion para enviar datos, solamente a la primera matriz del MAX7219*/
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
/* Funcion para enviar datos, solamente a la segunda matriz del MAX7219*/
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

/* Funcion para enviar datos, solamente a la tercera matriz del MAX7219*/
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
/* Funcion para enviar datos, solamente a la cuarta matriz del MAX7219*/
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

/* Funcion que retorna el valor para cada fila, segun la magnitud
 * de su columna
 *
 * Esta funcion esta hecha para una matriz tipica de LEDs del MAX7219 (8 x 8)
 * */
void rowOut(uint8_t* values, uint8_t* ledBuffer){
	uint8_t columns[8] = {1,2,4,8,16,32,64,128};

	for(uint8_t i = 0; i < 8; i ++){
		uint8_t colBuffer = 0;
		for(uint8_t j = 0; j < 8; j ++){
			if(i < values[j]){
				colBuffer += columns[j];
			}
		}
		ledBuffer[i] = colBuffer;
	}
}
/* Funcion lee un arreglo de magnitudes ordenadas por columnas, y se lo envia al
 * MAX7219, segun el modulo elegido*/
void setColumnsModx(uint8_t* values, uint8_t module){
	uint8_t digits[8] = {DIGIT0,DIGIT1,DIGIT2,DIGIT3,DIGIT4,DIGIT5,DIGIT6,DIGIT7};
	uint8_t ledBuffer[8] = {0};
	switch(module){
	case 1:
		rowOut(values,ledBuffer);
		for(uint8_t i = 0;i < 8;i ++){
			sendMatrix1(digits[i],ledBuffer[i]);
		}
		ledBuffer[8] = 0;
		break;
	case 2:
		rowOut(values,ledBuffer);
		for(uint8_t i = 0;i < 8;i ++){
			sendMatrix2(digits[i],ledBuffer[i]);
		}
		ledBuffer[8] = 0;
		break;
	case 3:
		rowOut(values,ledBuffer);
		for(uint8_t i = 0;i < 8;i ++){
			sendMatrix3(digits[i],ledBuffer[i]);
		}
		ledBuffer[8] = 0;
		break;
	case 4:
		rowOut(values,ledBuffer);
		for(uint8_t i = 0;i < 8;i ++){
			sendMatrix4(digits[i],ledBuffer[i]);
		}
		ledBuffer[8] = 0;
		break;
	default:
		__NOP();
	}
}
/* Todas las siguientes funciones se envian 4 veces para inicializar, ya que los modulos estan conectados en
 * cascada, en otras palabras: la salida de uno, es la entrada del otro*/

/* Funcion para limpiar todas las posiciones de las 4 matrices*/
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
		delay_ms(40);
		i--;
	}
}
/* Funcion para probar el funcionamiento de todos los LED*/
void displayTest(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(DISPLAY_TEST, TEST);
		i--;
	}
}
/* Funcion para desactivar la prueba del los LED*/
void displayNOTest(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(DISPLAY_TEST, NO_TEST);
		i--;
	}
}
/* Funcion para seleccionar el modo Matriz de LED*/
void noDeco(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(DECO_MODE, NO_DECO);
		i--;
	}
}
/* Funcion para seleccionar la intensidad de la matriz numeros del 0 al 15*/
void setIntensity(uint8_t intensity){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(INTENSITY, intensity);
		i--;
	}
}
/* Funcion para elegir cuantos LEDs se van a encender*/
void scanLimit(void){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(SCAN_LIMIT,ALL);
		i--;
	}
}
/* Funciona para elegir el funcionamiento del MAX7219, Normal o Apagado*/
void shutdown(int normal){
	uint8_t i = 4;
	while(i){
		send_to_MAX7219(SHUTDOWN, normal);
		i--;
	}
}






