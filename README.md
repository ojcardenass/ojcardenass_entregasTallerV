## Repository of the course Workshop V: Digital Electronics 2023-1

This repository contains a collection of projects and assignments developed during the course Workshop V: Digital Electronics at the Universidad Nacional de Colombia Sede Medellín. The following is a brief description of the main folder and highlights the project "LED Audio Analyzer".

## Table of Contents

```bash
├── Libraries
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
├── Assignments
│   ├── Digital
│   ├── Examen
│   ├── SolucionTarea2
│   ├── SolucionTarea3
│   ├── SolucionTarea4Especial
│   ├── Taller_8
│   └── Talleres
├── Project
│   ├── LED_AudioAnalizer_2.0
│   └── LED_AudioAnalizer_3
├── README.md
└── .gitignore
```

# LED Audio Analyzer
![Screenshot_2024-01-03-08-39-07-068_com google android apps docs](https://github.com/ojcardenass/ojcardenass_entregasTallerV/assets/124924365/4c1d6415-dc73-42ef-b062-27e27fbe0012)
The LED Audio Analyzer project is an Audio Spectrum Analyzer that uses LED arrays to graphically display the frequencies present in an audio signal. This project combines concepts of digital electronics, microcontroller programming and audio signal manipulation.


## Main Features:
### Graphical Display: 
Uses LED arrays to visually represent the detected audio frequencies.
### Real-Time Analysis: 
Provides real-time analysis of the incoming audio signal.
### Custom Settings: 
Allows you to adjust parameters such as sensitivity, refresh rate and spectrum resolution.


## Operation:
The LED Audio Analyzer captures the audio signal through a microphone or direct connection to audio devices; processes it through a Fast Fourier Transform (FFT) to identify the frequencies present. It then uses LED arrays to display light intensity bars corresponding to each detected frequency. This approach provides a unique graphical representation of the spectral composition of the audio signal.

## Materials Used

### Development Board
- **STM32F411RE Nucleo**

### Displays
- **LCD Display 2004A with I2C Module**
- **32 x 8 Dot Matrix Display with MAX7219**

### Audio Components
- **MAX9814 Microphone Amplifier**
- **3-Pole 3.5mm Stereo Headphone Jack**

### Other Components
- **3.3V/5V Output Breadboard Power Supply**

## Usage and Applications:
### Interactive Lighting Projects: 
Can be integrated into interactive lighting projects that respond to music or ambient sounds.
### Digital Electronics Education: 
Serves as an educational tool to understand the relationship between the audio signal and its graphical representation.

This project highlights the versatility and practical application of the concepts learned in the subject, providing an enriching learning experience in the field of Digital Electronics.
