#ifndef STM32F1_HAL_ANALOG_H
#define STM32F1_HAL_ANALOG_H

#include "stm32f1xx_hal.h"

void ADC_ConfigChannel(ADC_HandleTypeDef* hadc, uint32_t Channel);
void ADC_Init(ADC_HandleTypeDef* hadc);
uint16_t sf120_getAdc(ADC_HandleTypeDef* hadc);


#endif // STM32F1_HAL_ANALOG_H
