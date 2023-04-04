/*
 * Main.c
 *
 *  Created on: Mar 24, 2023
 *      Author: ojcardenass
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

// Definición de los handlers necesarios
GPIO_Handler_t 				handlerLED2		 			=	{0};
GPIO_Handler_t				handlerPinClock			 	=	{0};
GPIO_Handler_t				handlerPinData				=	{0};
GPIO_Handler_t				handlerEncButton			=	{0};

// Timer encargado del Blinky y del 7 segmentos
BasicTimer_Handler_t 		handlerBlinkyTimer 			=	{0};
BasicTimer_Handler_t		handlerDisplay				=	{0};

// Handlers necesarios para cada uno de los leds de los 7 segmentos y
// para activar la base de los transistores
GPIO_Handler_t				handlerSegA					=	{0};
GPIO_Handler_t				handlerSegB					=	{0};
GPIO_Handler_t				handlerSegC					=	{0};
GPIO_Handler_t				handlerSegD					=	{0};
GPIO_Handler_t				handlerSegE					=	{0};
GPIO_Handler_t				handlerSegF					=	{0};
GPIO_Handler_t				handlerSegG					=	{0};

GPIO_Handler_t				handlerBJT1					=	{0};
GPIO_Handler_t				handlerBJT2					=	{0};

// Elementos necesarios para configurar el EXTI
EXTI_Config_t				handlerPinExti				=	{0};
EXTI_Config_t				handlerPinExti2				=	{0};
uint8_t						fallingEdge					=	0;
uint8_t 					encoderButton				=	0;

// Contadores
uint8_t						counter						=	0;
uint8_t						counterEnc					=	0;

// Banderas de estado para hacer el blinky de los display
uint8_t 					flagUni						=	0;
uint8_t						flagDec						=	1;

/*Prototipo de las funciones del main*/
void init_Hardware(void);
void display_digits(uint8_t number);
void display_snake(uint8_t number);
void updateEncoderCounter(GPIO_Handler_t *handler1, uint8_t *extiFlag,
uint8_t upperLimit, uint8_t lowerLimit, uint8_t *counter, uint8_t runCycle);

int main(void){
	/*Inicializacion de todos los elementos del sistema*/
	init_Hardware();

	while(1){
		// Modo normal
		if(encoderButton == 0){

			/* Punto 3.
			 * La identificacion del giro del encoder se realiza con la funcion updateEncoderCounter,
			 * la cual esta explicada al final del codigo*/

			//LECTURA DE DIRECCION DEL ENCODER
			// Funcion de paso por referencia que modifica el counter segun la direccion de giro
			updateEncoderCounter(&handlerPinData, &fallingEdge, 99, 0, &counter, 0);

			/* Punto 4.
			 * Controlar el display segun la direccion de movimiento*/

			// DISPLAY 7 SEGMENTOS

			// Para encender el display correspondiente a las unidades se
			// enciende el BJ1 y apaga el BJ2. Luego se muestra en pantalla, el contador
			// módulo 10, por ejemplo en el caso 10%10 el residuo es cero, 11%10 es 1
			// y así sucesivamente. Obteniendo los números del 00-10 en orden.
			if (flagUni){
				GPIO_WritePin(&handlerBJT1, 0);
				GPIO_WritePin(&handlerBJT2, 1);
				display_digits(counter % 10);
			}
			// Para el display correspondiente a las decenas, se apaga BJ1 y
			// se enciende el BJ2. En pantalla queda el resultado de contador/10, que al
			// ser una operación entre enteros, y al estar definidos como variable entera, solo
			// habran resultado enteros ejem: 11/10 = 1, 22/10 = 2
			if (flagDec){
				GPIO_WritePin(&handlerBJT1, 1);
				GPIO_WritePin(&handlerBJT2, 0);
				display_digits(counter / 10);
			}
		}

		/* Punto 5 y 6.*/
		// MODO CULEBRITA
		else{
			updateEncoderCounter(&handlerPinData, &fallingEdge, 11, 0, &counterEnc, 1);

			// DISPLAY 7 SEGMENTOS
			switch(counterEnc){
				// Unidades
				case 0:
				case 5:
				case 6:
				case 9:
				case 10:
				case 11:
					GPIO_WritePin(&handlerBJT1, 0);
					GPIO_WritePin(&handlerBJT2, 1);
					display_snake(counterEnc);
					break;
				// Decenas
				case 1:
				case 2:
				case 3:
				case 4:
				case 7:
				case 8:
					GPIO_WritePin(&handlerBJT1, 1);
					GPIO_WritePin(&handlerBJT2, 0);
					display_snake(counterEnc);
					break;
			} // Fin Switch case
		} // Fin Else Culebrita

	} // FIN CICLO INFINITO
} // FIN DEL MAIN


