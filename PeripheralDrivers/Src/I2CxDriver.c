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



void i2c_config(I2C_Handler_t *ptrHandlerI2C){

	/* 1. Se activa la señal de reloj para el modulo I2C seleccionado*/
	if(ptrHandlerI2C->ptrI2Cx == I2C1){
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}

	else if(ptrHandlerI2C->ptrI2Cx == I2C2){
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}

	else if(ptrHandlerI2C->ptrI2Cx == I2C3){
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	/* 2. Se reinicia el periférico, de forma que inicia en un estado
	   conocido */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_SWRST;

	/* 3. Se indica cuál es la velocidad del reloj principal, que es la
	   señal utilizada por el periférico para generar la señal de reloj
	   para el bys I2C*/
	ptrHandlerI2C->ptrI2Cx->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos);
	if(CLKSPEED == 16){
		ptrHandlerI2C -> ptrI2Cx -> CR2 |= (MAIN_CLOCK_16_MHz_FOR_I2C << I2C_CR2_FREQ_Pos);
	}
	else{
		ptrHandlerI2C -> ptrI2Cx -> CR2 |= ((CLKSPEED / 2) << I2C_CR2_FREQ_Pos);
	}

	/* 4. Se configura el modo I2C en el que el sistema funciona.
	 * En esta configuración se incluye también la velocidad del
	 * reloj y el tiempo máximo para el cambio de la señal (T-Rise).
	 * Todo comienza con los dos registros en 0*/
	ptrHandlerI2C->ptrI2Cx->CCR = 0;
	ptrHandlerI2C->ptrI2Cx->TRISE = 0;

	if(ptrHandlerI2C->modeI2C == I2C_MODE_SM){

		// Estamos en modo "standar" (SM Mode)
		// Se selecciona el modo estándar
		ptrHandlerI2C->ptrI2Cx->CCR &= ~I2C_CCR_FS;

		// Se configura el registro que se encarga de generar la señal
		// del reloj
		switch (CLKSPEED) {
			case 16:
				ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz_16MHz << I2C_CCR_CCR_Pos);
				break;
			case 80:
				ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz_80MHz << I2C_CCR_CCR_Pos);
				break;
			case 100:
				ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz_80MHz << I2C_CCR_CCR_Pos);
				break;
			default:
				ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz_16MHz << I2C_CCR_CCR_Pos);
				break;
		}
		// Se configura el registro que controla el tiempo T-Rise
		// máximo
		switch (CLKSPEED) {
			case 16:
				ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM_16MHz;
				break;
			case 80:
				ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM_80MHz;
				break;
			case 100:
				ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM_100MHz;
				break;
			default:
				ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM_16MHz;
				break;
		}

	}
	else{

		// Estamos en modo "Fast" (FM mode)
		// Se selecciona el modo Fast
		ptrHandlerI2C->ptrI2Cx->CCR |= I2C_CCR_FS;

		// Se configura el registro que se encarga de generar la señal
		// del reloj
		switch (CLKSPEED) {
			case 16:
				ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_SPEED_400KHz_16MHz << I2C_CCR_CCR_Pos);
				break;
			case 80:
				ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_SPEED_400KHz_80MHz << I2C_CCR_CCR_Pos);
				break;
			case 100:
				ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_SPEED_400KHz_80MHz << I2C_CCR_CCR_Pos);
				break;
			default:
				ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_SPEED_400KHz_16MHz << I2C_CCR_CCR_Pos);
				break;
		}
		// Se configura el registro que controla el tiempo T-Rise
		// máximo
		switch (CLKSPEED) {
			case 16:
				ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM_16MHz;
				break;
			case 80:
				ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM_80MHz;
				break;
			case 100:
				ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM_100MHz;
				break;
			default:
				ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM_16MHz;
				break;
		}

	}

	/* 5. Se activa el modulo I2C */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_PE;

}

/* 8. Se genera la condición de stop*/
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* 7. Se genera la condición de stop */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_STOP;
}

/* 1. Se verifica que la línea no está ocupada - bit "busy" en I2C_CR2 */
/* 2. Se genera la señal "start" */
/* 2a. Se espera a que la bandera del evento "start" se levante */
/* Mientras se espera, el valor de SB es 0, entonces la negación (!) es
 *  1 */
void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* 1. Se verifica que la línea no está ocupada - bit "busy" en
	 * I2C_CR2*/
	while(ptrHandlerI2C->ptrI2Cx->SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	/* 2. Se genera la señal de "start" */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	/* 2a. Se espera a que la bandera del evento "start" se levante*/
	/* Mientras se espera, el valor de SB es 0, entonces la negación (!)
	 *  es 1 */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}

}

void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* 2. Se genera la señal de "start" */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	/* 2a. Se espera a que la bandera del evento "start" se levante*/
	/* Mientras se espera, el valor de SB es 0, entonces la negación (!)
	 *  es 1 */
	while (!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)) {
		__NOP();
	}
}

