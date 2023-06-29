/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "xprintf.h"
//#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Third_Party/FatFs_R0_12b__elm_chan/ffsample_stm32/xprintf.h"
//#include "/media/odmin/MyBook/embed/stm32/libThird_Party/FatFs_Chan/ff15_ffsample_stm32/xprintf.h"

#include "sf120_if.h"
#include "/media/odmin/MyBook/embed/stm32/libArgonte/ad7705/ad7705.h"
#define FLOAT_MAXIMAL 1.23e+14


#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Argonte/lcd12864_ks0108/v2/lcd12864_ks0108.h"
extern uint8_t font_3x4[];
extern uint8_t font_5x7[];
extern uint8_t Terminal8x14[];
extern uint8_t Consolas18x24[];

#define SF120_LCD_Top(s) do{ LCD_setPrint(0,0,font_5x7,0); LCD_puts((uint8_t*)s);}while(0)
#define SF120_LCD_But(s) do{ LCD_setPrint(0,7,font_5x7,0); LCD_puts((uint8_t*)s);}while(0)
#define SF120_LCD_l2_END(str,back) do{ LCD_setPrint(111,2,font_5x7,back); LCD_puts((uint8_t*)str);}while(0) //Use to indicate AD7705 multiplay
#define SF120_LCD_l3_END(str,back) do{ LCD_setPrint(111,3,font_5x7,back); LCD_puts((uint8_t*)str);}while(0) //Use indicate Exponent of digit
#define SF120_LCD_l4_END(str,back) do{ LCD_setPrint(111,4,font_5x7,back); LCD_puts((uint8_t*)str);}while(0)


#define BOARD__BluePill
#if defined(BOARD__BluePill)
	#define LED_ON()  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)
	#define LED_OFF() HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)
	void LED_Blink_SOS(uint8_t count);
#endif


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
	#define SIZE_UNION_ARRAY_TOTALU16 (MOTOR_WL_FINISH_HALOGEN - MOTOR_WL_START_HALOGEN)   //5974
	union ramArray_u{
		uint16_t u16_adc[SIZE_UNION_ARRAY_TOTALU16+5];
		uint16_t u16_IoIt[2][SIZE_UNION_ARRAY_TOTALU16/2];
	};
	extern union ramArray_u ramArray;
	#define u16_adc__startWL  ramArray.u16_adc[SIZE_UNION_ARRAY_TOTALU16+0]
	#define u16_adc__finishWL ramArray.u16_adc[SIZE_UNION_ARRAY_TOTALU16+1]
	#define u16_adc__maxValWL ramArray.u16_adc[SIZE_UNION_ARRAY_TOTALU16+2]
	#define u16_adc__min    ramArray.u16_adc[SIZE_UNION_ARRAY_TOTALU16+3]
	#define u16_adc__max    ramArray.u16_adc[SIZE_UNION_ARRAY_TOTALU16+4]
	union ramLCD_u{
		uint8_t u08_lcdY[1][128+4];
	};
	extern union ramLCD_u ramLCD;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void debug_str(char* str);
//#define debug_Error_Handler() do{ debug_str("\r\nError\r\n"); debug_str(__FILE__); /*debug_str(__LINE__);*/ }while(0)
extern void xsprintf (char* buff, const char*	fmt, ...);
#define debug_Error_Handler() do{char str[100]; xsprintf(str, "\r\nError\r\n%s (%d)\r\n", __FILE__, __LINE__); debug_str(str); }while(0)

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define NotUsed_Pin GPIO_PIN_2
#define NotUsed_GPIO_Port GPIOA
#define HC595_OE_MOTOR_Pin GPIO_PIN_0
#define HC595_OE_MOTOR_GPIO_Port GPIOB
#define HC595_Latch_MOTOR_Pin GPIO_PIN_1
#define HC595_Latch_MOTOR_GPIO_Port GPIOB
#define BOOT1_IN_Pin GPIO_PIN_2
#define BOOT1_IN_GPIO_Port GPIOB
#define SF120_KeyR3_Pin GPIO_PIN_7
#define SF120_KeyR3_GPIO_Port GPIOB
#define SF120_KeyR2_Pin GPIO_PIN_8
#define SF120_KeyR2_GPIO_Port GPIOB
#define SF120_KeyR1_Pin GPIO_PIN_9
#define SF120_KeyR1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
