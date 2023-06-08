/**
 ******************************************************************************
 * @file           : main.c
 * @title		   : Solucion Examen
 * @author         : ojcardenass
 * @Nombre         : Oscar Julian Cardenas Sosa
 * @brief          : Uso de PLL calibrado, I2C, ADC y USART
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"
#include "I2CxDriver.h"
#include "RTCDriver.h"
#include "LCDDriver.h"

GPIO_Handler_t			handlerBlinkyPin		=	{0};
GPIO_Handler_t			handlerMCO1				=	{0};
BasicTimer_Handler_t	handlerBlinkyTimer		=	{0};
BasicTimer_Handler_t	handlerSampleTimer		=	{0};

/* Elementos para la comunicacion Serial*/
GPIO_Handler_t			handlerPinTX			=	{0};
GPIO_Handler_t			handlerPinRX			=	{0};
USART_Handler_t			usart2Comm				=	{0};
uint8_t					rxData					=	0;
uint8_t					counterRx				=	0;
bool					stringComplete			= 	false;
char					cmd[64]					= 	{0};
char					userMsg[64]				=	"Hola, voy a dominar el mundo :) ";
unsigned int			firstParameter			=	0;
unsigned int			secondParameter			=	0;
unsigned int			thirdParameter			=	0;
char	bufferData[64] = "Hola, voy a dominar el mundo :) ";
char	bufferMsg[64] = {0};
char 	bufferRx[64];

/* Configuracion para el I2C del Acelerometro*/
GPIO_Handler_t			handlerI2cSDA			= 	{0};
GPIO_Handler_t			handlerI2cSCL			= 	{0};
I2C_Handler_t			handlerAccelerometer	=	{0};
uint8_t					i2cBuffer				=	0;

#define ACCEL_ADDRESS 	0b01101000
#define ACCEL_XOUT_H 	59	//0x3B
#define ACCEL_XOUT_L 	60	//0x3C
#define ACCEL_YOUT_H 	61	//0x3D
#define ACCEL_YOUT_L 	62	//0x3E
#define ACCEL_ZOUT_H 	63	//0x3F
#define ACCEL_ZOUT_L 	64	//0x40

#define PWR_MGMT_1 		107
#define WHO_AM_I		117

// Elementos del RTC
RTC_Handler_t 				handlerRTC			= {0};

// Variables necesarias para obtener valores de fecha y hora
uint8_t hour = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;
uint8_t year = 0;
uint8_t day = 0;
uint8_t month = 0;
uint8_t weekDay = 0;
uint8_t maxDay = 31;

/* Frecuencia del micro*/
int 		clockSpeed = SPEED_100MHz;
int 		clock = 0;

void init_Hardware(void);
void parseCommands(char *ptrBufferRx);

int main(void){
    /* Inicializamos todos los elementos del sistema */
	init_Hardware();

	/* Loop forever */
	while(1){
		// Obtenemos los valores de la fecha y la hora
//		hour = getHours(handlerRTC);
//		minutes = getMinutes(handlerRTC);
//		seconds = getSeconds(handlerRTC);
//		year = getYears (handlerRTC);
//		month = getMonths(handlerRTC);
//		day = getDays(handlerRTC);
//		weekDay = getWedn(handlerRTC);

		if(rxData != '\0'){
			/*Se crea una cadena de caracteres con los datos que llegan por el serial*/
			bufferRx[counterRx] = rxData;
			counterRx ++;

			/* Si el caracter recibido es #, se activa una bandera para indicar que ha terminado
			 * el comando*/
			if(rxData == '#'){
				stringComplete = true;
				/* Se escribe el caracter nulo en el arreglo para que sea el final del string*/
				bufferRx[counterRx] = '\0';
				counterRx = 0;
			}
			/* Se termina la interrupcion*/
			rxData = '\0';
		}

		/* Se analiza la cadena de datos obtenida*/
		if(stringComplete){
			parseCommands(bufferRx);
			stringComplete = false;
		}
	}
}

