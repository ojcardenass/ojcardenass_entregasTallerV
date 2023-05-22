/*
 * USARTxDriver.c
 *
 *  Created on: Apr 6, 2022
 *      Author: Oscar Julian Cardenas Sosa
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"

/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */

uint8_t	auxRxData	=	0;

void USART_Config(USART_Handler_t *ptrUsartHandler){
	/* 0. Desactivamos las interrupciones globales */

	__disable_irq();

	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 conectado al APB2 */
	if(ptrUsartHandler->ptrUSARTx == USART1){
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}

    /* 1.2 Configuramos el USART2 conectado al APB1*/
	else if(ptrUsartHandler->ptrUSARTx == USART2){
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	}

    /* 1.3 Configuramos el USART6 conectado al APB2 */
	else if(ptrUsartHandler->ptrUSARTx == USART6){
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	}

	else{
		__NOP();
	}

	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity esta activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){



		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			// Es even, entonces cargamos la configuracion adecuada
			ptrUsartHandler->ptrUSARTx->CR1 &= ~ USART_CR1_PS;
            // Se activa el parity control
            ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE;
		}
		else{
			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS;
            // Se activa el parity control
            ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE;
		}
	}else{
		// Si llegamos aca, es porque no deseamos tener el parity-check
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PCE;
	}

	// 2.3 Configuramos el tamaño del dato
    if (ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_8BIT){
		// Verificamos si se esta trabajando o no con paridad
		if (ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_NONE) {
			// Deseamos trabajar con datos de 8 bits
			ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_M;
		}
		else {
			// Si la partidad esta activada debemos agregar un bit adicional
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M;
		}
	}
    else if (ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_9BIT){
    	ptrUsartHandler->ptrUSARTx->CR1 |=  USART_CR1_M;
    }
    else{
    	__NOP();
    }

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {
		// Debemoscargar el valor 0b00 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 &= ~ USART_CR2_STOP;
		break;
	}
	case USART_STOPBIT_0_5: {
		// Debemoscargar el valor 0b01 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_0;
		break;
	}
	case USART_STOPBIT_2: {
		// Debemos cargar el valor 0b10 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_1;
		break;
	}
	case USART_STOPBIT_1_5: {
		// Debemoscargar el valor 0b11 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP;
		break;
	}
	default: {
		// En el casopor defecto seleccionamos 1 bit de parada
		ptrUsartHandler->ptrUSARTx->CR2 &= ~ USART_CR2_STOP;
		break;
	}
	}// fin del switch case

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	switch (CLKSPEED) {
		case 100:
			if(ptrUsartHandler->ptrUSARTx == USART2){
				/* Esta configuracion para la frecuencia del bus APB1 (50 MHz)*/
				if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
					// Configurando el Baudrate generator para una velocidad de 9600bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
				}

				else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
					//Configurando el Baudrate generator para una velocidad de 19200 bps
					ptrUsartHandler->ptrUSARTx->BRR = 0xA2C;
				}

				else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
					//Configurando el Baudrate generator para una velocidad de 115200 bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x1B2;
				}
				else{
					__NOP();
				}
			} // Fin IF USART2
			else{
				/* Esta configuracion solo funciona con la frecuencia del reloj del micro
				 * 100 MHz para el buses APB2 */
				if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
					// Configurando el Baudrate generator para una velocidad de 9600bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x28B1;
				}

				else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
					//Configurando el Baudrate generator para una velocidad de 19200 bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
				}

				else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
					// El valor a cargar es 8.6875 -> Mantiza = 8,fraction = 0.6875
					// Mantiza = 8 = 0x8, fraction = 16 * 0.6875 = 11 = B
					// Valor a cargar 0x811
					//Configurando el Baudrate generator para una velocidad de 115200 bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x364;
				}
				else{
					__NOP();
				}
			}
			break;
		case 80:
			if(ptrUsartHandler->ptrUSARTx == USART2){
				/* Esta configuracion para la frecuencia del bus APB1 (40 MHz)*/
				if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
					// Configurando el Baudrate generator para una velocidad de 9600bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x1047;
				}

				else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
					//Configurando el Baudrate generator para una velocidad de 19200 bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x823;
				}

				else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
					//Configurando el Baudrate generator para una velocidad de 115200 bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x15B;
				}
				else{
					__NOP();
				}
			} // Fin IF USART2
			else{
				/* Esta configuracion solo funciona con la frecuencia del reloj del micro
				 * 80 MHz para el buses APB2 */
				if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
					// Configurando el Baudrate generator para una velocidad de 9600bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x208D;
				}

				else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
					//Configurando el Baudrate generator para una velocidad de 19200 bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x1047;
				}

				else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
					// El valor a cargar es 8.6875 -> Mantiza = 8,fraction = 0.6875
					// Mantiza = 8 = 0x8, fraction = 16 * 0.6875 = 11 = B
					// Valor a cargar 0x811
					//Configurando el Baudrate generator para una velocidad de 115200 bps
					ptrUsartHandler->ptrUSARTx->BRR = 0x2B6;
				}
				else{
					__NOP();
				}
			}
			break;

		default:
			/* Esta configuracion solo funciona con la frecuencia estandar del reloj del micro
			 * 16 MHz, la cual se configura por defecto en ambos Buses APB1 y APB2 */
			// Ver tabla de valores (Tabla 75), Frec = 16MHz, overr = 0;
			if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
				// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
				// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
				// Valor a cargar 0x0683
				// Configurando el Baudrate generator para una velocidad de 9600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
				// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
				// Mantiza = 52 = 0x34, fraction = 16 * 0.0625 = 1
				// Valor a cargar 0x341
				//Configurando el Baudrate generator para una velocidad de 19200 bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x341;
			}

			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
				// El valor a cargar es 8.6875 -> Mantiza = 8,fraction = 0.6875
				// Mantiza = 8 = 0x8, fraction = 16 * 0.6875 = 11 = B
				// Valor a cargar 0x811
				//Configurando el Baudrate generator para una velocidad de 115200 bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x8B;
			}
			else{
				__NOP();
			}
			break;
	}


	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
        ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		break;
	}

	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
        ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		break;
	}
	}

	// 2.8 Verificamos la configuración de las interrupciones
	// 2.8a Interrupción por recepción
	if (ptrUsartHandler -> USART_Config.USART_interruptionEnableRx == USART_RX_INTERRUPT_ENABLE){
		// Como está activada debemos configurar la interrupción por recepción
		// Debemos activar la interrupción RX en la configuración del USART
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_RXNEIE;
		/*	Debemos matricular la interrupción en el NVIC
		 *  Lo debemos hacer para cada una de las posibles opciones que tengamos (USART1, USART2, USART6
		 */
		if (ptrUsartHandler -> ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
		}

		else if (ptrUsartHandler -> ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}

		else if (ptrUsartHandler -> ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}
		else{
			__NOP();
		}
	}
	// Si no hay interrupcion por Recepcion, se desactiva la posicion del registro
	else if (ptrUsartHandler -> USART_Config.USART_interruptionEnableRx == USART_RX_INTERRUPT_DISABLE){
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~ USART_CR1_RXNEIE;
	}
	else{
		__NOP();
	}

	// 2.8b Interrupción por transmision
	if (ptrUsartHandler -> USART_Config.USART_interruptionEnableTx == USART_TX_INTERRUPT_ENABLE){
		// Como está activada debemos configurar la interrupción por transmisión
		// Debemos activar la interrupción TX en la configuración del USART
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_TXEIE;
		/*	Debemos matricular la interrupción en el NVIC
		 *  Lo debemos hacer para cada una de las posibles opciones que tengamos (USART1, USART2, USART6
		 */
		if (ptrUsartHandler -> ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
		}

		else if (ptrUsartHandler -> ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}

		else if (ptrUsartHandler -> ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}
	}

	// Si no hay interrupcion por Transmision, se desactiva la posicion del registro
	else if (ptrUsartHandler -> USART_Config.USART_interruptionEnableTx == USART_TX_INTERRUPT_DISABLE){
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~ USART_CR1_TXEIE;
 	}
	else{
		__NOP();
	}

	// 2.9 Activamos el modulo serial.
	if(ptrUsartHandler -> USART_Config.USART_mode != USART_MODE_DISABLE){
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_UE;
	}
	else{
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~ USART_CR1_UE;
	}

	__enable_irq();
}

