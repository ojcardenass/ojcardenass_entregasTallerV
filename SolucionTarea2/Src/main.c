/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Oscar Julián Cárdenas Sosa
 * @brief          : Archivo principal (main)
 ******************************************************************************
 *
 *								SOLUCION TAREA 2
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

/**
 * |||||||||||||||||||||||||||||||||||||||Punto 1||||||||||||||||||||||||||||||||||||||||||||||||
*
 * El error en la guncion GPIO_ReadPin recae en la manera en que quedan los datos en el valor del registro IDR,
 * este se desplaza hacia la derecha a cantidad de veces que indica el numero del PIN especificado, pero no
 * garantiza que las demas posiciones sean cero (informacion a la izquierda del bit). Es por eso que, al realizar
 * la operacion AND con la mascara 0b1, se garantiza que los demas bits seran cero.
 * Obteniendo asi el valor real de dicho PIN
 *
 * */
//La solucion del error esta implementada en el codigo usado para el punto 3

/**
*  |||||||||||||||||||||||||||||||||||||||Punto 2||||||||||||||||||||||||||||||||||||||||||||||||
*
* La funcion GPIOxTooglePin(GPIO_Handler_t *pPinHandler), recibe un solo parametro el elemento
* GPIO_Handler_t. Para cambiar el estado de un PIN se hace uso del ODR (Output Data Register), ya que
* este es el unico registro en el que se puede leer o escribir datos, como se ve en la Fig. 19 del
* manual de referencia.
* Basandome en la estructura de la funcion GPIO_ReadPin del punto anterior, es posible hacer una
* estructura similar para nuestra funcion.
* pPinHandler->pGPIOx->ODR ^= (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber)
* En la cual mediante una operacion XOR con una mascara que contiene un 1, (en este
* caso la variable SET). Nos asegura que sin importar el valor que tenga el bit, este cambiara de
* estado.
*
* Esta funcion debe ir en el GPIOxDriver.c para que sea una funcion global, ademas se debe incluir
* su nombre y argumentos en el archivo de cabeceras GPIOxDriver.h.
*
**/

/**
*  |||||||||||||||||||||||||||||||||||||||Punto 3||||||||||||||||||||||||||||||||||||||||||||||||
*
*  						Contador de segundos binario up-down
*
**/
	/*Definimos los handlers para los PINs que deseamos configurar. En este caso los puertos son:
	 *
	 * 	bit-6    PC9
	 * 	bit-5    PC6
	 * 	bit-4    PB8
	 * 	bit-3    PA6
	 * 	bit-2    PC7
	 * 	bit-1    PC8
	 * 	bit-0    PA7
	 *	Boton	 PC13
	 *
	 * 	Que a su vez corresponden a los bit de un numero binario de 7 bits que se representa en 7 LEDs y al USER Button B1
	 */
		GPIO_Handler_t handlerPinLed0 = {0};		//0-PA7
		GPIO_Handler_t handlerPinLed1 = {0};		//1-PC8
		GPIO_Handler_t handlerPinLed2 = {0};		//2-PC7
		GPIO_Handler_t handlerPinLed3 = {0};		//3-PA6
		GPIO_Handler_t handlerPinLed4 = {0};		//4-PB8
		GPIO_Handler_t handlerPinLed5 = {0};		//5-PC6
		GPIO_Handler_t handlerPinLed6 = {0};		//6-PC9
		GPIO_Handler_t handlerUserButton = {0};		//USER_B1

