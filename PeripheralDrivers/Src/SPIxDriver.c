/*
 * SPIxDriver.c
 *
 *  Created on: Apr 19, 2023
 *      Author: julian
 */

#include "stm32f4xx.h"
#include "SPIxDriver.h"

/*
 * PASO-1: HABILITAR EL CLOCK DEL SPI
 * PASO-2: CONFIGURAR EL CR1
 * PASO-3: CONFIGURAR EL CR2
 *
 * */
uint8_t temp;

void SPI_Config(SPI_Handler_t *ptrSPIHandler){
	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
		/* Lo debemos hacer para cada uno de las posibles opciones que tengamos (SPI1, SPI2, SPI3, SPI4, SPI5)
		 * SPI2,SPI3 = APB1
		 * SPI1,SPI4,SPI5 = APB2*/
	/* 1.1 Configuramos el SPI1 conectado al APB2 */
	if(ptrSPIHandler->ptrSPIx == SPI1){
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	}
	/* 1.2 Configuramos el SPI2 conectado al APB1 */
	else if(ptrSPIHandler->ptrSPIx == SPI2){
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	}
	/* 1.3 Configuramos el SPI3 conectado al APB1 */
	else if(ptrSPIHandler->ptrSPIx == SPI3){
		RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	}
	/* 1.4 Configuramos el SPI4 conectado al APB2 */
	else if(ptrSPIHandler->ptrSPIx == SPI4){
		RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
	}
	/* 1.5 Configuramos el SPI5 conectado al APB2 */
	else if(ptrSPIHandler->ptrSPIx == SPI5){
		RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
	}

	/*Configuramos en el CR1
	 * El modo*
	 * */
	/*Primero limpiamos los dos registros del SPI*/
	ptrSPIHandler->ptrSPIx->CR1 = 0;
	ptrSPIHandler->ptrSPIx->CR2 = 0;

	/*Configuramos el modo del SPI
	 *  	CLKPol	CLKPhase
	MODO0	0		0
	MODO1	0		1
	MODO2	1		0
	MODO3	1		1
	*/

	/*MODO 0*/
	if(ptrSPIHandler->SPIConfig.SPIMode == SPI_MODE0){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_CPOL;
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_CPHA;
	}
	/*MODO 1*/
	else if(ptrSPIHandler->SPIConfig.SPIMode == SPI_MODE1){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_CPOL;
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_CPHA;
	}
	/*MODO 2*/
	else if(ptrSPIHandler->SPIConfig.SPIMode == SPI_MODE2){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_CPOL;
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_CPHA;
	}
	else if(ptrSPIHandler->SPIConfig.SPIMode == SPI_MODE3){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_CPOL;
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_CPHA;
	}
	else{
		__NOP();
	}

	// CONFIGURAMOS EL BAUD RATE
	//EJM: fPCLK/16, APB2 CLOCK = 100MHz, SPI clk = 6.25MHz
	if(ptrSPIHandler->SPIConfig.BaudRatePrescaler == SPI_SCLK_SPEED_DIV2){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_BR;
	}
	else if(ptrSPIHandler->SPIConfig.BaudRatePrescaler == SPI_SCLK_SPEED_DIV4){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_BR_0;
	}
	else if(ptrSPIHandler->SPIConfig.BaudRatePrescaler == SPI_SCLK_SPEED_DIV8){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_BR_1;
	}
	else if(ptrSPIHandler->SPIConfig.BaudRatePrescaler == SPI_SCLK_SPEED_DIV16){
		ptrSPIHandler->ptrSPIx->CR1 |= (SPI_CR1_BR_0 + SPI_CR1_BR_1);
	}
	else if(ptrSPIHandler->SPIConfig.BaudRatePrescaler == SPI_SCLK_SPEED_DIV32){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_BR_2;
	}
	else if(ptrSPIHandler->SPIConfig.BaudRatePrescaler == SPI_SCLK_SPEED_DIV64){
		ptrSPIHandler->ptrSPIx->CR1 |= (SPI_CR1_BR_2 + SPI_CR1_BR_0);
	}
	else if(ptrSPIHandler->SPIConfig.BaudRatePrescaler == SPI_SCLK_SPEED_DIV128){
		ptrSPIHandler->ptrSPIx->CR1 |= (SPI_CR1_BR_2 + SPI_CR1_BR_1);
	}
	else if(ptrSPIHandler->SPIConfig.BaudRatePrescaler == SPI_SCLK_SPEED_DIV256){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_BR;
	}
	else{
		__NOP();
	}

	// Elegimos que cual bit va primero LSB o MSB
	if(ptrSPIHandler->SPIConfig.FirstBit == SPI_LSBFIRST){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_LSBFIRST;
	}
	else{
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_LSBFIRST;
	}

	// Configuring the Data Size
	// Debe estar apagado el SPI para configurar
	if(ptrSPIHandler->SPIConfig.DataSize == SPI_DATA_SIZE_8BITS){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_DFF;
	}
	else if(ptrSPIHandler->SPIConfig.DataSize == SPI_DATA_SIZE_16BITS){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_DFF;
	}
	else{
		__NOP();
	}

	// Configuring the Bus Configuration
	// Full Duplex
	if(ptrSPIHandler->SPIConfig.DirectionBusConfig == SPI_BUS_CONFIG_FD){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_RXONLY;
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_BIDIMODE;
	}
	// Half Duplex
	else if(ptrSPIHandler->SPIConfig.DirectionBusConfig == SPI_BUS_CONFIG_HD){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_RXONLY;
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_BIDIMODE;
	}
	// Transmission Only
	else if(ptrSPIHandler->SPIConfig.DirectionBusConfig == SPI_BUS_CONFIG_TX){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_BIDIOE;
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_BIDIMODE;
	}
	else{
		__NOP();
	}

	// Configuring the CRC
	if(ptrSPIHandler->SPIConfig.CRCCalculation == SPI_CRC_ENABLE){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_CRCEN;
	}
	else if(ptrSPIHandler->SPIConfig.CRCCalculation == SPI_CRC_DISABLE){
			ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_CRCEN;
	}
	else{
		__NOP();
	}

	//Configuring the CR2 Register

	if(ptrSPIHandler->SPIConfig.FrameFormat == SPI_MOTOROLA_MODE){
		ptrSPIHandler->ptrSPIx->CR2 &= ~ SPI_CR2_FRF;
	}

	else if(ptrSPIHandler->SPIConfig.FrameFormat == SPI_TI_MODE){
			ptrSPIHandler->ptrSPIx->CR2 |= SPI_CR2_FRF;
	}
	else{
		__NOP();
	}


	// CONFIGURAMOS EL TIPO DE DISPOSITIVO
	// Maestro = 1
	if(ptrSPIHandler->SPIConfig.DeviceMode == SPI_DEVICE_MODE_MASTER){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_MSTR;
	}
	// Slave = 0
	else if(ptrSPIHandler->SPIConfig.DeviceMode == SPI_DEVICE_MODE_SLAVE){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_MSTR;
	}
	else{
		__NOP();
	}

	// Configuring the NSS (Negative Slave Select) the chip select signal is active-low
	// meaning that it is low when the device is selected and high when it is not.
	// For NSS to work properly SSM and SSI must be configured

	// In NSS software mode, set the SSM bit and clear the SSI bit in the SPI_CR1 register
	// La Familia F4 no tiene manera de generar un pulso en el PIN NSS, asi que es necesario
	// hacerlo manualmente

	if(ptrSPIHandler->SPIConfig.NSS == SPI_NSS_ENABLE){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_SSM; //Activamos el SSM
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_SSI; //Activamos el SSI
		//ptrSPIHandler->ptrSPIx->CR2 |= SPI_CR2_SSOE; //Activamos el SSOE
	}
	else if(ptrSPIHandler->SPIConfig.NSS == SPI_NSS_DISABLE){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_SSM; //Desactivamos el SSM
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_SSI; //Desactivamos el SSI
		//ptrSPIHandler->ptrSPIx->CR2 &= ~ SPI_CR2_SSOE; //Desactivamos el SSOE
	} // Esto lo uso para afectar el LOAD
	else{
		__NOP();
	}

	// Activamos el modulo serial
	if(ptrSPIHandler->SPIConfig.State == SPI_ENABLE){
		ptrSPIHandler->ptrSPIx->CR1 |= SPI_CR1_SPE;
	}
	else if(ptrSPIHandler->SPIConfig.State == SPI_DISABLE){
		ptrSPIHandler->ptrSPIx->CR1 &= ~ SPI_CR1_SPE;
	}
	else{
		__NOP();
	}

}
/* Funciones para el pin Chip Select*/
/*Seleccionamos el esclavo llevando el pin NSS a GND*/
void NSS_LOW(SPI_Handler_t* ptrSPIHandler){
	GPIO_WritePin(&ptrSPIHandler->NSS_Pin, RESET);
}

