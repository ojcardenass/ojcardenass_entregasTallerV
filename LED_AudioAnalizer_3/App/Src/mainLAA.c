/**
 ******************************************************************************
 * @file           : main.c
 * @title		   : Proyecto LED Audio Analizer
 * @author         : ojcardenass
 * @Nombre         : Oscar Julian Cardenas Sosa
 * @brief          : Uso de SPI, ADC y FFT para mostrar frecuencias de audio en
 * 					 una matrix LED 32 x 8
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
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"
#include "SPIxDriver.h"
#include "RTCDriver.h"
#include "ADCDriver.h"
#include "MAX7219Driver.h"

#include "arm_math.h"

/* ------------------------------ HANDLERS ------------------------------ */
GPIO_Handler_t			handlerBlinkyPin		=	{0};
GPIO_Handler_t			handlerMCO1				=	{0};
BasicTimer_Handler_t	handlerBlinkyTimer		=	{0};
BasicTimer_Handler_t	handlerRefreshTimer		=	{0};
/* ----- 	SPI 	---*/
GPIO_Handler_t			handlerCLK				=	{0};
GPIO_Handler_t			handlerMOSI				=	{0};
/* -----	USART	---*/
GPIO_Handler_t			handlerPinTX			=	{0};
GPIO_Handler_t			handlerPinRX			=	{0};
USART_Handler_t			usart2Comm				=	{0};
/* -----	PWM		---*/
GPIO_Handler_t			handlerPinPwmChannel	=	{0};
PWM_Handler_t			handlerEventTimer		= 	{0};
/* -----	ADC		---*/
ADC_Config_t			adcConfig				= 	{0};

/* ------------------------------ VARIABLES ------------------------------ */
/* Constantes */
#define SIZE_FFT 2048
/* Banderas	*/
uint8_t					flagcomplete			=	0;
uint8_t					flagADC					=	0;
uint16_t				counter					=	0;
uint8_t					flagRefresh				=	0;
uint8_t					flag200hz				=	0;
uint8_t 				freqMagArray[32] 		=  {0};

/* Elementos para la comunicacion Serial*/
/* ---------- USART ----------*/
uint8_t					rxData					=	0;
uint8_t					counterRx				=	0;
bool					stringComplete			= false;
char					cmd[64]					= {0};
char					userMsg[64]				=	"Hola, voy a dominar el mundo :) ";
unsigned int			firstParameter			=	0;
unsigned int			secondParameter			=	0;
unsigned int			thirdParameter			=	0;
char					bufferData[64] 			= 	"Hola, voy a dominar el mundo :) ";
char					bufferMsg[64] 			= {0};
char 					bufferRx[64];

/* Elementos para el ADC*/
uint16_t 				adcData1[SIZE_FFT] = {0};
uint16_t 				adcDataCounter = 0;
uint8_t 				adcCounter = 0;
bool 					adcIsComplete = false;
/* ---------- FFT ----------*/
/* Elementos para la FFT*/
float32_t				fft_input[SIZE_FFT];
float32_t 				fft_output[SIZE_FFT];
float32_t 				max_value;
float32_t 				mean;
uint8_t 				scaledData[SIZE_FFT/2];
float_t 				freq;
uint32_t 				max_index;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
arm_rfft_fast_instance_f32 fft_instance;

/* Frecuencia del micro*/
int 		clockSpeed = SPEED_100MHz;
int 		clock = 0;

/* ------------------------------ FUNCIONES ------------------------------ */
void saveData(void);
void init_Hardware(void);
void parseCommands(char *ptrBufferRx);
void doFFT(void);