/* Funcion principal del programa. Es aca donde se ejecuta todo */
int main(void)
{
		//Deseamos trabajar con el puerto GPIOA que es el puerto al que estan conectados los PINs PA6 y PA7 con
		//sus respectivos LED
		//Se configura como salida ya que los led se van a usar como indicadores
		//3-PA6
		handlerPinLed3.pGPIOx = GPIOA;
		handlerPinLed3.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
		handlerPinLed3.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
		handlerPinLed3.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinLed3.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinLed3.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
		handlerPinLed3.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna funcion
		//0-PA7
		handlerPinLed0.pGPIOx = GPIOA;
		handlerPinLed0.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
		handlerPinLed0.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
		handlerPinLed0.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinLed0.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinLed0.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
		handlerPinLed0.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna funcion

		//Se configuran ahora el PIN relacionado al puerto GPIOB, al que esta PB8 con su respectivo LED
		//4-PB8
		handlerPinLed4.pGPIOx = GPIOB;
		handlerPinLed4.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
		handlerPinLed4.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
		handlerPinLed4.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinLed4.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinLed4.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
		handlerPinLed4.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna funcion

		//Los PINs relacionados con puerto GPIOB, al que se conectan PC6,PC7,PC8 Y PC9 con sus respectivos LEDs
		//5-PC6
		handlerPinLed5.pGPIOx = GPIOC;
		handlerPinLed5.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
		handlerPinLed5.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
		handlerPinLed5.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinLed5.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinLed5.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
		handlerPinLed5.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna funcion
		//2-PC7
		handlerPinLed2.pGPIOx = GPIOC;
		handlerPinLed2.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
		handlerPinLed2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
		handlerPinLed2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinLed2.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinLed2.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
		handlerPinLed2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna funcion
		//1-PC8
		handlerPinLed1.pGPIOx = GPIOC;
		handlerPinLed1.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
		handlerPinLed1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
		handlerPinLed1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinLed1.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinLed1.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
		handlerPinLed1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna funcion
		//6-PC9
		handlerPinLed6.pGPIOx = GPIOC;
		handlerPinLed6.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
		handlerPinLed6.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
		handlerPinLed6.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinLed6.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinLed6.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
		handlerPinLed6.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna funcion

		//Deseamos trabajar con el puerto GPIOC, Pin del USER BOTTON (azul).
		handlerUserButton.pGPIOx = GPIOC;
		handlerUserButton.GPIO_PinConfig.GPIO_PinNumber              = PIN_13;

		//Es importante recordar que el botón es una entrada, por ende en "PinMode" se modifica
		// y se pone "IN" en lugar de "OUT".
	    handlerUserButton.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN;
	    handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLUP;
	    handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_FAST;
	    handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;				//Ninguna función

		//Cargamos las configuraciones del PINs
		GPIO_Config(&handlerPinLed0);
		GPIO_Config(&handlerPinLed1);
		GPIO_Config(&handlerPinLed2);
		GPIO_Config(&handlerPinLed3);
		GPIO_Config(&handlerPinLed4);
		GPIO_Config(&handlerPinLed5);
		GPIO_Config(&handlerPinLed6);
		GPIO_Config(&handlerUserButton);

	// Se inicializa el contador en su valor minimo = 0, ademas es una variable con signo
	// ya que puede tomar valores negativos
	int8_t counter = 0;

	// Se crea una variable para la direccion siendo,1 ascendente y -1 descendente
	int8_t direction = 1;

	while (1)
	{
		// Se verifica si el boton USER BUTTON B1 esta presionado
		if((GPIO_ReadPin(&handlerUserButton)) == 0){
			while ((GPIO_ReadPin(&handlerUserButton)) == 0)
			{
				// Se cambia la direccion mientras el boton esta presionado
				direction = -1;
				counter += direction;
				// Las siguientes lineas de codigo, son explicadas a detalle en la variante Else
				if (counter < 1)
				{
					counter = 60;
				}
				else if (counter > 60)
				{
					counter = 1;
				}

				GPIO_WritePin(&handlerPinLed0, (counter & 0x01));		//0-PA7
				GPIO_WritePin(&handlerPinLed1, (counter & 0x02)>>1);	//1-PC8
				GPIO_WritePin(&handlerPinLed2, (counter & 0x04)>>2);	//2-PC7
				GPIO_WritePin(&handlerPinLed3, (counter & 0x08)>>3);	//3-PA6
				GPIO_WritePin(&handlerPinLed4, (counter & 0x10)>>4);	//4-PB8
				GPIO_WritePin(&handlerPinLed5, (counter & 0x20)>>5);	//5-PC6
				GPIO_WritePin(&handlerPinLed6, (counter & 0x40)>>6);	//6-PC9

				for (uint32_t i = 0; i < 1240000; i++){
					NOP();
				}// La accion se mantiene mientras el boton continue presionado
			}
		}

		else{
			//Si el boton no esta presionado, la direccion es ascendente
			direction = 1;
		}
		// Segun el valor de la direccion, se realiza una operacion de resta o de suma al contador
		counter += direction;

		// Se restringen los valores del contador, si es menor que 1, se regresa a 60
		if (counter < 1)
		{
			counter = 60;
		}
		// Si es mayor que 60, se reinicia a 1
		else if (counter > 60)
		{
			counter = 1;
		}

		/**
		 *  Con operaciones bitwise y el uso de numeros Hexadecimales (para simplificar la visualizacion)
		 * se hallan los valores de cada posicion, ejm: counter = 36, la representacion de este numero en
		 * binarios es 0100100, por lo tanto para obtener los valores correspondientes para cada registro, se
		 * realizan las siguientes operaciones:
		 * ~ Para los pines PA7 y PA6:
		 * (counter & 0x01)			[Extrae el bit-0 (contador & 0000001), obteniendo un 0 en este PIN
		 * (counter & 0x08) >> 3	[Extrae bit-3 (contador & 0001000), obteniendo un 0 en este PIN
		 *
		 * ~ Para el pin PB8:
		 * (counter & 0x10) >> 4 	[Extrae el bit-4 (contador & 0010000) , obteniendo un 0 en este PIN
		 *
		 * ~ Para los pines PC6, PC7, PC8 y PC9:
		 * (counter & 0x20) >> 5	[Extrae el bit-5 (contador & 0100000), obteniendo un 1 en este PIN
		 * (counter & 0x04) >> 2 	[Extrae el bit-2 (contador & 0000100), obteniendo un 1 en este PIN
		 * (counter & 0x02) >> 1 	[Extrae el bit-1 (contador & 0000010), obteniendo un 0 en este PIN
		 * (counter & 0x40) >> 6 	[Extrae el bit-6 (contador & 1000000), obteniendo un 0 en este PIN
		 *
		 * En todas las operaciones se realiza un shift a la derecha cuya valor depende del numero del bit, y asi
		 * es posible escribir este valor en el PIN especifico
		 */

		//Hacemos que los pin queden encendidos segun el valor resultante del contador y la mascara especifica
		GPIO_WritePin(&handlerPinLed0, (counter & 0x01));		//0-PA7
		GPIO_WritePin(&handlerPinLed1, (counter & 0x02)>>1);	//1-PC8
		GPIO_WritePin(&handlerPinLed2, (counter & 0x04)>>2);	//2-PC7
		GPIO_WritePin(&handlerPinLed3, (counter & 0x08)>>3);	//3-PA6
		GPIO_WritePin(&handlerPinLed4, (counter & 0x10)>>4);	//4-PB8
		GPIO_WritePin(&handlerPinLed5, (counter & 0x20)>>5);	//5-PC6
		GPIO_WritePin(&handlerPinLed6, (counter & 0x40)>>6);	//6-PC9

		// 1 segundo son aproximadamente 1240000 iteraciones (Comprobado con el software Pulseview)
		for (uint32_t i = 0; i < 1240000; i++){
			NOP();
		}

	}

}
