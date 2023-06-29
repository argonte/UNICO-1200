# UNICO-1200
New firmware of spectrophotometer UNICO-1200

This project is aimed at developing the program code of the tsm microcontroller to replace the standard one installed in spectrophotometers. This will allow to expand their functionality and increase the accuracy of measurements.
I will be glad if it is useful to someone. I created this project for myself because the firmware of the standard microcontroller was corrupted. I was forced to replace the regular MC68HC705P9 microcontroller with a more powerful STM32F103CB and write a new firmware for it. This modernization is relevant for UNICO-1200, ULAB-102, КФК-3-01, ПЕ-5400, V-2100, UNICO-2150, SF-120 and many other single-beam devices, where the selection of the wavelength is implemented by applying control pulses to the stepper motor that moves the diffraction mirror. And the signal is removed from the photodiode through an operational amplifier that converts current into voltage.
