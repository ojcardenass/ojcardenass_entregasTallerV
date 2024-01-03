## Repositorio de la Materia Taller V: Electrónica Digital 2023-1

Este repositorio contiene una colección de proyectos y tareas desarrolladas durante la materia Taller V: Electrónica Digital de la Universidad Nacional de Colombia Sede Medellín. A continuación, se presenta una breve descripción de la carpeta principal y se destaca el proyecto "LED Audio Analyzer".

## Contenido del Repositorio

```bash
├── Librerias
│   ├── test
│   │   ├── TestADC
│   │   ├── TestCMSIS
│   │   ├── TestI2C
│   │   ├── TestPWM
│   │   ├── TestSPI
│   │   ├── TestUSART
│   │   └── TestUSART2
│   ├── 03-IntroHAL
│   ├── 05-DemoGPIOx
│   ├── 05a-IntroGPIOx
│   ├── 06-LEDBlinker
│   ├── 07-EXTI
│   ├── BasicConfig
│   ├── BasicTimer
│   ├── CMSIS-BasicConfig-DSP
│   ├── CMSIS-Full
│   ├── UpdateGPIO
│   ├── mcu_headers/CMSIS
│   ├── DSP_DRIVER/Include
│   └── PeripheralDrivers
├── Entregables
│   ├── Digital
│   ├── Examen
│   ├── SolucionTarea2
│   ├── SolucionTarea3
│   ├── SolucionTarea4Especial
│   ├── Taller_8
│   └── Talleres
├── Proyecto
│   ├── LED_AudioAnalizer_2.0
│   └── LED_AudioAnalizer_3
├── README.md
└── .gitignore
```

# LED Audio Analyzer

El proyecto LED Audio Analyzer es un Analizador de Espectro de Audio que utiliza matrices LED para visualizar de manera gráfica las frecuencias presentes en una señal de audio. Este proyecto combina conceptos de electrónica digital, programación de microcontroladores y manipulación de señales de audio.

## Características Principales:
### Visualización Gráfica: 
Utiliza matrices LED para representar de forma visual las frecuencias de audio detectadas.
### Análisis en Tiempo Real: 
Proporciona un análisis en tiempo real de la señal de audio entrante.
### Configuración Personalizada: 
Permite ajustar parámetros como la sensibilidad, tasa de refresco y la resolución del espectro.

## Funcionamiento:
El LED Audio Analyzer captura la señal de audio mediante un microfono o conexion directa a dispositivos de audio; la procesa mediante una Fast Fourier Transform (FFT) para identificar las frecuencias presentes. Luego, utiliza las matrices LED para mostrar barras de intensidad luminosa correspondientes a cada frecuencia detectada. Este enfoque ofrece una representación gráfica única de la composición espectral de la señal de audio.

## Uso y Aplicaciones:
### Proyectos de Iluminación Interactiva: 
Puede integrarse en proyectos de iluminación interactiva que responden a la música o sonidos ambientales.
### Educación en Electrónica Digital: 
Sirve como una herramienta educativa para entender la relación entre la señal de audio y su representación gráfica.

Este proyecto destaca la versatilidad y la aplicación práctica de los conceptos aprendidos en la materia, proporcionando una experiencia de aprendizaje enriquecedora en el campo de la Electrónica Digital.