/* ------------------------------	 MAIN	 ------------------------------ */
int main(void){
    /* Inicializamos todos los elementos del sistema */
	init_Hardware();
	MAX7219_Init();
	/* Loop forever */
	while(1){

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

		if(adcIsComplete){
			/* Si se activo la bandera de resfresco se realiza la FFT y se envia a la matriz */
			if(flagRefresh){
				/* Pendiente de ubicacion */
				doFFT();
				// Se seleccionan las frecuencias a mostrar en la matriz LED
				// Freq * (2048/(48000/2)) = Index
				freqMagArray[0] = scaledData[1];	// 12 Hz
				freqMagArray[1] = scaledData[2];	// 23 Hz
				freqMagArray[2] = scaledData[3];	// 35 Hz
				freqMagArray[3] = scaledData[4];	// 47 Hz
				freqMagArray[4] = scaledData[5];	// 59 Hz
				freqMagArray[5] = scaledData[6];	// 70 Hz
				freqMagArray[6] = scaledData[9];	// 105 Hz
				freqMagArray[7] = scaledData[11];	// 129 Hz
				freqMagArray[8] = scaledData[15];	// 176 Hz
				freqMagArray[9] = scaledData[20];	// 234 Hz
				freqMagArray[10] = scaledData[26];	// 305 Hz
				freqMagArray[11] = scaledData[35];	// 410 Hz
				freqMagArray[12] = scaledData[47];	// 551 Hz
				freqMagArray[13] = scaledData[51];	// 598 Hz
				freqMagArray[14] = scaledData[63];	// 738 Hz
				freqMagArray[15] = scaledData[83];	// 973 Hz
				freqMagArray[16] = scaledData[86];	// 1 kHz
				freqMagArray[17] = scaledData[111];	// 1.3 kHz
				freqMagArray[18] = scaledData[148];	// 1.74 kHz
				freqMagArray[19] = scaledData[171];	// 2 kHz
				freqMagArray[20] = scaledData[198];	// 2.32 kHz
				freqMagArray[21] = scaledData[264];	// 3.09 kHz
				freqMagArray[22] = scaledData[352];	// 4.12 kHz
				freqMagArray[23] = scaledData[469];	// 5.5 kHz
				freqMagArray[24] = scaledData[512];	// 6 kHz
				freqMagArray[25] = scaledData[625];	// 7.32 kHz
				freqMagArray[26] = scaledData[768];	// 9 kHz
				freqMagArray[27] = scaledData[835];	// 9.78 kHz
				freqMagArray[28] = scaledData[1109];// 13 kHz
				freqMagArray[29] = scaledData[1365];// 16 kHz
				freqMagArray[30] = scaledData[1485];// 17.4 kHz
				freqMagArray[31] = scaledData[1707];// 20 kHz

			    /* Se extraen 4 fragmentos de la lista de frecuencias y sus magnitudes
			     * dentro del for [i * 8] regresa la posicion inicial del fragmento, y desde
			     * alli la funcion setColumnsModx, recorre desde esa posicion 8 veces, y
			     * asi enviar el valor de la magnitud a la columna correspondiente. i+1 retorna
			     * el numero del modulo que va desde 1 hasta 4 */
				for (int i = 0; i < 4; i++) {
					setColumnsModx(&freqMagArray[i * 8], i + 1);
				}
			}
			adcIsComplete = false;
		} // Fin del condicional principal

	} // Fin del cliclo infinito
} // Fin del Main

/* Funcion encargada de la inicializacion del sistema*/
void init_Hardware(void){

	// Activamos el coprocesador matematico FPU
	SCB->CPACR |= (0xF << 20);

	/* Calibramos el HSI */
	// (Freq deseada - Freq Real)/48000
	// Limpiamos el HSITRIM
	RCC->CR &= ~(0x1F << RCC_CR_HSITRIM_Pos);
	// Ya que el signo de la operacion es negativo reducimos la frecuencia en 5 pasos
	RCC->CR |= (11 << RCC_CR_HSITRIM_Pos);

	configPLL(clockSpeed);
	config_SysTick_ms(CLKSPEED);

	/* ------------------------------ LED de estado ------------------------------ */
	// Configurando el pin H1 para el Led blinky
	handlerBlinkyPin.pGPIOx 								= GPIOH;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber     		= PIN_1;
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

	/* ------------------------------ USART2 ------------------------------ */
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

	/* ------------------------------ SPI ------------------------------ */
	handlerCLK.pGPIOx										= GPIOB;
	handlerCLK.GPIO_PinConfig.GPIO_PinNumber				= PIN_13;
	handlerCLK.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerCLK.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerCLK.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_HIGH;
	handlerCLK.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerCLK.GPIO_PinConfig.GPIO_PinAltFunMode			= AF5;
	GPIO_Config(&handlerCLK);

	handlerMOSI.pGPIOx										= GPIOB;
	handlerMOSI.GPIO_PinConfig.GPIO_PinNumber				= PIN_15;
	handlerMOSI.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerMOSI.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerMOSI.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerMOSI.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerMOSI.GPIO_PinConfig.GPIO_PinAltFunMode			= AF5;
	GPIO_Config(&handlerMOSI);

	//Se configura el Timer 4 para que genere la bandera del refresco de la matrix LED
	handlerRefreshTimer.ptrTIMx 							= TIM4;
	handlerRefreshTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerRefreshTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerRefreshTimer.TIMx_Config.TIMx_period				= 50;
	handlerRefreshTimer.TIMx_Config.TIMx_interruptEnable	= 1;
	BasicTimer_Config(&handlerRefreshTimer);

	/* ------------------------------ ADC ------------------------------ */
	/* Se configura el multicanal del ADC junto al canal de la interrupcion externa */
	// ADC Clock = CLK/4 = 25 MHz | T_conv = (Samp Time + Cycles) / ADC Clock
	// T_conv = (84 + 12 ) / 25 MHz = 3.84 us
	// T_conv = (112 + 12 ) / 25 MHz = 4.96 us
	// El tiempo de conversion debe ser menor al tiempo de sampleo, 48 kHz = 21 us
	adcConfig.channel										= ADC_CHANNEL_0;
	adcConfig.dataAlignment									= ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution									= ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod								= ADC_SAMPLING_PERIOD_84_CYCLES;
	adcConfig.eventExt										= TIM5_CH3;
	adcConfig.flank											= EXTERNAL_INTERRUPT_FALLING_EDGE;
	adc_Config(&adcConfig);

	/* ------------------------------ PWM  ------------------------------ */
	/* Configurando el PWM para el que genere el la interrupcion externa en el ADC
	 * con un muestreo de 48kHz = 21us */
	handlerEventTimer.ptrTIMx								= TIM5;
	handlerEventTimer.config.channel						= PWM_CHANNEL_3;
	handlerEventTimer.config.duttyCicle						= 5;
	handlerEventTimer.config.periodo						= 21;
	handlerEventTimer.config.prescaler						= 100;
	pwm_Config(&handlerEventTimer);
	/* Se activa la salida del evento externo */
	enableOutput(&handlerEventTimer);
	/* Se configuran las interrupciones externas del ADC */
	adcExternalConfig(&adcConfig);
} // Fin del init_Hardware