/*Seleccionamos el esclavo llevando el pin NSS a VCC*/
void NSS_HIGH(SPI_Handler_t* ptrSPIHandler){
	GPIO_WritePin(&ptrSPIHandler->NSS_Pin, SET);
}


/************** STEPS TO FOLLOW *****************
1. Wait for the TXE bit to set in the Status Register
2. Write the data to the Data Register
3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
4. Clear the Overrun flag by reading DR and SR
************************************************/

void SPI_Send(SPI_Handler_t *ptrSPIHandler, uint8_t TxBuffer){
   while( !(ptrSPIHandler->ptrSPIx->SR & SPI_SR_TXE)){
	   __NOP();
   };  // wait for TXE bit to set -> This will indicate that the buffer is empty
   ptrSPIHandler->ptrSPIx->DR = TxBuffer; // load the data into the Data Register

/*During discontinuous communications, there is a 2 APB clock period delay between the
write operation to the SPI_DR register and BSY bit setting. As a consequence it is
mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
data.
*/
	while( !(ptrSPIHandler->ptrSPIx->SR & SPI_SR_TXE)){
	   __NOP();
	}; // wait for TXE bit to set -> This will indicate that the buffer is empty
	while((ptrSPIHandler->ptrSPIx->SR & SPI_SR_BSY)){
		__NOP();
	}; // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication

	//  Clear the Overrun flag by reading DR and SR
	temp = ptrSPIHandler->ptrSPIx->DR;
	temp = ptrSPIHandler->ptrSPIx->SR;
}