/* 7a. Se activa la indicación para no-ACK (indicación para el Slave
 * de terminar) */
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C){
	/* Se debe escribir cero en la posición ACK del registro de
	 * control 1*/
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_ACK;
}

/* 7b. Se activa la indicación para ACK (indicación  */
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C){
	/* Se debe escribir uno en la posición ACK del registro de
	 * control 1*/
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_ACK;
}

void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite){
	/* 0. Se define una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;

	/* 3. Se envia la dirección del Slave y el bit que indica que se
	 * desea escribir (0) (en el siguiente paso se envia la dirección
	 * de memoria que se desea escribir)*/
	ptrHandlerI2C->ptrI2Cx->DR = (slaveAddress << 1) | readOrWrite;

	/* 3.1. Se espera hasta que la bandera del evento "addr" se levante
	 * (esto indica que la dirección fue enviada satisfactoriamente) */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/* 3.2. Se debe limpiar la bandera de la recepción de ACK de la "addr",
	 * para lo cual se debe leer en secuencia primero el I2C_SR1 y
	 * luego I2C_SR2*/
	auxByte = ptrHandlerI2C->ptrI2Cx->SR1;
	auxByte = ptrHandlerI2C->ptrI2Cx->SR2;
}

void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){
	/* 4. Se envia la dirección de memoria que se desea leer */
	ptrHandlerI2C->ptrI2Cx->DR = memAddr;

	/* 4.1. Se espera hasta que el byte sea transmitido */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)){
		__NOP();
	}

}

void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	/* 5. Se carga el valor que se desea escribir */
	ptrHandlerI2C->ptrI2Cx->DR = dataToWrite;

	/* 6. Se espera hasta que el byte sea transmitido */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF)){
			__NOP();
		}
}

uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C){
	/* 9. Se espera hasta que el byte entrante sea recibido */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_RXNE)){
			__NOP();
		}

	ptrHandlerI2C->dataI2C = ptrHandlerI2C->ptrI2Cx->DR;
	return ptrHandlerI2C->dataI2C;

}

uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead){
	/* 0. Se crea una variable auxiliar para recibir el dato que se
	 * lee */
	uint8_t auxRead = 0;

	/* 1. Se genera la condición Start*/
	i2c_startTransaction(ptrHandlerI2C);

	/* 2. Se envia la dirección del esclavo y la indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	/* 3. Se envía la dirección de memoria que se desea leer */
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	/* 4. Se crea una condición de reStart */
	i2c_reStartTransaction(ptrHandlerI2C);

	/* 5. Se envía la dirección del esclavo y la indicación de LEER */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	/* 6. Se genera la condición de NoAck, para que el Master no responda
	 * y el slave solo envie 1 byte */
	i2c_sendNoAck(ptrHandlerI2C);

	/* read multiple */

	/* 7. Se genera la condición Stop, para que el slave se detenga
	 * después de 1 byte */
	i2c_stopTransaction(ptrHandlerI2C);

	/* 8. Se lee el dato que envía el esclavo */
	auxRead = i2c_readDataByte(ptrHandlerI2C);

	return auxRead;
}

/* Funcion para leer varios registros, recibe
 * - Handler del I2C
 * - Registro desde que se empiezan a leer los datos
 * - Arreglo de salida
 * - Cantidad de registros a leer
 * */
void i2c_readMultipleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t startReg, uint8_t* data, uint8_t numRegs){
		/* 1. Se genera la condición Start*/
		i2c_startTransaction(ptrHandlerI2C);

		/* 2. Se envia la dirección del esclavo y la indicación de ESCRIBIR */
		i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

		/* 3. Se envía la dirección de memoria que se desea leer */
		i2c_sendMemoryAddress(ptrHandlerI2C, startReg);

		/* 4. Se crea una condición de reStart */
		i2c_reStartTransaction(ptrHandlerI2C);

		/* 5. Se envía la dirección del esclavo y la indicación de LEER */
		i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

		i2c_sendAck(ptrHandlerI2C);

		/* 8. Se lee el dato que envía el esclavo */
		for (uint8_t i = 0; i < numRegs; i ++){
			data[i] = i2c_readDataByte(ptrHandlerI2C);
		}

		/* 6. Se genera la condición de NoAck, para que el Master no responda
		 * y el slave solo envie 1 byte */
		i2c_sendNoAck(ptrHandlerI2C);

		/* 7. Se genera la condición Stop, para que el slave se detenga
		 * después de 1 byte */
		i2c_stopTransaction(ptrHandlerI2C);
}


void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue){
	/* 1. Se genera la condición Start*/
	i2c_startTransaction(ptrHandlerI2C);

	/* 2. Se envia la dirección del esclavo y la indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	/* 3. Se envía la dirección de memoria que se desea leer */
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	/* 4. Se envía el valor que se desea escribir en el registro seleccionado */
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	/* 7. Se genera la condición Stop, para que el slave se detenga
	 * después de 1 byte */
	i2c_stopTransaction(ptrHandlerI2C);

}