/* Funcion de idenficacion de comandos */
void parseCommands(char *ptrBufferRx){
	sscanf(ptrBufferRx, "%s %u %u %u %s", cmd,&firstParameter,&secondParameter,&thirdParameter,userMsg);

	/* Se analizan los comnados*/
	if(strcmp(cmd, "help") == 0){
		writeMsg(&usart2Comm, "\n");
		writeMsg(&usart2Comm, "||| Command Reference and Actions: |||\n");
		writeMsg(&usart2Comm, "\n");
		writeMsg(&usart2Comm, " IMPORTANT! All commands must end with: # \n");
		writeMsg(&usart2Comm, "\n");
		writeMsg(&usart2Comm, "1 | help					-- Display the command reference menu \n");
		writeMsg(&usart2Comm, "2 | clkscMCO1 #A			-- Change the source clock for MCO1 pin: 1 = HSI, 2 = LSE, 3 = PLL \n");
		writeMsg(&usart2Comm, "3 | prescalerMCO1 #A		-- Set prescaler division for MCO1 pin (1 to 5) \n");;
	}
}

void doFFT(void){
	statusInitFFT = arm_rfft_fast_init_f32(&fft_instance, SIZE_FFT);

	if(statusInitFFT == ARM_MATH_SUCCESS){
		// Se copia el array de los datos del ADC, para mantenerlos separados del FFT
		memcpy(fft_input,adcData1,SIZE_FFT);
		/* Se realiza la transformada de fourier*/
		arm_rfft_fast_f32(&fft_instance, fft_input, fft_output, 0);
		/* Se obtiene la amplitud de los pares de vectores re[n], im[n]*/
		arm_cmplx_mag_f32(fft_output, fft_output, SIZE_FFT/2);
		//fft_output[0] = 0;
		/* Para limpiar la señal se obtiene la media de todo el array*/
		arm_mean_f32(fft_output, SIZE_FFT, &mean);
		/* Se iguala la primera posicion del DC Offset al promedio de los datos, para
		 * evitar un pico en 0 Hz, debido a las fluctuaciones o variaciones irregulares*/
		fft_output[0] = mean;
		/* Se resta de todo el array, la media de los datos para normalizar un poco la señal*/
		//arm_offset_f32(fft_output, -mean, fft_output, SIZE_FFT/2);

		/* Se crea un arreglo temporal para guardar los valores de la transformada
		 * pero en escala de decibeles */
		float32_t dBData[SIZE_FFT/2];
		// Se convierte la magnitud del espectro a dB
		for (int i = 0; i < SIZE_FFT/2; i++) {
			dBData[i] = 20.0f * log10f(fft_output[i]);
		}

		/* Para hacer uso del driver MAX7219, es necesario escalar las medidas
		 * entre 0 y 8*/

		// Se encuentra el valor maximo en dB
		float32_t maxDB = 0;
		arm_max_f32(dBData, SIZE_FFT/2, &maxDB, &max_index);

		// Se escalan los valores segun el maximo valor, y se convierten a uint8_t
		for (int i = 0; i < SIZE_FFT; i++) {
			float32_t scaledValue = (dBData[i] / maxDB) * 8.0f;
			scaledData[i] = (uint8_t)(scaledValue + 0.5f);  // Se redondea al entero mas cercano
		}
	}
} // Fin doFFT

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
}

/* Callback para hacer el refresco de la matriz LED*/
void BasicTimer4_Callback(void){
	flagRefresh = 1;
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido por
 * el puerto del USART2*/
void usart2Rx_Callback (void){
	rxData = getRxData();
}

/* Callback del ADC */
void adcComplete_Callback(void){
	adcData1[adcDataCounter] = getADC();
	if(adcDataCounter == SIZE_FFT){
		adcDataCounter = 0;
		stopPwmSignal(&handlerEventTimer);
		adcIsComplete = true;
	}
	adcDataCounter++;

}

