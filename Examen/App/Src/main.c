/**
 ******************************************************************************
 * @file           : main.c
 * @title		   : Solucion Examen
 * @author         : ojcardenass
 * @Nombre         : Oscar Julian Cardenas Sosa
 * @brief          : Uso de PLL calibrado, I2C, ADC, USART y CMSIS-FFT
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
#include "I2CxDriver.h"
#include "RTCDriver.h"
#include "ADCDriver.h"

#include "arm_math.h"

/* ------------------------------ HANDLERS ------------------------------ */
GPIO_Handler_t			handlerBlinkyPin		=	{0};
GPIO_Handler_t			handlerMCO1				=	{0};
BasicTimer_Handler_t	handlerBlinkyTimer		=	{0};
BasicTimer_Handler_t	handlerSampleTimer		=	{0};

/* ------------------------------ VARIABLES ------------------------------ */
/* Constantes */
#define SIZE_FFT 512
/* Banderas*/
uint8_t					flagcomplete			=	0;
uint8_t					flagADC					=	0;
uint16_t				counter					=	0;
uint8_t					flagSampling			=	0;
uint8_t					flag200hz				=	0;
/* Arrays para el muestreo */
float32_t				xValueSample[512] 		=	{0};
float32_t				yValueSample[512] 		= 	{0};
float32_t				zValueSample[512] 		= 	{0};
/* Elementos para la comunicacion Serial*/

/* ---------- USART ----------*/
GPIO_Handler_t			handlerPinTX			=	{0};
GPIO_Handler_t			handlerPinRX			=	{0};
USART_Handler_t			usart6Comm				=	{0};
uint8_t					rxData					=	0;
uint8_t					counterRx				=	0;
bool					stringComplete			= 	false;
char					cmd[64]					= 	{0};
char					userMsg[64]				=	"Hola, voy a dominar el mundo :) ";
unsigned int			firstParameter			=	0;
unsigned int			secondParameter			=	0;
unsigned int			thirdParameter			=	0;
char					bufferData[64] = "Hola, voy a dominar el mundo :) ";
char					bufferMsg[64] = {0};
char 					bufferRx[64];

/*	ACELEROMETRO	*/
GPIO_Handler_t			handlerI2cSDA			= 	{0};
GPIO_Handler_t			handlerI2cSCL			= 	{0};
I2C_Handler_t			handlerAccelerometer	=	{0};
#define ACCEL_ADDRESS 	0b01101000
#define ACCEL_XOUT_H 	59	//0x3B
#define ACCEL_XOUT_L 	60	//0x3C
#define ACCEL_YOUT_H 	61	//0x3D
#define ACCEL_YOUT_L 	62	//0x3E
#define ACCEL_ZOUT_H 	63	//0x3F
#define ACCEL_ZOUT_L 	64	//0x40
#define PWR_MGMT_1 		107
#define WHO_AM_I		117
/* Buffers */
uint8_t					i2cBuffer				=	0;

/* ---------- PWM ----------*/
GPIO_Handler_t			handlerPinPwmChannel	=	{0};
PWM_Handler_t			handlerEventTimer		= 	{0};
/* ---------- ADC ----------*/
ADC_Config_t			adcConfig				= 	{0};
/* Elementos para el ADC*/
uint16_t 				adcData1[256] = {0};
uint16_t 				adcData2[256] = {0};
uint16_t 				adcDataCounter = 0;
uint8_t 				adcCounter = 0;
bool 					adcIsComplete = false;
/* ---------- FFT ----------*/
/* Elementos para la FFT*/
float32_t				fft_input[SIZE_FFT];
float32_t 				fft_output[SIZE_FFT];
float32_t 				max_value;
float32_t 				mean;
float_t 				freq;
uint32_t 				max_index;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
arm_rfft_fast_instance_f32 fft_instance;
/* ---------- RTC ----------*/
RTC_Handler_t 			handlerRTC				= 	{0};
/*	Elementos del RTC	*/
uint8_t 				hour = 0;
uint8_t 				minutes = 0;
uint8_t 				seconds = 0;
uint8_t 				year = 0;
uint8_t 				day = 0;
uint8_t 				month = 0;
uint8_t					weekDay = 0;
uint8_t 				maxDay = 31;

/* Frecuencia del micro*/
int 		clockSpeed = SPEED_100MHz;
int 		clock = 0;