/* Funcion encargada de la inicializacion del sistema*/
void init_Hardware(void){

	// Activamos el coprocesador matematico FPU
	SCB->CPACR |= (0xF << 20);

	configPLL(clockSpeed);
	config_SysTick_ms(CLKSPEED);

	/* Calibramos el HSI */
	// (Freq deseada - Freq Real)/48000
	//RCC->CR |= RCC_CR_HSITRIM_Pos;

	/* ------------------------------ LED de estado ------------------------------ */
	// Configurando el pin A5 para el Led blinky
	handlerBlinkyPin.pGPIOx 								= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber     		= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode        	= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType      	= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed       	= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	//Se carga la configuracion del HandlerBlinkyPin, para el led de estado.
	GPIO_Config(&handlerBlinkyPin);

	//Se configura el Timer 2 para que funcione con el blinky
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable		= 1;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* ------------------------------ MCO1 ------------------------------ */
	// Configurando el pin A8 para el Microcontroller Clock Output 1
	handlerMCO1.pGPIOx 										= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber     			= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode        			= GPIO_MODE_OUT;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType      			= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed       			= GPIO_OSPEED_FAST;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
	GPIO_Config(&handlerMCO1);

	/* ------------------------------ USART6 ------------------------------ */
	/* Configuracion para la comunicacion serial USART2*/
	handlerPinTX.pGPIOx										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx										= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx									= USART2;
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_interruptionEnableRx		= USART_RX_INTERRUPT_ENABLE;
	usart2Comm.USART_Config.USART_interruptionEnableTx		= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart2Comm);

	/* ------------------------------ Acelerometro ------------------------------ */
	/* Configurando los pines sobre los que funciona el I2C*/
	handlerI2cSCL.pGPIOx 									= GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber 			= PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_HIGH;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2cSCL);

	handlerI2cSDA.pGPIOx 									= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_HIGH;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2cSDA);

	handlerAccelerometer.ptrI2Cx							= I2C1;
	handlerAccelerometer.modeI2C							= I2C_MODE_FM;
	handlerAccelerometer.slaveAddress						= ACCEL_ADDRESS;
	i2c_config(&handlerAccelerometer);

	//Se configura el Timer 4 para que genere la bandera del muestro
	handlerSampleTimer.ptrTIMx 								= TIM4;
	handlerSampleTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerSampleTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerSampleTimer.TIMx_Config.TIMx_period				= 2500;
	handlerSampleTimer.TIMx_Config.TIMx_interruptEnable		= 1;
	BasicTimer_Config(&handlerSampleTimer);

	/* ------------------------------ RTC ------------------------------ */
	// Configuración inicial de la fecha y hora
	handlerRTC.hours												= 1;
	handlerRTC.minutes												= 26;
	handlerRTC.seconds												= 00;
	handlerRTC.year													= 22;
	handlerRTC.month												= 6;
	handlerRTC.weekDay												= 5;
	handlerRTC.day													= 8;
	rtc_config(&handlerRTC);

} // Fin del init_Hardware

