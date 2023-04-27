/****************************
 * Nombre		: Solucion de la tarea #3                                          *
 * author		: Luis Felipe Holguin Villada                                      *
 * Documento 	: 1039473604                                                       *
 * Fecha		: 29/09/2022                                                       *
 * Asignatura 	: Taller V                                                         *
 * Decripción	: Solucion de la tarea 3, USART SIMPLE                             *
 *                                                                                 *
 *                                                                                 *
 *                                                                                 *
 ***************************/
/**
 * Se incluyen los archivos y librerias pertintes para el desarrollo de la tarea.
 */
#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"

/**
 *Se crean los handlers necesarios para configurarlos posteriormente de la manera necesaria para la solucio de la tarea.
 */
USART_Handler_t           handlerUSART1      		     = {0} ;  //Handler para configuracion del USART1
BasicTimer_Handler_t      handlerTimer2                  = {0} ;  //Handler para el Timer
GPIO_Handler_t            handlerBlinkyPin               = {0} ;  //Handler para la configuracion del led de estado
GPIO_Handler_t            handlerButton       			 = {0} ;  //Handler para la configuracion del buton azul del micro.
GPIO_Handler_t            handlerTxPin                   = {0} ;  //handler para comunicacion de USART

//void TIM2_IRQHandler(void);

uint8_t data           = 0 ;   //Variable que omará diferentes valores en el while
uint8_t ButtonStatus   = 0 ;   //Variable que se usará para la lectura del estado del usarboton
uint8_t Caracter       = 0 ;   //Variable que se usará como una bandera


int main(void)
{
	//Se enciende el GPIOC para el PIN_5 que es el led 2 de la tarjeta del microcontrolador.
	handlerBlinkyPin.pGPIOx =GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_MEDIUM;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	//Se carga la configuracion del HandlerBlinkyPin, para el led de estado.
	GPIO_Config(&handlerBlinkyPin);


    // Se configura el boton azul del microcontrolador como una entrada y sin ninguna funcion auxiliar
	//el pin asociado a este boton es el PIN 13.
	handlerButton.pGPIOx = GPIOC;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber       = PIN_13;
	handlerButton.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_IN;
	handlerButton.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_PULLUP;
	handlerButton.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerButton.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;    //Ninguna funcion
	//Se carga la configuracion del HandlerButton.
    GPIO_Config(&handlerButton);


    //Configuramos el puerto GPIO para el módulo serial, necesario para la transmisión de informacion,
    //es decir el caracter.
	handlerTxPin.pGPIOx =GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber        = PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode    = AF7;
    //Se carga la configuracion del GPIO para el puerto serial.
	GPIO_Config(&handlerTxPin);


    //Se configura el Timer 2 con el periodo correspondiente al exigido en la tarea
	handlerTimer2.ptrTIMx = TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode               = BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed              = BTIMER_SPEED_1ms;     //Se elige valor para no hacer conversiones
	                                                                          //en el periodo a configurar
	handlerTimer2.TIMx_Config.TIMx_period             = 250;                  //Se configura el perido del Timer pedidio
	                                                                          //en la tarea
	handlerTimer2.TIMx_Config.TIMx_interruptEnable    = 1;
	BasicTimer_Config(&handlerTimer2);

	//Se configura el USART1
	handlerUSART1.ptrUSARTx = USART2;
	handlerUSART1.USART_Config.USART_mode             = USART_MODE_TX;
	handlerUSART1.USART_Config.USART_baudrate         = USART_BAUDRATE_115200;
	handlerUSART1.USART_Config.USART_datasize         = USART_DATASIZE_8BIT;   //Se configura el tamaño de bit a comunicar
	handlerUSART1.USART_Config.USART_parity           = USART_PARITY_NONE;     //Se configura sin paridad
	handlerUSART1.USART_Config.USART_stopbits         = USART_STOPBIT_1;
	//Se carga la configuracion del USART1
	USART_Config(&handlerUSART1);

	/**
	 * Programa que da solucion a la tarea # 3
	 */


    /* Loop forever */
	while(1){

		/*
		 * Se le asocia la funcion GPIO_ReadPin a la variable ButtonStatus para que el programa identifique si
		 * el botón el boton está o no presionado
		 */

		/*
		 * SOLUCION PUNTO 2
		 */
	    ButtonStatus = GPIO_ReadPin(&handlerButton);


	    if(Caracter){                              // Aqui se lee si la bandera está o no levantada y de ser así
	    	                                       // el programa se corre.
	    	if(ButtonStatus == SET){               // Si el boton no esta presionado entonces se enviara por medio del PIN9
	    		                                   // el caracter definido en el condicional, en este caso "f".
	    		writeChar(&handlerUSART1, 'f');
           /*
            * SOLUCION PUNTO 3
            */

	    	}
	    	else {                                //Si el boton está presionado, el programa enviara un valor denominado
	    		                                  // data, que está inicializado en 0 en el inicio del programa y si el
	    		                                  // boton se mantiene presionado se enviará el calor de dato + 1,
	    		                                  // enviando cada vez un valor diferente al anterior.

	    		writeChar(&handlerUSART1, data);
	    		data++;

	    	}
	    	Caracter=0;                           // Se baja la bandera.
	    }



	}


}
/**
 * ESTA ES LA SOLUCION PARA EL PUNTO 1, ES DECIR, LA CORRECTA CONFIRUCACION DEL LED DE ESTADO.
 *
 */
void BasicTimer2_Callback(void){
/*
 * Se crea una función auxiliar que dispara la interrupción del timer, para que
 * desarrolle cada 250ms el cambio del registro ODR, que se traduce a un blinky
 * en el Pin 5 asociado al User Led.
 */
	handlerBlinkyPin.pGPIOx -> ODR ^= GPIO_ODR_OD5;
/*
 * Se llama la bandera en el callback y de esta manera aseguramos que las interrupciones, es decir, el envio de
 * datos se dará con el mismo periodo que esta configurado el Timer2.
 */
	Caracter = 1;


}