/* ------------------------------ FUNCIONES ------------------------------ */
/* Funciones para leer los registros del acelerometro*/
int16_t readXValue(I2C_Handler_t *ptrHandlerI2C);
int16_t readYValue(I2C_Handler_t *ptrHandlerI2C);
int16_t readZValue(I2C_Handler_t *ptrHandlerI2C);
/* Funcion para guardar los datos en los array*/
void saveData(void);

void init_Hardware(void);
void parseCommands(char *ptrBufferRx);

/* ------------------------------ MAIN ------------------------------ */
int main(void){
    /* Inicializamos todos los elementos del sistema */
	init_Hardware();

	// Se imprime un mensaje de inicio por la terminal serial
	writeMsg(&usart6Comm, "EXAMEN FINAL - Taller V | 2023-01 \n");
	writeMsg(&usart6Comm, "Author: Oscar Julian Cardenas Sosa \n");
	writeMsg(&usart6Comm, "\n");
	writeMsg(&usart6Comm, "Please send the command 'help #' to retrieve a comprehensive list of the available commands. \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⡿⠿⠿⠿⣿⣿⣿⣿⡿⠿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⡇⠀⠀⢀⡿⠟⠋⠁⠀⠀⠀⠀⠸⢿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⡇⠀⠀⣠⡴⢦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠀⢀⣬⢯⣭⡉⠛⠻⢿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣇⣀⣸⡏⠀⠀⢹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠀⢈⣷⠀⠀⠶⣾⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣷⣶⣿⣷⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣿⣷⣶⣾⣿⣷⣶⣶⣿⣿\n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");

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

		/* Se analiza si la bandera de sampleo del acelerometro fue activada */
		if(flag200hz){
			/* Si se dio la orden desde un comando para realizar el sampleo */
			if(flagSampling){
				if(counter > 512){
					flagSampling = 0;
					sprintf(bufferData," Sampling complete ... \n");
					writeMsg(&usart6Comm, bufferData);
					delay_ms(1000);
					flagcomplete = 1;
					counter = 0;
				}
				saveData();
			}
			// Se baja la bandera para continuar recibiendo interrupciones cada 200 hz
			flag200hz = 0;
		}

		if(adcIsComplete){
			for(int i = 0; i < 256; i++){
				sprintf(bufferData,"%u\t%u \n",(unsigned int)adcData1[i],(unsigned int)adcData2[i]);
				writeMsg(&usart6Comm,bufferData);
			}
			adcIsComplete = false;
		}

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

	/* ------------------------------ MCO1 ------------------------------ */
	// Configurando el pin A8 para el Microcontroller Clock Output 1
	handlerMCO1.pGPIOx 										= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber     			= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode        			= GPIO_MODE_ALTFN;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType      			= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed       			= GPIO_OSPEED_FAST;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
	GPIO_Config(&handlerMCO1);

	/* ------------------------------ USART6 ------------------------------ */
	/* Configuracion para la comunicacion serial USART6*/
	handlerPinTX.pGPIOx										= GPIOC;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF8;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx										= GPIOC;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF8;
	GPIO_Config(&handlerPinRX);

	usart6Comm.ptrUSARTx									= USART6;
	usart6Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usart6Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart6Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart6Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart6Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart6Comm.USART_Config.USART_interruptionEnableRx		= USART_RX_INTERRUPT_ENABLE;
	usart6Comm.USART_Config.USART_interruptionEnableTx		= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart6Comm);

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
	handlerSampleTimer.TIMx_Config.TIMx_period				= 50;
	handlerSampleTimer.TIMx_Config.TIMx_interruptEnable		= 1;
	BasicTimer_Config(&handlerSampleTimer);

	/* ------------------------------ RTC ------------------------------ */
	// Configuración inicial de la fecha y hora
	handlerRTC.hours										= 1;
	handlerRTC.minutes										= 26;
	handlerRTC.seconds										= 00;
	handlerRTC.year											= 22;
	handlerRTC.month										= 6;
	handlerRTC.weekDay										= 5;
	handlerRTC.day											= 8;
	rtc_config(&handlerRTC);

	/* ------------------------------ ADC ------------------------------ */
	/* Se configura el multicanal del ADC junto al canal de la interrupcion externa */
	adcConfig.channels[0]									= ADC_CHANNEL_0;
	adcConfig.channels[1]									= ADC_CHANNEL_1;
	adcConfig.dataAlignment									= ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution									= ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod								= ADC_SAMPLING_PERIOD_144_CYCLES;
	adcConfig.eventExt										= TIM5_CH3;
	adcConfig.flank											= EXTERNAL_INTERRUPT_FALLING_EDGE;
	multiChannelConfig(&adcConfig, 2);

	/* ------------------------------ PWM  ------------------------------ */
	/* Configurando el PWM para el que genere el la interrupcion externa en el ADC */
	handlerEventTimer.ptrTIMx								= TIM5;
	handlerEventTimer.config.channel						= PWM_CHANNEL_3;
	handlerEventTimer.config.duttyCicle						= 5;
	handlerEventTimer.config.periodo						= 10;
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
		writeMsg(&usart6Comm, "\n");
		writeMsg(&usart6Comm, "||| Command Reference and Actions: |||\n");
		writeMsg(&usart6Comm, "\n");
		writeMsg(&usart6Comm, " IMPORTANT! All commands must end with: # \n");
		writeMsg(&usart6Comm, "\n");
		writeMsg(&usart6Comm, "1 | help					-- Display the command reference menu \n");
		writeMsg(&usart6Comm, "2 | clkscMCO1 #A			-- Change the source clock for MCO1 pin: 1 = HSI, 2 = LSE, 3 = PLL \n");
		writeMsg(&usart6Comm, "3 | prescalerMCO1 #A		-- Set prescaler division for MCO1 pin (1 to 5) \n");
		writeMsg(&usart6Comm, "4 | setTime #A #B #C 	-- Set time for RTC. Format: #HH #MM #SS (hours,minutes,seconds) \n");
		writeMsg(&usart6Comm, "5 | setDate #A #B #C		-- Set date for RTC. Format: #D #M #Y (day,month,year) \n");
		writeMsg(&usart6Comm, "6 | getTime				-- Display the current time of the RTC. Format: #HH #MM #SS (hours,minutes,seconds) \n");
		writeMsg(&usart6Comm, "7 | getDate				-- Display the current date of the RTC (24 H). Format: #D #M #Y (day,month,year) \n");
		writeMsg(&usart6Comm, "8 | setSampFreq #A		-- Set the sampling frequency in kHz. (Accepted values: 8 to 40) \n");
		writeMsg(&usart6Comm, "9 | displayData			-- Display the sampled data from 2 channels \n");
		writeMsg(&usart6Comm, "10| getAccelData			-- Sample the acceleration of axes x, y, z with a sampling time of 200 Hz \n");
		writeMsg(&usart6Comm, "11| doFTT				-- Calculate and display the data from the Fourier Transform of the accelerometer \n");
		/*Extra Commands*/
		writeMsg(&usart6Comm, "12| setPeriod #A			-- Change the LED_state period (µs) \n");
		writeMsg(&usart6Comm, "13| printF1				-- Show the start page draw \n");


	}

	else if(strcmp(cmd, "clkscMCO1") == 0){
			writeMsg(&usart6Comm, " Command: clkscMCO1 \n");
			if(firstParameter >= 1 && firstParameter <= 3){
				switch (firstParameter) {
					/* HSI */
					case 1:
						// HSI clock selected
						RCC -> CFGR &= ~RCC_CFGR_MCO1;
						writeMsg(&usart6Comm, "Clock source for MCO1 set to HSI \n");
						break;
					/* LSE */
					case 2:
						// LSE clock selected
						RCC -> CFGR |= RCC_CFGR_MCO1_0;
						writeMsg(&usart6Comm, "Clock source for MCO1 set to LSE \n");
						break;
					/* PLL */
					case 3:
						// PLL clock selected
						RCC -> CFGR |= RCC_CFGR_MCO1;
						// Factor de división (prescaler) = 5
						RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
						RCC -> CFGR |= RCC_CFGR_MCO1PRE;
						writeMsg(&usart6Comm, "Clock source for MCO1 set to PLL \n");
						break;
					default:
						// HSI clock selected
						RCC -> CFGR &= ~RCC_CFGR_MCO1;
						break;
				}
			}
			else{
				/* Se imprime un mensaje para avisar que el valor no esta definido */
				writeMsg(&usart6Comm, "Value outside the range 1 to 3, please try again \n");

			}
	}

	else if(strcmp(cmd, "prescalerMCO1") == 0){
		writeMsg(&usart6Comm, " Command: prescalerMCO1 \n");
		if(firstParameter >= 1 && firstParameter <= 5){
			switch (firstParameter) {
				/* Sin division*/
				case 1:
					// Factor de división (prescaler) = 1
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					writeMsg(&usart6Comm, " Prescaler set to 1 \n");
					break;
				/* Dividido 2*/
				case 2:
					// Factor de división (prescaler) = 2,
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
					writeMsg(&usart6Comm, " Prescaler set to 2 \n");
					break;
				/* Dividido 3*/
				case 3:
					// Factor de división (prescaler) = 3
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_0;
					writeMsg(&usart6Comm, " Prescaler set to 3 \n");
					break;
				/* Dividido 4*/
				case 4:
					// Factor de división (prescaler) = 4
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE_1;
					writeMsg(&usart6Comm, " Prescaler set to 4 \n");

					break;
				/* Dividido 5*/
				case 5:
					// Factor de división (prescaler) = 5
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE;
					writeMsg(&usart6Comm, " Prescaler set to 5 \n");
					break;
				default:
					// Factor de división (prescaler) = 5
					RCC -> CFGR &= ~ RCC_CFGR_MCO1PRE;
					RCC -> CFGR |= RCC_CFGR_MCO1PRE;
					break;
			}
		}
		else{
			/* Se imprime un mensaje para avisar que el valor no esta definido */
			writeMsg(&usart6Comm, " Value outside the range 1 to 5, please try again \n");

		}
	}

	else if(strcmp(cmd, "setTime") == 0){
		writeMsg(&usart6Comm, " Command: setTime \n");
		// Validacion de la hora
		if(firstParameter < 0 || firstParameter > 23){
			writeMsg(&usart6Comm, " Value outside the range of hours 0 to 23, please try again \n");
		}
		// Validacion de los minutos
		if(secondParameter < 0 || secondParameter > 59){
			writeMsg(&usart6Comm, " Value outside the range of minutes 0 to 59, please try again \n");
		}
		// Validacion de los segundos
		if(thirdParameter < 0 || thirdParameter > 59){
			writeMsg(&usart6Comm, " Value outside the range of seconds 0 to 59, please try again \n");
		}
		else{
			handlerRTC.hours = firstParameter;
			handlerRTC.minutes = secondParameter;
			handlerRTC.seconds = thirdParameter;
			rtc_config(&handlerRTC);

			sprintf(bufferData, " Selected time: %02u:%02u:%02u \n", firstParameter,secondParameter,thirdParameter);
			writeMsg(&usart6Comm, bufferData);
		}
	}

	else if(strcmp(cmd, "setDate") == 0){
		writeMsg(&usart6Comm, " Command: setDate \n");
		// Validacion de los meses
		if (secondParameter < 1 || secondParameter > 12){
			/* Se imprime un mensaje para avisar que el valor no esta definido */
			writeMsg(&usart6Comm, " Value outside the range of months 1 to 12, please try again \n");
		}
		// Validacion de los años 2000 y 2099
		if (thirdParameter < 00 || thirdParameter > 99) {
			/* Se imprime un mensaje para avisar que el valor no esta definido */
			writeMsg(&usart6Comm, " Value outside the range of years 2000 to 2099, please try again \n");
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
				writeMsg(&usart6Comm, " Value outside the range of days \n");
				sprintf(bufferData, " The maximum day for month %02u in year %02u, is %02u \n", secondParameter,thirdParameter,maxDay);
				writeMsg(&usart6Comm, bufferData);
			}
			/* De los contrario, si son validos se configuran y se reporta en la terminal*/
			else{
				handlerRTC.day = firstParameter;
				handlerRTC.month = secondParameter;
				handlerRTC.year = thirdParameter;
				rtc_config(&handlerRTC);

				sprintf(bufferData, " Selected date: %02u-%02u-%02u \n", firstParameter,secondParameter,thirdParameter);
				writeMsg(&usart6Comm, bufferData);
			}
		}
	}

	else if(strcmp(cmd, "getDate") == 0){
		day = getDays(handlerRTC);
		month = getMonths(handlerRTC);
		year = getYears(handlerRTC);

		writeMsg(&usart6Comm, " Command: getDate \n");
		sprintf(bufferData, " Current date = %02u-%02u-%02u \n", day,month,year);
		writeMsg(&usart6Comm, bufferData);
	}

	else if(strcmp(cmd, "getTime") == 0){
		hour = getHours(handlerRTC);
		minutes = getMinutes(handlerRTC);
		seconds = getSeconds(handlerRTC);

		writeMsg(&usart6Comm, " Command: getTime \n");
		sprintf(bufferData, " Current time = %02u:%02u:%02u \n", hour,minutes,seconds);
		writeMsg(&usart6Comm, bufferData);
	}

	else if(strcmp(cmd, "setSampFreq") == 0){
		writeMsg(&usart6Comm, " Command: setSampFreq \n");
		/* Si los valores estan por fuera del maximo permitido se envia un mensaje */
		if (firstParameter < 8 || firstParameter > 40) {
			writeMsg(&usart6Comm, " Value outside the range of sampling 8 to 40 kHz \n");
		}
		else{
			uint16_t freq = firstParameter * 1000;
			uint16_t sampl = freq / 10;
			uint16_t period = (1000 / firstParameter);
			sprintf(bufferData, " Selected sampling frequency: %u kHz \n", firstParameter);
			writeMsg(&usart6Comm, bufferData);
			sprintf(bufferData, " That frequency is capable of sampling a signal of %u Hz \n", sampl);
			writeMsg(&usart6Comm, bufferData);
			updateFrequency(&handlerEventTimer, period);
			flagADC = 1;
		}
	}

	else if(strcmp(cmd, "displayData") == 0){
		writeMsg(&usart6Comm, " Command: displayData \n");
		if(flagADC){
			writeMsg(&usart6Comm, "Analog-to-digital conversion initialized successfully!\n");
			writeMsg(&usart6Comm, "\n");

			delay_ms(500);
			startPwmSignal(&handlerEventTimer);
			// Se baja la bandera para obligar a cargar la frecuencia
			flagADC = 0;
		}
		else{
			writeMsg(&usart6Comm, "Command locked. Use setSampFreq first to set the frequency of your signal.\n");
			writeMsg(&usart6Comm, "\n");
		}
	}

	else if(strcmp(cmd, "getAccelData") == 0){
		writeMsg(&usart6Comm, "Command: getAccelData \n");
		sprintf(bufferData, "	... Initializing MPU-6050 ... \n");
		writeMsg(&usart6Comm, bufferData);
		/* Se solicita la identidad del Acelerometro*/
		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
		/* Se resetea el Aceletrometro para poder hacer mediciones*/
		i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
		/* Si se recibe una identidad del Accelerometro se continua la funcion*/
		if(i2cBuffer != 0 ){
			sprintf(bufferData, " ... Accelerometer is working OK \n");
			writeMsg(&usart6Comm, bufferData);
			delay_ms(500);
			sprintf(bufferData, " Starting Accelerometer sampling at 200 Hz ... \n");
			writeMsg(&usart6Comm, bufferData);
			flagSampling = 1;
		}
		else{
			sprintf(bufferData, " Accelerometer is not working \n");
			writeMsg(&usart6Comm, bufferData);
			sprintf(bufferData, " We kindly request you to re-enter the command 'getAccelData' \n");
			writeMsg(&usart6Comm, bufferData);
		}

	}

	else if(strcmp(cmd, "doFFT") == 0){
		writeMsg(&usart6Comm, "Command: doFFT \n");

		statusInitFFT = arm_rfft_fast_init_f32(&fft_instance, SIZE_FFT);

		if(statusInitFFT == ARM_MATH_SUCCESS){
			writeMsg(&usart6Comm, "The FFT has been successfully initialized. \n");
			delay_ms(500);
			/* Se realiza la transformada de fourier*/
			arm_rfft_fast_f32(&fft_instance, zValueSample, fft_output, 0);
			/* Se obtiene la amplitud de los pares de vectores re[n], im[n]*/
			arm_cmplx_mag_f32(fft_output, fft_output, SIZE_FFT/2);
			//fft_output[0] = 0;
			/* Para limpiar la señal se obtiene la media de todo el array*/
			arm_mean_f32(fft_output, SIZE_FFT, &mean);
			/* Se iguala la primera posicion del DC Offset al promedio de los datos, para
			 * evitar una señal de 0 Hz debido a las fluctuaciones o variaciones irregulares*/
			fft_output[0] = mean;
			/* Se resta de todo el array, la media de los datos para normalizar un poco la señal*/
			arm_offset_f32(fft_output, -mean, fft_output, SIZE_FFT/2);
			/* Se obtiene el valor maximo junto con su indice */
			arm_max_f32(fft_output, SIZE_FFT/2, &max_value, &max_index);
			/* Para obtener la frecuencia, utilizamos la forma en que se reportan los datos de la FFT.
			 * Cada posición del arreglo representa una frecuencia ascendente, que aumenta según
			 * la resolución de frecuencia de la FFT. La resolución de frecuencia está determinada
			 * por la relación: Fs/Tamaño_FFT, donde Fs es la frecuencia de muestreo. */
			freq = (max_index * 200.0) / SIZE_FFT;

			sprintf(bufferData, " Detected frequency: %.2f Hz \n",freq);
			writeMsg(&usart6Comm, bufferData);
			memset(fft_output, 0, sizeof(fft_output));
		}
	}

	else if(strcmp(cmd, "setPeriod") == 0){
		writeMsg(&usart6Comm, "Command: setPeriod \n");
		if(firstParameter < 100){
			firstParameter = 100;
		}
		if(firstParameter > 10000){
			firstParameter = 10000;
		}
		/* Actualizamos el nuevo periodo para el timer*/
		handlerBlinkyTimer.TIMx_Config.TIMx_period = firstParameter;
		BasicTimer_Config(&handlerBlinkyTimer);
	}

	else if(strcmp(cmd, "printF1") == 0){
		writeMsg(&usart6Comm, "Command: printF1 \n");
		writeMsg(&usart6Comm,"\n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⡿⠿⠿⠿⣿⣿⣿⣿⡿⠿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⡇⠀⠀⢀⡿⠟⠋⠁⠀⠀⠀⠀⠸⢿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⡇⠀⠀⣠⡴⢦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠀⢀⣬⢯⣭⡉⠛⠻⢿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣇⣀⣸⡏⠀⠀⢹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠀⢈⣷⠀⠀⠶⣾⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣷⣶⣿⣷⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣿⣷⣶⣾⣿⣷⣶⣶⣿⣿\n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
		writeMsg(&usart6Comm,"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ \n");
	}

	else{
		/* Se imprime el mensaje "Caution!! Wrong Command, try again" para avisar que el comando
		 * no esta definido */
		writeMsg(&usart6Comm, "Caution!! Wrong Command, please try again. \n");
		writeMsg(&usart6Comm, "Type: help # for command menu \n");
	}

}