void parseCommands(char *ptrBufferRx){
	sscanf(ptrBufferRx, "%s %u %u %u %s", cmd,&firstParameter,&secondParameter,&thirdParameter,userMsg);

	/* Se analizan los comnados*/
	if(strcmp(cmd, "help") == 0){
		writeMsg(&usart2Comm, "||| Command Reference and Actions: |||\n");
		writeMsg(&usart2Comm, "\n");
		writeMsg(&usart2Comm, " IMPORTANT! All commands must end with: # \n");
		writeMsg(&usart2Comm, "\n");
		writeMsg(&usart2Comm, "1 | help \t\t\t-- Print this menu \n");
		writeMsg(&usart2Comm, "2 | clkscMCO1 #A -- Change the source clock for MCO1 pin: 1 = HSI, 2 = LSE, 3 = PLL \n");
		writeMsg(&usart2Comm, "3 | prescalerMCO1 #A -- Set prescaler division for MCO1 pin from 1 to 5 \n");
		writeMsg(&usart2Comm, "4 | setTime #A #B #C -- Set time for RTC. Format: #HH #MM #SS (hours,minutes,seconds) \n");
		writeMsg(&usart2Comm, "5 | setDate #A #B #C -- Set date for RTC. Format: #D #M #Y (day,month,year) \n");
		writeMsg(&usart2Comm, "6 | getTime  -- Displays time of RTC. Format: #HH #MM #SS (hours,minutes,seconds) \n");
		writeMsg(&usart2Comm, "7 | getDate  -- Displays date of RTC. Format: #D #M #Y (day,month,year) \n");
		/* Comandos pendientes por configurar */
		writeMsg(&usart2Comm, "8 | dummy #A #B \t\t -- dummy cmd, #A and #B are uint32_t \n");
		writeMsg(&usart2Comm, "9 | usermsg # # msg \t-- msg is a string coming from outside \n");
		writeMsg(&usart2Comm, "10| initLCD \t\t\t -- steps to initialize the LCD \n");
		writeMsg(&usart2Comm, "11| dummy #A #B \t\t -- dummy cmd, #A and #B are uint32_t \n");
		writeMsg(&usart2Comm, "12| usermsg # # msg \t-- msg is a string coming from outside \n");
		writeMsg(&usart2Comm, "13| initLCD \t\t\t -- steps to initialize the LCD \n");
		writeMsg(&usart2Comm, "14| setPeriod #A \t\t -- Change the LED_state period (µs) \n");

	}

	else if(strcmp(cmd, "clkscMCO1") == 0){
			writeMsg(&usart2Comm, " Command: clkscMCO1 \n");
			if(firstParameter >= 1 && firstParameter <= 3){
				switch (firstParameter) {
					/* HSI */
					case 1:
						// HSI clock selected
						RCC -> CFGR &= ~RCC_CFGR_MCO1;
						writeMsg(&usart2Comm, "Clock source for MCO1 set to HSI \n");
						break;
					/* LSE */
					case 2:
						// LSE clock selected
						RCC -> CFGR |= RCC_CFGR_MCO1_0;
						writeMsg(&usart2Comm, "Clock source for MCO1 set to LSE \n");
						break;
					/* PLL */
					case 3:
						// PLL clock selected
						RCC -> CFGR |= RCC_CFGR_MCO1;
						writeMsg(&usart2Comm, "Clock source for MCO1 set to PLL \n");
						break;
					default:
						__NOP();
						break;
				}
			}
			else{
				/* Se imprime un mensaje para avisar que el valor no esta definido */
				writeMsg(&usart2Comm, "Value outside the range 1 to 3, please try again \n");

			}
	}

	else if(strcmp(cmd, "prescalerMCO1") == 0){
		writeMsg(&usart2Comm, " Command: prescalerMCO1 \n");
		if(firstParameter >= 1 && firstParameter <= 5){
			switch (firstParameter) {
				/* Sin division*/
				case 1:
					// Factor de división (prescaler) = 1
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					writeMsg(&usart2Comm, " Prescaler set to 1 \n");
					break;
				/* Dividido 2*/
				case 2:
					// Factor de división (prescaler) = 2,
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
					writeMsg(&usart2Comm, " Prescaler set to 2 \n");
					break;
				/* Dividido 3*/
				case 3:
					// Factor de división (prescaler) = 3
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_0;
					writeMsg(&usart2Comm, " Prescaler set to 3 \n");
					break;
				/* Dividido 4*/
				case 4:
					// Factor de división (prescaler) = 4
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_1;
					writeMsg(&usart2Comm, " Prescaler set to 4 \n");

					break;
				/* Dividido 5*/
				case 5:
					// Factor de división (prescaler) = 5
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE;
					writeMsg(&usart2Comm, " Prescaler set to 5 \n");
					break;
				default:
					__NOP();
					break;
			}
		}
		else{
			/* Se imprime un mensaje para avisar que el valor no esta definido */
			writeMsg(&usart2Comm, " Value outside the range 1 to 5, please try again \n");

		}
	}

	else if(strcmp(cmd, "setTime") == 0){
		writeMsg(&usart2Comm, " Command: setTime \n");
		// Validacion de la hora
		if(firstParameter < 0 || firstParameter > 23){
			writeMsg(&usart2Comm, " Value outside the range of hours 0 to 23, please try again \n");
		}
		// Validacion de los minutos
		if(secondParameter < 0 || secondParameter > 59){
			writeMsg(&usart2Comm, " Value outside the range of minutes 0 to 59, please try again \n");
		}
		// Validacion de los segundos
		if(thirdParameter < 0 || thirdParameter > 59){
			writeMsg(&usart2Comm, " Value outside the range of seconds 0 to 59, please try again \n");
		}
		else{
			handlerRTC.hours = firstParameter;
			handlerRTC.minutes = secondParameter;
			handlerRTC.seconds = thirdParameter;
			rtc_config(&handlerRTC);

			sprintf(bufferData, " Selected time: %02u:%02u:%02u \n", firstParameter,secondParameter,thirdParameter);
			writeMsg(&usart2Comm, bufferData);
		}
	}

	else if(strcmp(cmd, "setDate") == 0){
		writeMsg(&usart2Comm, " Command: setDate \n");
		// Validacion de los meses
		if (secondParameter < 1 || secondParameter > 12){
			/* Se imprime un mensaje para avisar que el valor no esta definido */
			writeMsg(&usart2Comm, " Value outside the range of months 1 to 12, please try again \n");
		}
		// Validacion de los años 2000 y 2099
		if (thirdParameter < 00 || thirdParameter > 99) {
			/* Se imprime un mensaje para avisar que el valor no esta definido */
			writeMsg(&usart2Comm, " Value outside the range of years 2000 to 2099, please try again \n");
		}
		// Si tanto el mes como el año son validos, se validan los dias maximos
		if (secondParameter >= 1 && secondParameter <= 12 && thirdParameter >= 00 && thirdParameter <= 99) {
			// Validacion de los dias maximos segun el mes y el año
			switch (secondParameter) {
				case 2: // Febrero
					// Se evalua si es bisiesto o no
					if (((thirdParameter + 2000) % 4 == 0 && (thirdParameter + 2000) % 100 != 0) || (thirdParameter + 2000) % 400 == 0) {
						maxDay = 29; // Año bisiesto, Febrero tiene 29 dias
					}
					else {
						maxDay = 28; // Año normal, Febrero tiene 28 days
					}
					break;
				case 4: // Abril
				case 6: // Junio
				case 9: // Septiembre
				case 11: // Noviembre
					maxDay = 30; // Abril, Junio, Septiembre, Noviembre tienen 30 dias
					break;
				default:
					maxDay = 31; // Other months have 31 days
					break;
			}
			/* Si los valores de los dias estan por fuera del maximo permitido se envia un mensaje */
			if (firstParameter < 1 || firstParameter > maxDay) {
				writeMsg(&usart2Comm, " Value outside the range of days \n");
				sprintf(bufferData, " The maximum day for month %02u in year %02u, is %02u \n", secondParameter,thirdParameter,maxDay);
				writeMsg(&usart2Comm, bufferData);
			}
			/* De los contrario, si son validos se configuran y se reporta en la terminal*/
			else{
				handlerRTC.day = firstParameter;
				handlerRTC.month = secondParameter;
				handlerRTC.year = thirdParameter;
				rtc_config(&handlerRTC);

				sprintf(bufferData, " Selected date: %02u-%02u-%02u \n", firstParameter,secondParameter,thirdParameter);
				writeMsg(&usart2Comm, bufferData);
			}
		}
	}

	else if(strcmp(cmd, "getDate") == 0){
		day = getDays(handlerRTC);
		month = getMonths(handlerRTC);
		year = getYears(handlerRTC);

		writeMsg(&usart2Comm, " Command: getDate \n");
		sprintf(bufferData, " Current date = %02u-%02u-%02u \n", day,month,year);
		writeMsg(&usart2Comm, bufferData);
	}

	else if(strcmp(cmd, "getTime") == 0){
		hour = getHours(handlerRTC);
		minutes = getMinutes(handlerRTC);
		seconds = getSeconds(handlerRTC);

		writeMsg(&usart2Comm, " Command: getTime \n");
		sprintf(bufferData, " Current time = %02u:%02u:%02u \n", hour,minutes,seconds);
		writeMsg(&usart2Comm, bufferData);
	}




	else if(strcmp(cmd, "dummy") == 0){
		writeMsg(&usart2Comm, " Command: dummy \n");
		sprintf(bufferData, "Number A = %u \n", firstParameter);
		writeMsg(&usart2Comm, bufferData);

		sprintf(bufferData, "Number B = %u \n", secondParameter);
		writeMsg(&usart2Comm, bufferData);
	}

	else if(strcmp(cmd, "usermsg") == 0){
		writeMsg(&usart2Comm, "Command: usermsg \n");
		writeMsg(&usart2Comm, userMsg);
		writeMsg(&usart2Comm, "\n");
	}

	else if(strcmp(cmd, "setPeriod") == 0){
		writeMsg(&usart2Comm, "Command: setPeriod \n");
		if(firstParameter < 100){
			firstParameter = 100;
		}
		if(firstParameter > 10000){
			firstParameter = 10000;
		}
		/* Actualizamos el nuevo perio para el timer*/
		handlerBlinkyTimer.TIMx_Config.TIMx_period = firstParameter;
		BasicTimer_Config(&handlerBlinkyTimer);
	}

	else{
		/* Se imprime el mensaje "Caution!! Wrong Command, try again" para avisar que el comando
		 * no esta definido */
		writeMsg(&usart2Comm, "Caution!! Wrong Command, please try again. \n");
		writeMsg(&usart2Comm, "Type: help # for command menu \n");
	}

}

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido por
 * el puerto del USART2*/
void usart2Rx_Callback (void){
	rxData = getRxData();
}