void init_Hardware(void){
	/*Configuracion del LED2 - PA5*/
	handlerLED2.pGPIOx 										= GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed  				= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;

	/*Cargar la configuracion del LED*/
	GPIO_Config(&handlerLED2);
	// Se inicia con el LED2 encendido
	GPIO_WritePin(&handlerLED2, SET);

	// Configuracion del TIM2 para que haga un blinky cada 250 ms
	handlerBlinkyTimer.ptrTIMx								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 250; //Lanza una interrupcion cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;

	// Cargando la configuracion del TIM2 en los registros
	BasicTimer_Config(&handlerBlinkyTimer);

	// Configuración del puerto para el clock del encoder, se configura como
	// una entrada digital simple, el encoder ya es pull-up [PB12]
	handlerPinClock.pGPIOx									= GPIOC;
	handlerPinClock.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerPinClock.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerPinClock.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;

    //Cargando la configuracion en los registros
	GPIO_Config(&handlerPinClock);

    handlerPinExti.edgeType									= EXTERNAL_INTERRUPT_FALLING_EDGE; //flancos de bajada
    handlerPinExti.pGPIOHandler								= &handlerPinClock;

    // Cargando la configuracion del EXTI
    extInt_Config(&handlerPinExti);

	// Configuración del puerto para el data del encoder, se configura como
	// una entrada digital simple, el encoder ya es pull-up [PA11]
	handlerPinData.pGPIOx									= GPIOA;
	handlerPinData.GPIO_PinConfig.GPIO_PinNumber			= PIN_11;
	handlerPinData.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerPinData.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;

    //Cargando la configuracion en los registros
	GPIO_Config(&handlerPinData);

	// Configuración del botón del encoder PA12
	handlerEncButton.pGPIOx									= GPIOA;
	handlerEncButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;
	handlerEncButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerEncButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerEncButton.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerEncButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerEncButton);

    handlerPinExti2.edgeType								= EXTERNAL_INTERRUPT_FALLING_EDGE; //flancos de bajada
    handlerPinExti2.pGPIOHandler							= &handlerEncButton;
    // Cargando la configuracion del EXTI
    extInt_Config(&handlerPinExti2);

    //CONFIGURACION DE LOS PINES DE LOS SEGMENTOS
	// Configuración del pin para el led del segmento A del 7 segmentos
	handlerSegA.pGPIOx										= GPIOC;
	handlerSegA.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerSegA.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerSegA.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerSegA.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerSegA.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSegA);

	// Configuración del pin para el led del segmento B del 7 segmentos
	handlerSegB.pGPIOx										= GPIOC;
	handlerSegB.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerSegB.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerSegB.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerSegB.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerSegB.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSegB);

	// Configuración del pin para el led del segmento C del 7 segmentos
	handlerSegC.pGPIOx										= GPIOC;
	handlerSegC.GPIO_PinConfig.GPIO_PinNumber				= PIN_1;
	handlerSegC.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerSegC.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerSegC.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerSegC.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSegC);

	// Configuración del pin para el led del segmento D del 7 segmentos
	handlerSegD.pGPIOx										= GPIOC;
	handlerSegD.GPIO_PinConfig.GPIO_PinNumber				= PIN_0;
	handlerSegD.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerSegD.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerSegD.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerSegD.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSegD);

	// Configuración del pin para el led del segmento E del 7 segmentos
	handlerSegE.pGPIOx										= GPIOB;
	handlerSegE.GPIO_PinConfig.GPIO_PinNumber				= PIN_0;
	handlerSegE.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerSegE.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerSegE.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerSegE.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSegE);

	// Configuración del pin para el led del segmento F del 7 segmentos
	handlerSegF.pGPIOx										= GPIOA;
	handlerSegF.GPIO_PinConfig.GPIO_PinNumber				= PIN_4;
	handlerSegF.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerSegF.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerSegF.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerSegF.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSegF);

	// Configuración del pin para el led del segmento G del 7 segmentos
	handlerSegG.pGPIOx										= GPIOA;
	handlerSegG.GPIO_PinConfig.GPIO_PinNumber				= PIN_1;
	handlerSegG.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerSegG.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerSegG.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerSegG.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSegG);

	// CONFIGURACION DE LOS TRANSISTORES

	// Configuración del BJ1
	handlerBJT1.pGPIOx										= GPIOB;
	handlerBJT1.GPIO_PinConfig.GPIO_PinNumber				= PIN_5;
	handlerBJT1.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBJT1.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerBJT1.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerBJT1.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerBJT1);

	// Configuración del BJ2
	handlerBJT2.pGPIOx										= GPIOB;
	handlerBJT2.GPIO_PinConfig.GPIO_PinNumber				= PIN_4;
	handlerBJT2.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBJT2.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerBJT2.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerBJT2.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerBJT2);

	// Configuración del Timer 5 para que funcione el display 7 segmentos, se
	// configura con un tiempo de refresco de 120 hz de modo que los display
	// aparentemente están encendidos al tiempo

	handlerDisplay.ptrTIMx 									= TIM5;
	handlerDisplay.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerDisplay.TIMx_Config.TIMx_speed 					= BTIMER_SPEED_1ms;
	handlerDisplay.TIMx_Config.TIMx_period 					= 8;
	handlerDisplay.TIMx_Config.TIMx_interruptEnable			= BTIMER_INTERRUP_ENABLE;

	BasicTimer_Config(&handlerDisplay);

}// Termina el init_Hardware

