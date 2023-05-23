/*
 * I2CxDriver.c
 *
 *  Created on: May 13, 2023
 *      Author: julian
 */

#include <stdint.h>
#include "I2CxDriver.h"

/*
 * Recordar que se deben configurar los pines para el I2C (SDA y SCL),
 * para lo cual se necesita el módulo GPIO y los pines configurados
 * en el modo Alternative Function. Además, estos pines deben ser
 * configurados como salidas open-drain y con las resistencias en
 * modo pull-up.
 */

void i2c_config (I2C_Handler_t *ptrHandlerI2C){

	/* 1. Activamos la señal de reloj para el módulo I2C seleccionado */
	if (ptrHandlerI2C -> ptrI2Cx == I2C1){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C1EN;
	}

	else if (ptrHandlerI2C -> ptrI2Cx == I2C2){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C2EN;
	}

	else if (ptrHandlerI2C -> ptrI2Cx == I2C3){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	/* 2. Reiniciamos el periférico, de forma que inicia en un estado conocido */
	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C -> ptrI2Cx -> CR1 &= ~I2C_CR1_SWRST;

	/* 3. Indicamos cual es la velocidad del reloj principal, que es la señal utilizada
	 *    por el periférico para generar la señal de reloj para el bus I2C */
	ptrHandlerI2C -> ptrI2Cx -> CR2 &= ~ (0b111111 << I2C_CR2_FREQ_Pos);
	if(CLKSPEED == 16){
		ptrHandlerI2C -> ptrI2Cx -> CR2 |= (MAIN_CLOCK_16_MHz_FOR_I2C << I2C_CR2_FREQ_Pos);
	}
	else{
		ptrHandlerI2C -> ptrI2Cx -> CR2 |= ((CLKSPEED / 2) << I2C_CR2_FREQ_Pos);
	}

	/* 4. Configuramos el modo I2C en el que el sistema funciona
	 * En esta configuramción se incluye también la velocidad del reloj
	 * y el timpo máximo para el cambio de la señal (T_rise).
	 * Todo comienza con los dos registros en 0 */
	ptrHandlerI2C -> ptrI2Cx -> CCR = 0;
	ptrHandlerI2C -> ptrI2Cx -> TRISE = 0;

	if (ptrHandlerI2C -> modeI2C == I2C_MODE_SM){

		// Estamos en modo "standar" (SM Mode)
		// Seleccionamos el modo estandar
		ptrHandlerI2C -> ptrI2Cx -> CCR &= ~ I2C_CCR_FS;

		// Configuramos el registro que se encarga de generar la señal del reloj
		ptrHandlerI2C -> ptrI2Cx -> CCR |= (I2C_MODE_SM_SPEED_100KHz << I2C_CCR_CCR_Pos);

		ptrHandlerI2C -> ptrI2Cx -> TRISE |= I2C_MAX_RISE_TIME_SM;
	}

	else{
		// Estamos en modo "Fast" (FM Mode)
		// Seleccionamos el modo Fast
		ptrHandlerI2C -> ptrI2Cx -> CCR |= I2C_CCR_FS;

		// Configuramos el registro que se encarga de generar la señal del reloj
		ptrHandlerI2C -> ptrI2Cx -> CCR |= (I2C_MODE_FM_SPEED_400KHz << I2C_CCR_CCR_Pos);

		// Configuramos el registro que controla el tiempo T_Rise máximo
		ptrHandlerI2C -> ptrI2Cx -> TRISE |= I2C_MAX_RISE_TIME_FM;
	}

	/* 5. activamos el módulo I2C */
	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_PE;

}


/* 8. Generamos la condición de stop */
void i2c_stopTransaction (I2C_Handler_t *ptrHandlerI2C){
	/* 7. Generamos la condición de stop */
	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_STOP;
}

/* 1. Verificamos que la línea no está ocupada - bit "busy" en I2C_CR2 */
/* 2. Generamos la señal "start" */
/* 2a. Esperamos a que la bandera del evento "start" se levante */
void i2c_startTransaction (I2C_Handler_t *ptrHandlerI2C){
	/* 1. Verificamos que la línea no está ocupada - bit "busy" en I2C_CR2 */
	while (ptrHandlerI2C -> ptrI2Cx -> SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	/* 2. Generamos la señal "start" */
	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	/* 2a. Esperamos a que la bandera del evento "start" se levante */
	/* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
	while (!ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_SB){
		__NOP();
	}
}

void i2c_reStartTransaction (I2C_Handler_t *ptrHandlerI2C){
	/* 2. Generamos la señal "start" */
	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	/* 2a. Esperamos a que la bandera del evento "start" se levante */
	/* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
	while (!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_SB)){
		__NOP();
	}
}

/* 7a. Activamos la indicación para no-Ack (indicación para el Slave de terminar */
void i2c_sendNoAck (I2C_Handler_t *ptrHandlerI2C){
	/* (Debemos escribir cero en la posición ACK del registro de control 1) */
	ptrHandlerI2C -> ptrI2Cx -> CR1 &= ~ I2C_CR1_ACK;
}

/* 7b. Activamos la indicción para ACK (indicación para el Slave indicar que está atento) */
void i2c_sendAck (I2C_Handler_t *ptrHandlerI2C){
	/* (Debemos escribir uno en la posición ACK del registro de control 1) */
	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_ACK;
}

void i2c_sendSlaveAddressRW (I2C_Handler_t *ptrHandlerI2C, uint8_t slave, uint8_t readOrWrite){
	/* 0. Definimos una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;

	/* 3. Enviamos la dirección del Slave y el bit que indica que deseamos escribir (0)
	 *   (en el siguiente paso se envía la dirección de memoria que se desea escribir */
	ptrHandlerI2C -> ptrI2Cx -> DR = (slave << 1) | readOrWrite;


	 /* 3.1. Esperamos hasta que la bandera del evento "ADDR" se levante
	  * (esto nos indica que la dirección fue enviada satisfactoriamente) */
	while (!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/* 3.2. Debemos limpiar la bandera de la recepción de ACK de la "ADDR", para lo cual
	 *      debemos leer en secuencia el I2C_SR1 y luego I2C_SR2 */
	auxByte = ptrHandlerI2C -> ptrI2Cx -> SR1;
	auxByte = ptrHandlerI2C -> ptrI2Cx -> SR2;
}

void i2c_sendMemoryAddress (I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){
	/* 4. Enviamos la dirección de memoria que desamos leer */
	ptrHandlerI2C -> ptrI2Cx -> DR = memAddr;

	/* 4.1. Esperamos hasta que el byte sea transmitido */
	while (!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_TXE)){
		__NOP();
	}
}

void i2c_sendDataByte (I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	/* 5. Cargamos el valor que deseamos escribir */
	ptrHandlerI2C -> ptrI2Cx -> DR = dataToWrite;

	/* 6. Esperamos hasta que el byte sea transmitido */
	while (!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_BTF)){
		__NOP();
	}
}

uint8_t i2c_readDataByte (I2C_Handler_t *ptrHandlerI2C){
	/* 9. Esperamos hasta que el byte entrante sea recibido */
	while (!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_RXNE)){
		__NOP();
	}

	ptrHandlerI2C -> dataI2C = ptrHandlerI2C -> ptrI2Cx -> DR;
	return ptrHandlerI2C -> dataI2C;
}