int16_t readXValue(I2C_Handler_t *ptrHandlerI2C){
	uint8_t AccelX_low = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_XOUT_L);
	uint8_t AccelX_high = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_high << 8 | AccelX_low;
	return AccelX;
}

int16_t readYValue(I2C_Handler_t *ptrHandlerI2C){
	uint8_t AccelY_low = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_YOUT_L);
	uint8_t AccelY_high = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_YOUT_H);
	int16_t AccelY = AccelY_high << 8 | AccelY_low;
	return AccelY;
}

int16_t readZValue(I2C_Handler_t *ptrHandlerI2C){
	uint8_t AccelZ_low = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(ptrHandlerI2C, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
	return AccelZ;
}

void saveData(void){
	int16_t AccelX = readXValue(&handlerAccelerometer);
	int16_t AccelY = readYValue(&handlerAccelerometer);
	int16_t AccelZ = readZValue(&handlerAccelerometer);

	if( flagSampling && counter >= 0){
		xValueSample[counter] = (AccelX * 9.78f)/16384.0f;;
		yValueSample[counter] = (AccelY * 9.78f)/16384.0f;;
		zValueSample[counter] = (AccelZ * 9.78f)/16384.0f;;
	}
}

/* Callback del TIM2 par hacer un blinky con el led de estado*/
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
}

/* Callback para hacer el muestreo de los datos cada 200 Hz*/
void BasicTimer4_Callback(void){
	flag200hz = 1;
	if(flagSampling){
		counter ++;
	}
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido por
 * el puerto del USART2*/
void usart6Rx_Callback (void){
	rxData = getRxData();
}

/* Callback del ADC */
void adcComplete_Callback(void){
	if(adcCounter == 0){
		adcData1[adcDataCounter] = getADC();
	}
	else{
		adcData2[adcDataCounter] = getADC();
		adcDataCounter++;
	}
	adcCounter++;
	if(adcDataCounter == 256){
		adcDataCounter = 0;
		stopPwmSignal(&handlerEventTimer);
		adcIsComplete = true;
	}
	if(adcCounter == 2){
		adcCounter = 0;
	}
}