/* Funcion que modifica valores con paso por referencia, recibe el handler del pin Data,
 * una bandera generada por la interrupcion del pin clock del encoder, el limite superior
 * e inferior del contador, la variable del contador y si el contador funciona en ciclo o no */
void updateEncoderCounter(GPIO_Handler_t *handler1, uint8_t *extiFlag,
uint8_t upperLimit, uint8_t lowerLimit, uint8_t *counter, uint8_t runCycle){
	switch(runCycle){
	case 0:
		//Sin ciclo
		//Si se detecta un flanco de bajada y data en 1
		if(*extiFlag && (GPIO_ReadPin(handler1) == 1)){
		// Cuando el counter llegue a upperLimit queda en upperLimit
			if (*counter >= upperLimit){
				*counter = upperLimit;
			}
			// En otros casos se aumenta uno al counter
			else{
				(*counter) ++;
			}
			// Bajar la bandera del EXTI
			*extiFlag = 0;
		}
		//Si se detecta un flanco de bajada y data en 0
		else if(*extiFlag && (GPIO_ReadPin(handler1) == 0)){
			if(*counter <= lowerLimit){
				*counter = lowerLimit;
			}
			// De lo contrario se resta 1
			else{
				(*counter) --;
			}
			// Bajar la bandera del EXTI
			*extiFlag = 0;
		}
		break;
	case 1:
		//Con ciclo
		//Si se detecta un flanco de bajada y data en 1
		if(*extiFlag && (GPIO_ReadPin(handler1) == 1)){
		// Cuando el counter valga upperLimit queda en lowerLimit
			if (*counter >= upperLimit){
				*counter = lowerLimit;
			}
			// En otros casos se aumenta uno al counter
			else{
				(*counter) ++;
			}
			// Bajar la bandera del EXTI
			*extiFlag = 0;
		}
		//Si se detecta un flanco de bajada y data en 0
		else if(*extiFlag && (GPIO_ReadPin(handler1) == 0)){
			if(*counter <= lowerLimit){
				*counter = upperLimit;
			}
			// De lo contrario se resta 1
			else{
				(*counter) --;
			}
			// Bajar la bandera del EXTI
			*extiFlag = 0;
		}
		break;
	default:
		break;
	} // fin switch
} // Fin funcion update encoder counter

void display_snake(uint8_t number){
	switch (number) {
			case 0:
				//DispUni-A
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 1:
				//DispDec-A
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 2:
				// DispDecF
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, RESET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 3:
				// DispDecE
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, RESET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 4:
				// DispDecD
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 5:
				// DispUniE
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, RESET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 6:
				// DispUniF
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, RESET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 7:
				// DispDecB
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 8:
				// DispDecC
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 9:
				// DisUniD
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 10:
				// DisUniC
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 11:
				// DisUniB
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			default:
				break;
		}
}

void display_digits(uint8_t number){
	switch (number) {
			case 0:
				//Número 0
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, RESET);
				GPIO_WritePin(&handlerSegF, RESET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 1:
				// Número 1
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 2:
				// Número 2
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, SET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, RESET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, RESET);
				break;
			case 3:
				// Número 3
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, RESET);
				break;
			case 4:
				// Número 4
				GPIO_WritePin(&handlerSegA, SET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, RESET);
				GPIO_WritePin(&handlerSegG, RESET);
				break;
			case 5:
				// Número 5
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, RESET);
				GPIO_WritePin(&handlerSegG, RESET);
				break;
			case 6:
				// Número 6
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, SET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, RESET);
				GPIO_WritePin(&handlerSegF, RESET);
				GPIO_WritePin(&handlerSegG, RESET);
				break;
			case 7:
				// Número 7
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, SET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, SET);
				GPIO_WritePin(&handlerSegG, SET);
				break;
			case 8:
				// Número 8
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, RESET);
				GPIO_WritePin(&handlerSegF, RESET);
				GPIO_WritePin(&handlerSegG, RESET);
				break;
			case 9:
				// Número 9
				GPIO_WritePin(&handlerSegA, RESET);
				GPIO_WritePin(&handlerSegB, RESET);
				GPIO_WritePin(&handlerSegC, RESET);
				GPIO_WritePin(&handlerSegD, RESET);
				GPIO_WritePin(&handlerSegE, SET);
				GPIO_WritePin(&handlerSegF, RESET);
				GPIO_WritePin(&handlerSegG, RESET);
				break;
			default:
				break;
		}
}

/* Punto 1.*/
// Blinky del led de estado
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}

/* Punto 2.
 * Interrupcion de la señal del clock con flanco de bajada*/
void callback_extInt7(void){
	//Flag que indica que se detecto un flanco de bajada
	fallingEdge = 1;
}

void BasicTimer5_Callback(void){
	// Cambia el estado cada 8 ms
	flagUni ^= 1;
	flagDec ^= 1;
}

/* Callback del EXTI_ para el botón del encoder*/
void callback_extInt12(void){
	//Cambiar el estado del flag del boton del enconder
	encoderButton ^= 1;
}
