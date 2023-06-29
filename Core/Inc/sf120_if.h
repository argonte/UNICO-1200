/*
 * sf120_if.h
 *
 *  Created on: 29 бер. 2021 р.
 *      Author: admin
 */

#ifndef INC_SF120_IF_H_
#define INC_SF120_IF_H_

#include <stdint.h>

#include "ad7705.h"

/*Темновий струм при заводському виконанні менше 1 мВ*/

#define A_StepToWl (float)0.133757962
#define B_StepToWl (float)(-25.7961796)

/*3200 - ледь помітний для мене синій АЦП==694*/
/*3500 - синій АЦП==860*/
#define MOTOR_WL_FINISH_HALOGEN 8774  //це точно провірене максимальне значення повороту дзеркала кафедрального SF120
#define MOTOR_WL_DARCK  50
#define MOTOR_WL_WHITE  480
#define MOTOR_WL_START_HALOGEN  2800
#define MOTOR_WL_UPHIOLET  MOTOR_WL_START_HALOGEN
#define MOTOR_WL_PHIOLET  2900
#define MOTOR_WL_PHIOLET_TO_BLUE  3030
#define MOTOR_WL_BLUE  3400 //Max on MOTOR_FIL_BLUE ////if(fil==MOTOR_FIL_BLUE){adc = 00970} //one peak, adcDark=15..20
#define MOTOR_WL_BLUE_TO_GREEN    3600
#define MOTOR_WL_GREEN  4000
#define MOTOR_WL_GREEN_TO_YELLOW  4310
#define MOTOR_WL_YELLOW  4500
#define MOTOR_WL_ORANGE  5000
#define MOTOR_WL_YELLOW_TO_RED    5500
#define MOTOR_WL_RED  5562 //Max on MOTOR_FIL_RED ////if(fil==MOTOR_FIL_RED){adc = 03078} //from 5368, adcDark=0..15
#define MOTOR_WL_IRED  7000

#define ENUM_FIL_ENUMS   8
#define ENUM_FIL_NONE1   7
#define ENUM_FIL_NONE2   6
#define ENUM_FIL_RED     5
#define ENUM_FIL_YELLOW  4
#define ENUM_FIL_GREEN   3
#define ENUM_FIL_BLUE    2
#define ENUM_FIL_PHIOLET 1
#define ENUM_FIL_DARK    0

#define MOTOR_FIL_NEXT_N30     24391
#define MOTOR_FIL_FULL_PERIOD  813 //(MOTOR_FIL_NEXT_N30/30)       //813,0333
#define MOTOR_FIL_NEXT_N(n)    102 //((MOTOR_FIL_NEXT_N30*n)/30)   //101.629

#define MOTOR_FIL_NONE1   0  //MOTOR_FIL_NEXT_N(ENUM_FIL_NONE1)
#define MOTOR_FIL_NONE2   102//MOTOR_FIL_NEXT_N(ENUM_FIL_NONE2)
#define MOTOR_FIL_RED     204//MOTOR_FIL_NEXT_N(ENUM_FIL_RED)
#define MOTOR_FIL_YELLOW  306//MOTOR_FIL_NEXT_N(ENUM_FIL_YELLOW)
#define MOTOR_FIL_GREEN   408//MOTOR_FIL_NEXT_N(ENUM_FIL_GREEN)
#define MOTOR_FIL_BLUE    509//MOTOR_FIL_NEXT_N(ENUM_FIL_BLUE) //if(wl==MOTOR_WL_WHITE){adc = 02393}
#define MOTOR_FIL_PHIOLET 611//MOTOR_FIL_NEXT_N(ENUM_FIL_PHIOLET)
#define MOTOR_FIL_DARK    712//MOTOR_FIL_NEXT_N(ENUM_FIL_DARK)

//
	#define MOTOR_ROTATE_RIGHT 1
	#define MOTOR_ROTATE_LEFT  2
    #define MOTOR_MODE_WL   1
    #define MOTOR_MODE_FIL  2
extern int16_t sf120_WL;
extern int16_t sf120_WL_keyUpOrDown;
extern int16_t sf120_FIL;

#ifdef AD7705_H
	#define AD7705_CH_SF120    AD7705_CH1
	#if defined( ADC7705_DRDY )
	#else
		extern uint8_t AD7705_DelayGetADC_ms;
	#endif
#endif

void sf120_process_scanKey(void);
//
void SF120_LCD_print_DigFloat(float val, uint8_t backColor);
void SF120_LCD_print_Dig(uint16_t val, uint8_t backColor);
void SF120_LCD_print_sDig(int16_t val, uint8_t backColor);
void SF120_LCD_prepareSpec(uint8_t count);
void SF120_LCD_wiewSpec(uint8_t count, uint8_t wiewAxisStr);
void SF120_LCD_WL(uint8_t backColor);
void SF120_LCD_SwitchPrintVal(uint8_t print);
	#define SWITCHPRINTVAL_WL_start  1
	#define SWITCHPRINTVAL_WL_finish 2
void sf120_LCD_print_mode(uint8_t mode);

//==========================================
void SF120_motorTx(uint8_t u08tx);
void SF120_motor_StepN(int32_t count, uint8_t motor);

void sf120_setFilter(uint8_t filterEnum);

#if 1 // defined(u16_adc__startWL) && defined(u16_adc__finishWL) && defined(u16_adc__min) && defined(u16_adc__max)
void sf120_printSpectr( uint8_t count );
#else
void sf120_printSpectr(uint16_t start, uint16_t finish, uint8_t count);
#endif

//=====================================================================
#define HC595_STEPMOTOR_NSS_Pin GPIO_PIN_1
#define HC595_STEPMOTOR_NSS_GPIO_Port GPIOB
#define HC595_STEPMOTOR_OE_Pin GPIO_PIN_0
#define HC595_STEPMOTOR_OE_GPIO_Port GPIOB

#define HC595_OE_MOTOR_OFF()  HAL_GPIO_WritePin(HC595_OE_MOTOR_GPIO_Port, HC595_OE_MOTOR_Pin, GPIO_PIN_SET);//HC595 OutputDisable
#define HC595_OE_MOTOR_ON()   HAL_GPIO_WritePin(HC595_OE_MOTOR_GPIO_Port, HC595_OE_MOTOR_Pin, GPIO_PIN_RESET);//HC595 OutputEnable


//#define SF120_Prepare_ReadKey_STOPSTART() do{HC595_OE_MOTOR_OFF(); HAL_GPIO_WritePin(HC595_Latch_MOTOR_GPIO_Port, HC595_Latch_MOTOR_Pin, GPIO_PIN_SET); } while(0) //Need to SF120_ReadKey_START()
#define SF120_Prepare_ReadKey_STOPSTART() do{SF120_motor_StepN(-1, MOTOR_MODE_WL); SF120_motor_StepN(1, MOTOR_MODE_WL);} while(0) //Need to SF120_ReadKey_START()
//SF120_ReadKey dont work after LCD_puts !!!
#define SF120_ReadKey_STOP()    (HAL_GPIO_ReadPin(SF120_KeyR2_GPIO_Port, SF120_KeyR2_Pin) == GPIO_PIN_SET)
#define SF120_ReadKey_START()   (HAL_GPIO_ReadPin(SF120_KeyR1_GPIO_Port, SF120_KeyR1_Pin) == GPIO_PIN_SET)

void SF120_keyTx(uint8_t u08tx);
uint8_t SF120_keyRx(void);


void getOpticalAdsorbance(uint8_t countIndex);

#endif /* INC_SF120_IF_H_ */