/* funcion para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, char dataToSend ){
	/* Verificar si el dato se transmitio = 1,si no se transmitio = 0
	 * si esta ocupado un registro es 0 y el otro es 1, lo que hace que
	 * ! = True y & = True lo que hace que entre al ciclo y no haga nada
	 * */
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}
	/*Si esta libre se manda el char*/
	ptrUsartHandler->ptrUSARTx->DR = dataToSend;
	return dataToSend;
}

/* Función para escribir un mensaje */
void writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend){
	while(*msgToSend != '\0'){
		writeChar(ptrUsartHandler, *msgToSend);
		msgToSend++;
	}
}

/* Funcion para leer un solo char */
char readChar(USART_Handler_t *ptrUsartHandler){
	// Leemos el valor recibido en el data register
	char dataToRead;
	dataToRead = ((char) (ptrUsartHandler -> ptrUSARTx -> DR));
	return dataToRead;
}
/* Lectura del caracter que llega por la interfaz serial*/
uint8_t getRxData (void){
	return auxRxData;
}

/*
 *  Handler de la interrupción del USART
 *  Acá deben estar todas las interrupciones asociadas: TX, RX, PE...
 */

/* Interrupciones asociadas a RX*/
void USART1_IRQHandler (void){
	// Evaluamos si la interrupción que se dió es por RX
	if (USART1 -> SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART1 -> DR;
		usart1Rx_Callback();
	}
}

void USART2_IRQHandler (void){
	// Evaluamos si la interrupción que se dió es por RX
	if (USART2 -> SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART2 -> DR;
		usart2Rx_Callback();
	}
}

void USART6_IRQHandler (void){
	// Evaluamos si la interrupción que se dió es por RX
	if (USART6 -> SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART6 -> DR;
		usart6Rx_Callback();
	}
}

__attribute__((weak)) void usart1Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void usart2Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void usart6Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
