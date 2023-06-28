/*
 * sf120_if.c
 *
 *  Created on: 29 бер. 2021 р.
 *      Author: admin
 */
#include "sf120_if.h"
#include <math.h>

#include "main.h"
//#include <stdio.h>

#include "ad7705.h"

extern uint8_t cmdline[];//use in cli_relase.c

#define STEPMOTORDRV_ULN2003_CONNECT_TO_HALSPI_USE_HC595
#define          KEY_ULN2003_CONNECT_TO_HALSPI_USE_HC595

#if defined(STEPMOTORDRV_ULN2003_CONNECT_TO_HALSPI_USE_HC595)
	#define STEPMOTORDRV_ULN2003_HC595_SPI_DATALEN 8 //8 or 16
	extern SPI_HandleTypeDef hspi1;
	//extern SPI_HandleTypeDef hspi2;
	#define STEPMOTORDRV_HC595_SPI hspi1
#else
	#error
#endif

#if defined(KEY_ULN2003_CONNECT_TO_HALSPI_USE_HC595)
	//Copy from LCD driver src
	#define HC595_LCD_E_GPIO_Port GPIOA
	#define HC595_LCD_E_Pin       GPIO_PIN_4
	//	#define LCD_HC595_WIRES_CS1 (1<<0)/*не використовується, замісь нього CS1=(CS2>>1)*/
	#define LCD_HC595_WIRES_CS2 (1<<1)/*CS2 має бути обовязково наступним бітом після CS1*/
	#define LCD_HC595_WIRES_DI  (1<<2)
	//
	#define KEY_ULN2003_HC595_SPI_DATALEN   STEPMOTORDRV_ULN2003_HC595_SPI_DATALEN // KEY_ULN2003_HC595_SPI_DATALEN == STEPMOTORDRV_ULN2003_HC595_SPI_DATALEN
	#define KEY_ULN2003_HC595_SPI  STEPMOTORDRV_HC595_SPI //KEY_ULN2003_HC595_SPI == STEPMOTORDRV_HC595_SPI == LCD
#else
	#error
#endif

extern void LCD_delay_NOP(uint32_t count);
#if 0
//This func present in LCD driver src
	{
		uint32_t temp = count;
		#define NOP_COUNT_DELAY_1ms 100
		while(temp--){ for(uint8_t i = 0; i<NOP_COUNT_DELAY_1ms; i++) { __NOP(); } }
		#undef NOP_COUNT_DELAY_1ms
	}
#endif
#define KEY_TX_delay_NOP(n)   LCD_delay_NOP(n)
#define MOTOR_TX_delay_NOP(n)   LCD_delay_NOP(n)


extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;
//
extern uint8_t sf120_mode;
extern uint16_t adcVal[3]; //Dark; I0; It
//
extern uint8_t pDataUART1rx[];

//#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Third_Party/FatFs_R0_12b__elm_chan/ffsample_stm32/xprintf.h"
//#include "/media/odmin/MyBook/embed/stm32/libThird_Party/FatFs_Chan/ff15_ffsample_stm32/xprintf.h"
//#include "xprintf.h"

#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Argonte/lcd12864_ks0108/v2/lcd12864_ks0108.h"

extern uint8_t font_5x7[];
extern uint8_t Terminal8x14[];
extern uint8_t Consolas18x24[];

void sf120_process_scanKey(void)
{
	#if 1 //SF120_getKey()
	  static uint8_t keyCount[10]={0,0,0,0,0,0,0,0,0};

	  if(SF120_keyRx()!=0)
	  {
		  HAL_Delay(10);
		  uint8_t keyPress = SF120_keyRx();
		  char strDebug[20];
		  xsprintf(strDebug, "SF120_getKey = %d", keyPress);
		  debug_str(strDebug);
		  switch(keyPress)
		  {
			  case 0:
				  debug_str("(NULL)\r\n");
				  break;
			  case 1:
				  //debug_str("(MODE)\r\n");
				  sf120_LCD_print_mode(++sf120_mode);
				  sf120_mode %=6;
				  switch(sf120_mode){
					  case 0: //setup mode
						  break;
					  case 1: //T
						  //break;
					  case 2: //A
						  SF120_LCD_WL(0);
						  break;
					case 3: //C
						break;
					case 4: //F
						break;
					case 5: //S
				  		LCD_setPrint(0,0,font_5x7,1); LCD_puts((uint8_t*)" Get spectr setup:  ");
				  		LCD_setPrint(0,2,font_5x7,0); LCD_puts((uint8_t*)"Start WL: ");
				  		SF120_LCD_SwitchPrintVal(SWITCHPRINTVAL_WL_start);
				  		LCD_setPrint(0,3,font_5x7,0); LCD_puts((uint8_t*)"Stop  WL: ");
				  		SF120_LCD_SwitchPrintVal(SWITCHPRINTVAL_WL_finish);
				  		//
				  		break;
					default: debug_str("(MODE ?)\r\n");break;
					}
				  break;
			  case 2://Press key GOTO_L
				  switch(sf120_mode){
				  	  case 0: debug_str("(GOTO_L 0)\r\n");
				  		  break;
				  	  case 1:
				  	  case 2: //T or A
						  debug_str("(GOTO_L T or A)\r\n");
						  if(sf120_WL_keyUpOrDown > MOTOR_WL_FINISH_HALOGEN)
						  {
							  sf120_WL_keyUpOrDown = MOTOR_WL_FINISH_HALOGEN;
						  }else if(sf120_WL_keyUpOrDown < MOTOR_WL_DARCK)
						  {
							  sf120_WL_keyUpOrDown = MOTOR_WL_DARCK;
						  }
						  SF120_motor_StepN(sf120_WL_keyUpOrDown-sf120_WL, MOTOR_MODE_WL);

							if( sf120_WL > MOTOR_WL_YELLOW_TO_RED ) { SF120_motor_StepN(MOTOR_FIL_RED    -sf120_FIL, MOTOR_MODE_FIL); } else
								if( sf120_WL > MOTOR_WL_GREEN_TO_YELLOW ) { SF120_motor_StepN(MOTOR_FIL_YELLOW -sf120_FIL, MOTOR_MODE_FIL); } else
									if( sf120_WL > MOTOR_WL_BLUE_TO_GREEN ) { SF120_motor_StepN(MOTOR_FIL_GREEN  -sf120_FIL, MOTOR_MODE_FIL); } else
										if( sf120_WL > MOTOR_WL_PHIOLET_TO_BLUE ) { SF120_motor_StepN(MOTOR_FIL_BLUE   -sf120_FIL, MOTOR_MODE_FIL); } else
											if( sf120_WL >= MOTOR_WL_START_HALOGEN ) { SF120_motor_StepN(MOTOR_FIL_PHIOLET-sf120_FIL, MOTOR_MODE_FIL); } else
												if( sf120_WL >= (MOTOR_WL_START_HALOGEN-10) ) { SF120_motor_StepN(MOTOR_FIL_NONE1-sf120_FIL, MOTOR_MODE_FIL); } else
													if( sf120_WL >= (MOTOR_WL_START_HALOGEN-20) ) { SF120_motor_StepN(MOTOR_FIL_NONE2-sf120_FIL, MOTOR_MODE_FIL); } else
														if( sf120_WL == MOTOR_WL_DARCK ) { SF120_motor_StepN(MOTOR_FIL_DARK-sf120_FIL, MOTOR_MODE_FIL); }
							//
						  SF120_LCD_WL(0);
						  if(sf120_WL_keyUpOrDown != sf120_WL){ debug_str("motorStepCounter != sf120_WL\r\n"); }
				  		  break;
				  	case 3: debug_str("(GOTO_L 3)\r\n"); break;
				  	case 4: debug_str("(GOTO_L 4)\r\n"); break;
				  	case 5: debug_str("(GOTO_L -> Spectr)\r\n");
				  		u16_adc__finishWL = u16_adc__startWL;
				  		u16_adc__startWL -=100;
				  		SF120_LCD_SwitchPrintVal(SWITCHPRINTVAL_WL_start);
				  		SF120_LCD_SwitchPrintVal(SWITCHPRINTVAL_WL_finish);
				  		break;
				  	default: debug_str("(GOTO_L ?)\r\n");break;
				  	}
				  	break;
			  case 3:
				  switch(sf120_mode){
				  	  case 0: //setup mode
						#if defined( ADC7705_DRDY )
						#else
				  		  LCD_setPrint(0,0,font_5x7,1); LCD_puts((uint8_t*)"AD7705_DelayGetADC_ms");
				  		  SF120_LCD_print_Dig(++AD7705_DelayGetADC_ms, 0);
						#endif

				  		  break;
				  	  case 1:
				  	  case 2: //T or A
				  		  debug_str("(UP WL)\r\n");
				  		  if(keyCount[3]++ <6){sf120_WL_keyUpOrDown++;}else if(keyCount[3] >20){sf120_WL_keyUpOrDown +=100;}else {sf120_WL_keyUpOrDown += 10;}
				  		  SF120_LCD_WL(1);
				  		  break;
				  	case 3: debug_str("(UP 3)\r\n"); break;
				  	case 4: debug_str("(UP 4)\r\n"); break;
				  	case 5: //S
						debug_str("(UP S)\r\n");
						if(keyCount[6]++ <6){u16_adc__startWL++;}else if(keyCount[6] >20){u16_adc__startWL +=100;}else {u16_adc__startWL += 10;}
						SF120_LCD_SwitchPrintVal(SWITCHPRINTVAL_WL_start);
						break;
				  	default: debug_str("(UP ?)\r\n");break;
				  	}
				  	break;
			  case 4:
				  debug_str("(SET)\r\n");
				  switch(sf120_mode){
					  case 0:
						  //if ( hUsbDeviceFS.dev_old_state == 1 ){
						  if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_SET ){
							  debug_str("USB worked\r\n");
						  }else{
							  MX_USB_DEVICE_Init();
						  }
						  break;
					  case 1: //debug_str("(SET T)\r\n");
					  case 2: //debug_str("(SET A)\r\n");
						  getOpticalAdsorbance(1);
						  break;
					  case 3: debug_str("(SET 3)\r\n"); break;
					  case 4: debug_str("(SET 4)\r\n"); break;
					  case 5: debug_str("(SET 5)\r\n"); break;
					  default: debug_str("(SET ?)\r\n");break;
				  }
				  break;
			  case 5:
				  debug_str("(ZERO)\r\n");
				  switch(sf120_mode){
					  case 0: //debug_str("(ZERO 0)\r\n");
					  case 1: //debug_str("(ZERO 1)\r\n");
					  case 2: //debug_str("(ZERO 2)\r\n");
						  xsprintf((char*)cmdline, "sf getAdc d"); break;
					  case 3: //debug_str("(ZERO 3)\r\n"); break;
					  case 4: //debug_str("(ZERO 4)\r\n"); break;
					  case 5: //debug_str("(ZERO 5)\r\n"); break;
					  default: debug_str("(ZERO ?)\r\n");break;
				  }
				  break;
			  case 6:
				  switch(sf120_mode){
				  	  case 0: //setup mode
						#if defined( ADC7705_DRDY )
						#else
				  		  LCD_setPrint(0,0,font_5x7,1); LCD_puts((uint8_t*)"AD7705_DelayGetADC_ms");
				  		  SF120_LCD_print_Dig(--AD7705_DelayGetADC_ms, 0);
						#endif

				  		  break;
				  	  case 1:
				  	  case 2: //T or A
				  		  debug_str("(DOWN WL)\r\n");
				  		  if(keyCount[6]++ <6){sf120_WL_keyUpOrDown--;}else if(keyCount[6] >20){sf120_WL_keyUpOrDown -=100;}else {sf120_WL_keyUpOrDown -= 10;}
				  		  SF120_LCD_WL(1);
				  		  break;
				  	case 3: debug_str("(DOWN 3)\r\n"); break;
				  	case 4: debug_str("(DOWN 4)\r\n"); break;
				  	case 5: //S
				  		debug_str("(DOWN S)\r\n");
				  		if(keyCount[6]++ <6){u16_adc__startWL--;}else if(keyCount[6] >20){u16_adc__startWL -=100;}else {u16_adc__startWL -= 10;}
				  		SF120_LCD_SwitchPrintVal(SWITCHPRINTVAL_WL_start);
				  		break;
				  	default: debug_str("(DOWN ?)\r\n");break;
				  	}
				  	break;
			  case 7:
				  switch(sf120_mode){
					  case 0: debug_str("(PRINT 0)\r\n");
					  	  SF120_LCD_wiewSpec(0, 1);
					  	  break;
					  case 1: debug_str("(PRINT 1)\r\n"); break;
					  case 2: debug_str("(PRINT 2)\r\n"); break;
					  case 3: debug_str("(PRINT 3)\r\n"); break;
					  case 4: debug_str("(PRINT 4)\r\n"); break;
					  case 5: //debug_str("(PRINT 5)\r\n"); break;
						  SF120_LCD_wiewSpec(0, 0);
						  break;
					  default: debug_str("(PRINT ?)\r\n");break;
				  }
				  break;
			  case 8:
				  debug_str("(ESC)\r\n");
				  break;
			  case 9:
				  debug_str("(ENTER)\r\n");
				  switch(sf120_mode){
					  case 0: //debug_str("(ENTER 0)\r\n");
						  xsprintf((char*)cmdline, "sf getSpec %d %d o", u16_adc__startWL, u16_adc__finishWL); cmd_process_loop(); break;
					  case 1: //debug_str("(ENTER 1)\r\n");
					  case 2: //debug_str("(ENTER 2)\r\n");
						  getOpticalAdsorbance(2);
						  break;
					  case 3: //debug_str("(ENTER 3)\r\n"); break;
					  case 4: //debug_str("(ENTER 4)\r\n"); break;
					  case 5: //debug_str("(ENTER 5)\r\n"); break;
						  //xsprintf((char*)cmdline, "sf getSpec %d %d", u16_adc__startWL, u16_adc__finishWL); cmd_process_loop(); break;
						  xsprintf((char*)cmdline, "sf getSpec %d %d t", u16_adc__startWL, u16_adc__finishWL); cmd_process_loop(); break;
					  default: debug_str("(ENTER ?)\r\n");break;
				  }
				  break;
			  default:
				  debug_str("(Default)\r\n");
				  break;
		  }//END switch

	  }else{
		  for(uint8_t i=0; i<10; i++){keyCount[i]=0;}
	  }
	#endif //SF120_getKey
}

//----------------------------------------
void SF120_LCD_print_DigFloat(float val, uint8_t backColor)
{
    if(backColor == 0)
    {
        LCD_Line_H(  0, 1, 127, 0x03);//0b00000011 is Top line
        LCD_Line_H(  0, 2, 110, 0x00);
    	LCD_Line_H(  0, 3, 110, 0x00);
    	LCD_Line_H(  0, 4, 110, 0x00);
    	LCD_Line_H(  0, 5, 127, 0x03<<4);//0b00110000 is Button line
    }else{
        LCD_Line_H(  0, 1, 110, 0x03 | (0x01<<7));//0b10000011 is Top line
        LCD_Line_H(109, 1, 127, 0x03);//0b00000011 is Top line
        LCD_Line_H(  0, 2, 110, 0xFF);
    	LCD_Line_H(  0, 3, 110, 0xFF);
    	LCD_Line_H(  0, 4, 110, 0xFF);
    	LCD_Line_H(  0, 5, 110, (0x03<<4) | 0x01);//0b00110001 is Button line
    	LCD_Line_H(109, 5, 127, (0x03<<4));//0b00110000 is Button line
    }
    SF120_LCD_l3_END("   ", backColor);
    SF120_LCD_l4_END("   ", backColor);
    if(val == FLOAT_MAXIMAL){ return; }

#define LCD_print_Dig_START 0 //19
	uint8_t str[12];



		#if ( defined(XPRINTF_DEF) && (XPRINTF_DEF == 2021) )
			xsprintf((char*)str, (const char*)"%.4E", val);
		#else
			#error
		#endif

		debug_str( "DigFloat: " );
		debug_str( (const char*)str );
		debug_str( "\r\n" );
		if(str[0]=='-')
		{
			SF120_LCD_l4_END(&str[8], backColor);
		}else{
			SF120_LCD_l4_END(&str[7], backColor);
		}
		str[6]='\0';
	for(uint8_t i=0; i<6; i++)
	{
		if(str[i]>='0' && str[i]<='9') { LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((str[i]-'0'+3)*(18*24)/8), (18*24)/8, backColor); }
		else if(str[i]=='.' || str[i]==','){LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((2)*(18*24)/8), (18*24)/8, backColor);}
		else if(str[i]=='-'){LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((1)*(18*24)/8), (18*24)/8, backColor);}
		else {LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((0)*(18*24)/8), (18*24)/8, backColor);}
	}
#undef LCD_print_Dig_START
}
void SF120_LCD_print_Dig(uint16_t val, uint8_t backColor)
{
    if(backColor == 0)
    {
        LCD_Line_H(  0, 1, 127, 0x03);//0b00000011 is Top line
        LCD_Line_H(  0, 2, 110, 0x00);
    	LCD_Line_H(  0, 3, 110, 0x00);
    	LCD_Line_H(  0, 4, 110, 0x00);
    	LCD_Line_H(  0, 5, 127, 0x03<<4);//0b00110000 is Button line
    }else{
        LCD_Line_H(  0, 1, 110, 0x03 | (0x01<<7));//0b10000011 is Top line
        LCD_Line_H(109, 1, 127, 0x03);//0b00000011 is Top line
        LCD_Line_H(  0, 2, 110, 0xFF);
    	LCD_Line_H(  0, 3, 110, 0xFF);
    	LCD_Line_H(  0, 4, 110, 0xFF);
    	LCD_Line_H(  0, 5, 110, (0x03<<4) | 0x01);//0b00110001 is Button line
    	LCD_Line_H(109, 5, 127, (0x03<<4));//0b00110000 is Button line
    }
    SF120_LCD_l3_END("   ", backColor);
    SF120_LCD_l4_END("   ", backColor);
    if(val == 0xFFFF){ return; }

#define LCD_print_Dig_START 0 //19
	uint8_t str[12];
	#if 0
		if(val<0){val = val*(-1); xsprintf((char*)str, (const char*)"-%03d.%03d", val/100, val%100);}
		else { xsprintf((char*)str, (const char*)"%03d.%02d", val/100, val%100); }
		SF120_LCD_l4_END("   ", 0);
	#elif 1 //if (val > 32767) pratsue ne pravilno!
		uint32_t val32 = 0;
		val32 |= val;
		xsprintf((char*)str, (const char*)"%03ld.%02ld", val32/100, val32%100);
		//SF120_LCD_l4_END("   ", 0);
	#elif 0 //(_STRFUNC_ChaN == 2011) && (defined(_STRFUNC_ChaN) )
		/*unsigned*/ //EXAMPLE xprintf("%f", 678.0123);
		float adc_read = val;
		adc_read /= 100;
		float tmpVal = adc_read;
		int32_t tmpInt1 = tmpVal;           // Get the integer (678).
		float tmpFrac = tmpVal - tmpInt1;   // Get fraction (0.0123).
		int32_t tmpInt2 = tmpFrac * 100;  // Turn into integer (123).
		xsprintf ((char*)str, (const char*)"%03d.%02d", tmpInt1, tmpInt2);
	#elif 0 //(_STRFUNC_ChaN == 2011) && (defined(_STRFUNC_ChaN) )
		/*signed*/ //EXAMPLE xprintf("%f", 678.0123);
		float adc_read = (float)val/100;
		uint8_t* tmpSign = (adc_read < 0) ? (uint8_t*)"-" : (uint8_t*)"";
		float tmpVal = (adc_read < 0) ? -adc_read : adc_read;
		int32_t tmpInt1 = tmpVal;           // Get the integer (678).
		float tmpFrac = tmpVal - tmpInt1;   // Get fraction (0.0123).
		int32_t tmpInt2 = tmpFrac * 100;  // Turn into integer (123).
		xsprintf ((char*)str, (const char*)"%s%03d.%02d", tmpSign, tmpInt1, tmpInt2);
	#elif 0 //( defined(XPRINTF_DEF) && (XPRINTF_DEF == 2021) )
		float valFloat = (uint32_t)val;
		valFloat /=100;
		xsprintf((char*)str, (const char*)"%f", valFloat);
	#elif ( defined(XPRINTF_DEF) && (XPRINTF_DEF == 2021) )
		float valFloat = (int32_t)val;
		xsprintf((char*)str, (const char*)"%.4E", valFloat);
		SF120_LCD_C2_END(&str[7], backColor);
	#else //if (val > 32767) pratsue ne pravilno!
		uint16_t ostacha = val%100;
		uint16_t chile = val/100;
		xsprintf((char*)str, (const char*)"%03d.%02d", chile, ostacha);
		//
	#endif
		debug_str( "Dig: " );
		debug_str( (char*)str );
		debug_str( "\r\n" );
		str[6]='\0';
	for(uint8_t i=0; i<6; i++)
	{
		if(str[i]>='0' && str[i]<='9') { LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((str[i]-'0'+3)*(18*24)/8), (18*24)/8, backColor); }
		else if(str[i]=='.' || str[i]==','){LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((2)*(18*24)/8), (18*24)/8, backColor);}
		else if(str[i]=='-'){LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((1)*(18*24)/8), (18*24)/8, backColor);}
		else {LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((0)*(18*24)/8), (18*24)/8, backColor);}
	}

	//LCD_putBitmap(19+18*0, 2, 19+18*1, Consolas18x24+(('0'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
	//LCD_putBitmap(20+18*1, 2, 20+18*2, Consolas18x24+(('9'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
	//LCD_putBitmap(21+18*2, 2, 21+18*3, Consolas18x24+(('8'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
#undef LCD_print_Dig_START
}
void SF120_LCD_print_sDig(int16_t val, uint8_t backColor)
{
#define LCD_print_Dig_START 0 //19
	uint8_t str[8];
	if(val<0){val = val*(-1); xsprintf((char*)str, (const char*)"-%03d.%02d", val/100, val%100);}
	else { xsprintf((char*)str, (const char*)"%03d.%02d", val/100, val%100); }
	for(uint8_t i=0; i<6; i++)
	{
		if(str[i]>='0' && str[i]<='9') { LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((str[i]-'0'+3)*(18*24)/8), (18*24)/8, backColor); }
		else if(str[i]=='.' || str[i]==','){LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((2)*(18*24)/8), (18*24)/8, backColor);}
		else if(str[i]=='-'){LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((1)*(18*24)/8), (18*24)/8, backColor);}
		else {LCD_putBitmap(LCD_print_Dig_START+18*i, 2, LCD_print_Dig_START+18*(1+i), Consolas18x24+((0)*(18*24)/8), (18*24)/8, backColor);}
	}

	//LCD_putBitmap(19+18*0, 2, 19+18*1, Consolas18x24+(('0'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
	//LCD_putBitmap(20+18*1, 2, 20+18*2, Consolas18x24+(('9'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
	//LCD_putBitmap(21+18*2, 2, 21+18*3, Consolas18x24+(('8'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
#undef LCD_print_Dig_START
}

void SF120_LCD_prepareSpec(uint8_t count)
{
	//ця функція перезаписує дані, тому при першому випадку її не викликаємо
	#if 0 //Це вже зроблено в попередній функції: коли заповнювався масив то відразу визначалися мінімум і максимум
		u16_adc__min = 0xFFFF;
		u16_adc__max = 0x0000;
		for(uint16_t i=0; i<(u16_adc__finishWL-u16_adc__startWL); i++)
		{
			if( ramArray.u16_adc[i] > u16_adc__max ) { u16_adc__max = ramArray.u16_adc[i]; }
			if( ramArray.u16_adc[i] < u16_adc__min ) { u16_adc__min = ramArray.u16_adc[i]; }
		}

		for(uint16_t i=0; i<(u16_adc__finishWL-u16_adc__startWL); i++)
		{
			if( ramArray.u16_adc[i] ==  u16_adc__max ) {  u16_adc__maxValWL = u16_adc__startWL + i; break; }
		}
	#endif

	float koefDivY = ((float)(u16_adc__max - u16_adc__min))/(float)64;
	uint8_t koefDivX= round( (float)(u16_adc__finishWL - u16_adc__startWL)/128 );

	//Масштабувати масив даних по Y (Дані в масиві перезаписуються)
	//На майбутнє треба її переписати так щоб вона не переписувала вхідний масив, а відразу писала в масив дисплея
	for(uint16_t i=0; i<(const uint16_t)(u16_adc__finishWL - u16_adc__startWL); i++)
	{
		//ramArray.u16_adc[i] = (ramArray.u16_adc[i] - u16_adc__min)/koefDivY;//Якщо нічого не округлювати то все округлиться в сторону меньшого
		ramArray.u16_adc[i] = round((ramArray.u16_adc[i] - u16_adc__min)/koefDivY);
	}

	#if 0 //use normalisation
		#define NUM_BUFFF_TO_NORMAL 2
		for(uint16_t i=koefDivX*NUM_BUFFF_TO_NORMAL/2; i<(const uint16_t)((u16_adc__finish - u16_adc__start)-(koefDivX*NUM_BUFFF_TO_NORMAL/2)); i++)
		{
			for(int8_t j=(koefDivX*NUM_BUFFF_TO_NORMAL/2)*(-1); j<(const uint16_t)(koefDivX*NUM_BUFFF_TO_NORMAL/2); j++)
			{
				ramArray.u16_adc[i] += ramArray.u16_adc[i+j];
			}
			ramArray.u16_adc[i] /= koefDivX*NUM_BUFFF_TO_NORMAL;
		}
	#endif


	//Масштабувати масив даних по X (Дані записуються в новий масив)
	for(uint8_t i=0; i<128; i++)
	{
		for(uint8_t j=1; j<koefDivX; j++)
		{
			ramArray.u16_adc[(i*koefDivX)] += ramArray.u16_adc[ (i*koefDivX)+j ];
		}
		ramLCD.u08_lcdY[count][i] = round( (float)ramArray.u16_adc[(i*koefDivX)]/koefDivX );
	}

	SF120_LCD_wiewSpec(count, 1);

	#if 0 //debug
		xprintf("\n\Dump ramLCD.u08_lcdY[%d][..]\n\f", count);
		for(uint8_t i=0; i<13; i++)
		{
			xput_dump(&ramLCD.u08_lcdY[count][i*10], (i*10), 10, DW_DECIMAL8/*DW_CHAR*//*DW_DECIMAL8*/);
		}
		xputs("\n\---------------------------\n\f");
		for(uint8_t i=0; i<128; i++)
		{
			xprintf("%d %d\n\f", i, ramLCD.u08_lcdY[count][i]);
		}
	#endif
}
void SF120_LCD_wiewSpec(uint8_t count, uint8_t wiewAxisStr)
{
	/*if(count == 0)*/ { lcd_Clear(); }

	for(uint8_t i=0; i<128; i++)
	{
		uint8_t temp = 64 - ramLCD.u08_lcdY[count][i];
		if(ramLCD.u08_lcdY[count][i] >= 64) { temp=63; }
		LCD_putPix( i, temp );
	}

	if(wiewAxisStr == 1)
	{
		char str[9];
		xsprintf(str, "%d", u16_adc__max); LCD_setPrint(0,0,font_3x4,        0); LCD_puts((uint8_t*)str);
		xsprintf(str, "%d", u16_adc__min); LCD_setPrint(0,6,font_3x4, (4<<1)|0); LCD_puts((uint8_t*)str);

		float wl;
		/*start*/ wl = ((A_StepToWl*u16_adc__startWL )+(B_StepToWl))*100; xsprintf((char*)str, (const char*)"%6.2f", wl/100); LCD_setPrint(  0,7,font_3x4, (4<<1)|0); LCD_puts((uint8_t*)str);
		/*finish*/wl = ((A_StepToWl*u16_adc__finishWL)+(B_StepToWl))*100; xsprintf((char*)str, (const char*)"%6.2f", wl/100); LCD_setPrint(100,7,font_3x4, (4<<1)|0); LCD_puts((uint8_t*)str);
		/*max*/   wl = ((A_StepToWl*u16_adc__maxValWL)+(B_StepToWl))*100; xsprintf((char*)str, (const char*)"%6.2f", wl/100); LCD_setPrint( 40,7,font_3x4, 0); LCD_puts((uint8_t*)str);
	}
}

void SF120_LCD_WL(uint8_t backColor)
{
	uint8_t str[30];

  #ifndef A_StepToWl
	LCD_setPrint(0,0,font_5x7,backColor);
	//xsprintf((char*)str, (const char*)"WL = %d; F=%d  ", sf120_WL, sf120_FIL);
	switch(sf120_WL)
	{
		case MOTOR_WL_DARCK:         xsprintf((char*)str, (const char*)"WL %d (Dark) ", sf120_WL); break;
		case MOTOR_WL_WHITE:         xsprintf((char*)str, (const char*)"WL %d (White)", sf120_WL); break;
		case MOTOR_WL_START_HALOGEN: xsprintf((char*)str, (const char*)"WL %d (Start)", sf120_WL); break;
		case MOTOR_WL_UPHIOLET:      xsprintf((char*)str, (const char*)"WL %d (UPhio)", sf120_WL); break;
		case MOTOR_WL_PHIOLET:       xsprintf((char*)str, (const char*)"WL %d (Phiol)", sf120_WL); break;
		case MOTOR_WL_BLUE:          xsprintf((char*)str, (const char*)"WL %d (Blue) ", sf120_WL); break;
		case MOTOR_WL_GREEN:         xsprintf((char*)str, (const char*)"WL %d (Green)", sf120_WL); break;
		case MOTOR_WL_YELLOW:        xsprintf((char*)str, (const char*)"WL %d (Yell) ", sf120_WL); break;
		case MOTOR_WL_ORANGE:        xsprintf((char*)str, (const char*)"WL %d (Orang)", sf120_WL); break;
		case MOTOR_WL_RED:           xsprintf((char*)str, (const char*)"WL %d (Red)  ", sf120_WL); break;
		case MOTOR_WL_IRED:          xsprintf((char*)str, (const char*)"WL %d (IRed) ", sf120_WL); break;
		default: xsprintf((char*)str, (const char*)"WL %d.%d nm     ", sf120_WL/10, sf120_WL%10); break;
	}
	LCD_puts(str);
	LCD_setPrint(83,0,font_5x7,backColor);
	xsprintf((char*)str, (const char*)" F%d ", sf120_FIL);
	LCD_puts(str);
  #else
	LCD_setPrint(0,0,font_5x7,backColor);
	//
	#if defined(XPRINTF_DEF)
		#if (XPRINTF_DEF == 2021)
			float wl;
			if(backColor == 0)
			{
				wl = ((A_StepToWl*sf120_WL)+(B_StepToWl))*100;
			}else if(backColor == 1){
				wl = ((A_StepToWl*sf120_WL_keyUpOrDown)+(B_StepToWl))*100;
			}
			xsprintf((char*)str, (const char*)"WL %7.2f nm, F%04d    ", wl/100, sf120_FIL);
		#else
			#error
		#endif
	#elif defined(_STRFUNC_ChaN)
		#if (_STRFUNC_ChaN == 2011)
			float wl;
			if(backColor == 0)
			{
				wl = ((A_StepToWl*sf120_WL)+(B_StepToWl))*100;
			}else if(backColor == 1){
				wl = ((A_StepToWl*sf120_WL_keyUpOrDown)+(B_StepToWl))*100;
			}
			uint32_t wl32_t = wl;
			xsprintf((char*)str, (const char*)"WL %d.%02d nm, F%04d    ", wl32_t/100, wl32_t%100, sf120_FIL);

		#else
			#error
		#endif



	#else
		#error
	#endif

	//
	str[20]='\0';
	LCD_puts(str);
  #endif

}
void SF120_LCD_SwitchPrintVal(uint8_t print)
{
	float wl;
	char str[20];
	switch (print)
	{
		case SWITCHPRINTVAL_WL_start:
			if( u16_adc__startWL  < (MOTOR_WL_START_HALOGEN-20) ) { u16_adc__startWL  = (MOTOR_WL_START_HALOGEN-20); }
			LCD_setPrint(10*6, 2, font_5x7, 0);
			wl = ((A_StepToWl*u16_adc__startWL)+(B_StepToWl))*100;
			xsprintf((char*)str, (const char*)"%7.2f nm", wl/100);
			str[20]='\0';
			LCD_puts(str);
			break;
		case SWITCHPRINTVAL_WL_finish:
			if( u16_adc__finishWL > MOTOR_WL_FINISH_HALOGEN     ) { u16_adc__finishWL = MOTOR_WL_FINISH_HALOGEN    ; }
			LCD_setPrint(10*6, 3, font_5x7, 0);
			wl = ((A_StepToWl*u16_adc__finishWL)+(B_StepToWl))*100;
			xsprintf((char*)str, (const char*)"%7.2f nm", wl/100);
			str[20]='\0';
			LCD_puts(str);
			break;
	}
}
void sf120_LCD_print_mode(uint8_t mode)
{
	SF120_LCD_print_Dig(0xFFFF, 0);

	for(uint8_t i=0; i<15; i++)//Clear string
	{
		LCD_putBitmap(0+(i*8),   6, 8+(i*8),   Terminal8x14+((' '-' ')*16), 16, 0);
	}

	if(mode==0)
	{
		  LCD_setPrint(0,7,font_5x7,1);
		  LCD_Line_H(0, 6, 127, 0x01<<7);//0b10000000 is Down line
		  LCD_puts((uint8_t*)" Please select MODE! ");
	}else{
	  #define MODE_LCD_START 4
	  #define MODE_LCD_POS1(x) (MODE_LCD_START+x)
	  #define MODE_LCD_POS2(x) (MODE_LCD_POS1(x)+8)
	uint8_t temp[6]={0,0,0,0,0,0};
	temp[mode%6]=1;

	  LCD_putBitmap(MODE_LCD_POS1(4),   6, MODE_LCD_POS2(4),   Terminal8x14+(('T'-' ')*16), 16, temp[1]);
	  LCD_putBitmap(MODE_LCD_POS1(30),  6, MODE_LCD_POS2(30),  Terminal8x14+(('A'-' ')*16), 16, temp[2]);
	  LCD_putBitmap(MODE_LCD_POS1(60),  6, MODE_LCD_POS2(60),  Terminal8x14+(('C'-' ')*16), 16, temp[3]);
	  LCD_putBitmap(MODE_LCD_POS1(85),  6, MODE_LCD_POS2(85),  Terminal8x14+(('F'-' ')*16), 16, temp[4]);
	  LCD_putBitmap(MODE_LCD_POS1(110), 6, MODE_LCD_POS2(110), Terminal8x14+(('S'-' ')*16), 16, temp[5]);
	}
}

//=============================================================================================================
void SF120_motorTx(uint8_t u08tx)
{
	//MOTOR_TX_delay_NOP(1); //HAL_Delay(1);
	#if defined(STEPMOTORDRV_ULN2003_CONNECT_TO_HALSPI_USE_HC595)
		//HAL_GPIO_WritePin(HC595_OE_MOTOR_GPIO_Port, HC595_OE_MOTOR_Pin, GPIO_PIN_RESET);//if coment to always HC595 OutputEnable
		HAL_GPIO_WritePin(HC595_Latch_MOTOR_GPIO_Port, HC595_Latch_MOTOR_Pin, GPIO_PIN_RESET);//HC595 CristalSellect and pepare to load
		//
		#if (STEPMOTORDRV_ULN2003_HC595_SPI_DATALEN == 8)
			#if 1 //Good
				HAL_SPI_Transmit(&STEPMOTORDRV_HC595_SPI, (uint8_t*)&u08tx, 1, 0xFF);
			#elif 1 //Good to
				uint16_t tx16bit = ((uint16_t)u08tx<<8);
				HAL_SPI_Transmit(&STEPMOTORDRV_HC595_SPI, (uint8_t*)&tx16bit, 2, 0xFF);
			#endif
		#elif (STEPMOTORDRV_ULN2003_HC595_SPI_DATALEN == 16)
			uint16_t tx16bit = u08tx;
			HAL_SPI_Transmit(&STEPMOTORDRV_HC595_SPI, (uint8_t*)&tx16bit, 1, 0xFF);
		#else
			#error
		#endif
		HAL_GPIO_WritePin(HC595_Latch_MOTOR_GPIO_Port, HC595_Latch_MOTOR_Pin, GPIO_PIN_SET);//HC595 load
	#else
		#error
	#endif
	//MOTOR_TX_delay_NOP(1);
	//MOTOR_TX_delay_NOP(1);
}

#define Motor_StopStep()    {;}/*not use*/
//#define STEPMODE_CONSTMASIV
#define STEPMODE_SWITCH
void SF120_motor_StepN(int32_t count, uint8_t motor)
{
	//
	static uint8_t  stage[2]={0,0};
	//
	uint8_t rotate=MOTOR_ROTATE_RIGHT;



	if(count < 0)
	{
		rotate=MOTOR_ROTATE_LEFT;
		count *=(-1);

		if(motor == MOTOR_MODE_WL)
		{
			if(count > MOTOR_WL_FINISH_HALOGEN) { count = MOTOR_WL_FINISH_HALOGEN; sf120_WL = MOTOR_WL_FINISH_HALOGEN; }
		/*}else if(motor == MOTOR_MODE_FIL){
			if(count > MOTOR_FIL_FULL_PERIOD) { count = MOTOR_FIL_FULL_PERIOD; sf120_FIL = MOTOR_FIL_FULL_PERIOD; }*/
		}
	}

	#define DELAY_in_StepN_Work 2
	#define DELAY_in_StepN_Free 0

	#if defined( STEPMODE_SWITCH)

	if(motor==MOTOR_MODE_WL){
	switch(stage[0]%8)
	{
	  	case 0: SF120_motorTx(0x01);      break;
	  	case 1: SF120_motorTx(0x01|0x08); break;
		case 2: SF120_motorTx(0x08);      break;
		case 3: SF120_motorTx(0x08|0x02); break;
		case 4: SF120_motorTx(0x02);      break;
		case 5: SF120_motorTx(0x02|0x04); break;
		case 6: SF120_motorTx(0x04);      break;
		case 7: SF120_motorTx(0x04|0x01); break;
	  	default: break;
	}}else if(motor==MOTOR_MODE_FIL){
	switch(stage[1]%8)
	{
		case 0: SF120_motorTx(0x10);      break;
		case 1: SF120_motorTx(0x10|0x20); break;
		case 2: SF120_motorTx(0x20);      break;
		case 3: SF120_motorTx(0x20|0x40); break;
		case 4: SF120_motorTx(0x40);      break;
		case 5: SF120_motorTx(0x40|0x80); break;
		case 6: SF120_motorTx(0x80);      break;
		case 7: SF120_motorTx(0x80|0x10); break;
		default: break;
	}}
	HC595_OE_MOTOR_ON();
	HAL_Delay(DELAY_in_StepN_Free);
	SF120_keyTx(1<<7);//Scan ROW_3
	//while(count-- && GETPIN_MOTOR_STOP() !=0)
	while(count--) //не перевіряти, бо в крайніх станах неможливо рушити
	{
		//
		if(motor==MOTOR_MODE_WL)
		{
			if(SF120_ReadKey_STOP() && rotate==MOTOR_ROTATE_LEFT){ sf120_WL=0; break;}
			if(SF120_ReadKey_START() && SF120_ReadKey_STOP() ){ break;}
			//
			if(rotate == MOTOR_ROTATE_LEFT) { sf120_WL--; stage[0]--; }else{ sf120_WL++; stage[0]++;}
			switch(stage[0]%8)
			{
				case 0: SF120_motorTx(0x01);      break;
				case 1: SF120_motorTx(0x01|0x08); break;
				case 2: SF120_motorTx(0x08);      break;
				case 3: SF120_motorTx(0x08|0x02); break;
				case 4: SF120_motorTx(0x02);      break;
				case 5: SF120_motorTx(0x02|0x04); break;
				case 6: SF120_motorTx(0x04);      break;
				case 7: SF120_motorTx(0x04|0x01); break;
				default: break;
			}/*end switch(MOTOR_WL)*/
		}else if(motor==MOTOR_MODE_FIL){
			if(rotate == MOTOR_ROTATE_LEFT) { sf120_FIL--; stage[1]--; }else{ sf120_FIL++; stage[1]++;}
			switch(stage[1]%8)
			{
				case 0: SF120_motorTx(0x10);      break;
				case 1: SF120_motorTx(0x10|0x20); break;
				case 2: SF120_motorTx(0x20);      break;
				case 3: SF120_motorTx(0x20|0x40); break;
				case 4: SF120_motorTx(0x40);      break;
				case 5: SF120_motorTx(0x40|0x80); break;
				case 6: SF120_motorTx(0x80);      break;
				case 7: SF120_motorTx(0x80|0x10); break;
				default: break;
			}//end switch(MOTOR_FIL)
	  }//END if( MOTOR )
	HAL_Delay(DELAY_in_StepN_Work);
	}//end while()

	HAL_Delay(DELAY_in_StepN_Work);
	Motor_StopStep();
	HC595_OE_MOTOR_OFF();

	sf120_WL_keyUpOrDown=sf120_WL;
#elif defined( STEPMODE_CONSTMASIV)
	//
	MODIFY_REG(MOTOR_GPIO_PORT->ODR, MOTOR_SET_MASK_ALL, MOTOR_SET_MASK[stage%4]);

	//while(count-- && GETPIN_MOTOR_STOP() !=0)
	while(count--) //не перевіряти, бо в крайніх станах неможливо рушити
	{
		if(rotate == MOTOR_ROTATE_LEFT && GETPIN_MOTOR_STOP_LEFT() !=0)
    {
			motorStepCounter--;
			stage--;
    }else if(rotate == MOTOR_ROTATE_RIGHT && GETPIN_MOTOR_STOP_RIGHT() !=0){
      motorStepCounter++;
		  stage++;
		}
		HAL_Delay(DELAY_in_StepN_Work);
		MODIFY_REG(MOTOR_GPIO_PORT->ODR, MOTOR_SET_MASK_ALL, MOTOR_SET_MASK[stage%4]);
		//
	}
	HAL_Delay(DELAY_in_StepN_Free);
	Motor_StopStep();
#else
	#error
#endif
	//return;
	#undef MOTOR_ROTATE_RIGHT
	#undef MOTOR_ROTATE_LEFT
	#undef DELAY_in_StepN_Work
	#undef DELAY_in_StepN_Free
}
//=====================================================================================================================
void sf120_setFilter(uint8_t filterEnum)
{
	SF120_motor_StepN(((-1)*MOTOR_WL_FINISH_HALOGEN), MOTOR_MODE_WL);
	SF120_motor_StepN(MOTOR_WL_BLUE, MOTOR_MODE_WL);

	uint16_t adcMax;

	#if defined( AD7705_H )
		uint8_t gainRestore = ad7705_enumDiv[AD7705_CH_SF120];
		//
	    //xsprintf((char*)cmdline, "sf init ADC 7 %c0", (AD7705_CH_SF120+'A') );cmd_process_loop();
		ad7705ResetSoftSync();
		ad7705ClockSetupSync(AD7705_MASTERCLK_MHZ49152, AD7705_UPDATE_HZ50, AD7705_CH_SF120);
		ad7705SetupAndCalibrateSync(0, AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_UNIPOLAR/*AD7705_SETUP_BIPOLAR*/, AD7705_CH_SF120);
		/*Кілька холостих визначень для стабілізації АЦП*/
		adcMax = ad7705GetADCSync(AD7705_CH_SF120);
		adcMax = ad7705GetADCSync(AD7705_CH_SF120);
		adcMax = ad7705GetADCSync(AD7705_CH_SF120);
		adcMax = ad7705GetADCSync(AD7705_CH_SF120);
	#else
		#error
		adcMax = sf120_getAdc(&hadc1);
		adcMax = sf120_getAdc(&hadc1);
		adcMax = sf120_getAdc(&hadc1);
		adcMax = sf120_getAdc(&hadc1);
	#endif
	for(uint16_t i=0; i<MOTOR_FIL_FULL_PERIOD; i++)
	{
		#if defined( AD7705_H )
		uint16_t adcTemp = ad7705GetADCSync(AD7705_CH_SF120);
		#else
		uint16_t adcTemp = sf120_getAdc(&hadc1);
		#endif
		if ( adcMax < adcTemp ){ adcMax = adcTemp; }
		SF120_motor_StepN(1, MOTOR_MODE_FIL);
	}
	//
	//uint32_t timeout = HAL_GetTick();
	for(uint8_t i=0; i<2; i++)
	{
	do{
			SF120_motor_StepN(1, MOTOR_MODE_FIL);
		#if 1
			if(SF120_ReadKey_STOP() ){ break;}
		#elif 0
			if (SF120_keyRx()!=0 )//if press any key
		#elif 0//Дуже довго чекати на спектр
			if (SF120_keyRx()==8 || (timeout+900000)<HAL_GetTick())//if press ESC or timeout
			{
				debug_str("setFil Fail!\r\n");
				return;
			}
		#endif
		//HAL_Delay(1);
		#if defined( AD7705_H )
		}while(ad7705GetADCSync(AD7705_CH_SF120) < (adcMax - (adcMax/10)) );//55000);//треба вибрати найбільше значення при якому надійно спрацьовує зупинка
		#else
		}while(sf120_getAdc(&hadc1) < (adcMax - (adcMax/10)) );//1000);//1600
		#endif
		SF120_motor_StepN(-300, MOTOR_MODE_FIL);
	}
	SF120_motor_StepN(25, MOTOR_MODE_FIL); // is a MOTOR_FIL_NONE1
	sf120_FIL = MOTOR_FIL_FULL_PERIOD-304;
	if(filterEnum < ENUM_FIL_ENUMS)
	{
		//SF120_motor_StepN(MOTOR_FIL_NEXT_N(p_arg[1][0]-'0'), MOTOR_MODE_FIL);
		SF120_motor_StepN(MOTOR_WL_WHITE-MOTOR_WL_BLUE, MOTOR_MODE_WL);
		switch(filterEnum){
			case ENUM_FIL_NONE1://sf120_FIL=MOTOR_FIL_NONE1; sf120_WL=MOTOR_WL_WHITE
				//SF120_motor_StepN(MOTOR_WL_WHITE-MOTOR_WL_BLUE, MOTOR_MODE_WL);
				SF120_motor_StepN(306, MOTOR_MODE_FIL);
				sf120_FIL -= MOTOR_FIL_FULL_PERIOD;
				//debug_str( "White1\r\n" );
				break;
			case ENUM_FIL_NONE2://sf120_FIL=MOTOR_FIL_NONE2; sf120_WL=MOTOR_WL_WHITE
				//SF120_motor_StepN(MOTOR_WL_WHITE-MOTOR_WL_BLUE, MOTOR_MODE_WL);
				SF120_motor_StepN(406, MOTOR_MODE_FIL);
				sf120_FIL -= MOTOR_FIL_FULL_PERIOD;
				//debug_str( "White2\r\n" );
				break;
			case ENUM_FIL_RED://sf120_FIL=MOTOR_FIL_RED; sf120_WL=MOTOR_WL_RED
				//SF120_motor_StepN(MOTOR_WL_RED-MOTOR_WL_BLUE, MOTOR_MODE_WL);
				SF120_motor_StepN(-305, MOTOR_MODE_FIL);
				//debug_str( "Red\r\n" );
				break;
			case ENUM_FIL_YELLOW://sf120_FIL=MOTOR_FIL_YELLOW; sf120_WL=MOTOR_WL_YELLOW
				//SF120_motor_StepN(MOTOR_WL_YELLOW-MOTOR_WL_BLUE, MOTOR_MODE_WL);
				SF120_motor_StepN(-203, MOTOR_MODE_FIL);
				//debug_str( "Yellow\r\n" );
				break;
			case ENUM_FIL_GREEN://sf120_FIL=MOTOR_FIL_GREEN; sf120_WL=MOTOR_WL_GREEN
				//SF120_motor_StepN(MOTOR_WL_GREEN-MOTOR_WL_BLUE, MOTOR_MODE_WL);
				SF120_motor_StepN(-101, MOTOR_MODE_FIL);
				//debug_str( "Green\r\n" );
				break;
			case ENUM_FIL_BLUE://sf120_FIL=MOTOR_FIL_BLUE; sf120_WL=MOTOR_WL_BLUE
				//SF120_motor_StepN(MOTOR_WL_BLUE-MOTOR_WL_BLUE, MOTOR_MODE_WL);
				SF120_motor_StepN(0, MOTOR_MODE_FIL);
				//debug_str( "Blue\r\n" );
				break;
			case ENUM_FIL_PHIOLET:
				//SF120_motor_StepN(MOTOR_WL_PHIOLET-MOTOR_WL_BLUE, MOTOR_MODE_WL);
				SF120_motor_StepN(102, MOTOR_MODE_FIL);
				//debug_str( "Phiol\r\n" );
				break;
			case ENUM_FIL_DARK:
				//SF120_motor_StepN(MOTOR_WL_DARCK-MOTOR_WL_BLUE, MOTOR_MODE_WL);
				SF120_motor_StepN(204, MOTOR_MODE_FIL);
				//debug_str( "Dark\r\n" );
				break;
			default: break;
		}//END switch
	}//END if(num_args==2)

	#if defined( AD7705_H )
		//xsprintf((char*)cmdline, "sf init ADC 7 %c%d", (AD7705_CH_SF120+'A'), gainRestore );cmd_process_loop();
		ad7705ResetSoftSync();
		ad7705ClockSetupSync(AD7705_MASTERCLK_MHZ49152, AD7705_UPDATE_HZ50, AD7705_CH_SF120);
		ad7705SetupAndCalibrateSync(gainRestore, AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_UNIPOLAR/*AD7705_SETUP_BIPOLAR*/, AD7705_CH_SF120);
		/*Кілька холостих визначень для стабілізації АЦП*/
		adcMax = ad7705GetADCSync(AD7705_CH_SF120);
		adcMax = ad7705GetADCSync(AD7705_CH_SF120);
		adcMax = ad7705GetADCSync(AD7705_CH_SF120);
		adcMax = ad7705GetADCSync(AD7705_CH_SF120);
	#else
		#error
		adcMax = sf120_getAdc(&hadc1);
		adcMax = sf120_getAdc(&hadc1);
		adcMax = sf120_getAdc(&hadc1);
		adcMax = sf120_getAdc(&hadc1);
	#endif
}
//----------------------------------------------------------------------
#if defined(u16_adc__startWL) && defined(u16_adc__finishWL) && defined(u16_adc__min) && defined(u16_adc__max)
void sf120_printSpectr( uint8_t count )
#else
#endif
{
	#if defined(u16_adc__startWL) && defined(u16_adc__finishWL) && defined(u16_adc__min) && defined(u16_adc__max)
		if(u16_adc__startWL > u16_adc__finishWL){ return; }
		if( u16_adc__startWL  < (MOTOR_WL_START_HALOGEN-20) ) { u16_adc__startWL  = (MOTOR_WL_START_HALOGEN-20); }
		if( u16_adc__finishWL > MOTOR_WL_FINISH_HALOGEN     ) { u16_adc__finishWL = MOTOR_WL_FINISH_HALOGEN    ; }
	#else
		if(start > finish){ return; }
		if( start  < (MOTOR_WL_START_HALOGEN-20) ) { start  = (MOTOR_WL_START_HALOGEN-20); }
		if( finish > MOTOR_WL_FINISH_HALOGEN     ) { finish = MOTOR_WL_FINISH_HALOGEN    ; }
	#endif

	lcd_Clear();
	LCD_setPrint(0, 0, font_5x7, 0); LCD_puts((uint8_t*)" sf120_printSpectr  ");
	LCD_Line_H(0, 1, 128, 0x00);//Clear progress bar

	#if defined(u16_adc__startWL) && defined(u16_adc__finishWL) && defined(u16_adc__min) && defined(u16_adc__max)
		u16_adc__min = 0xFFFF;
		u16_adc__max = 0x0000;
	#else
		u16_adc__start  = start;
		u16_adc__finish = finish;
		u16_adc__min = 0xFFFF;
		u16_adc__max = 0x0000;
	#endif

    #if 0
	LCD_setPrint(0,7,font_5x7,1); LCD_puts((uint8_t*)"Clear box press START");
	pDataUART1rx[1]='N';
	while( !SF120_ReadKey_START() && pDataUART1rx[1]=='N' ){ __NOP(); }
    #endif
	//
	if( u16_adc__startWL > MOTOR_WL_YELLOW_TO_RED ) { sf120_setFilter(ENUM_FIL_RED); } else
		if( u16_adc__startWL > MOTOR_WL_GREEN_TO_YELLOW ) { sf120_setFilter(ENUM_FIL_YELLOW); } else
			if( u16_adc__startWL > MOTOR_WL_BLUE_TO_GREEN ) { sf120_setFilter(ENUM_FIL_GREEN); } else
				if( u16_adc__startWL > MOTOR_WL_PHIOLET_TO_BLUE ) { sf120_setFilter(ENUM_FIL_BLUE); } else
					if( u16_adc__startWL >= MOTOR_WL_START_HALOGEN ) { sf120_setFilter(ENUM_FIL_PHIOLET); } else
						if( u16_adc__startWL >= (MOTOR_WL_START_HALOGEN-10) ) { sf120_setFilter(ENUM_FIL_NONE1); } else
							if( u16_adc__startWL >= (MOTOR_WL_START_HALOGEN-20) ) { sf120_setFilter(ENUM_FIL_NONE2); }
	//
	SF120_motor_StepN(((-1)*MOTOR_WL_FINISH_HALOGEN), MOTOR_MODE_WL); //set sf120_WL=0
	LCD_setPrint(0,7,font_5x7,1); LCD_puts((uint8_t*)"Press START to print ");
	SF120_motor_StepN(u16_adc__startWL, MOTOR_MODE_WL);
	xputs("\nSF120 spectrogramm data table.\n\f");

	uint16_t countStart=0;
	if(count == 0 ){
	  	//countStart=0;
	}else if(count == 1 && ( (u16_adc__finishWL - u16_adc__startWL) < (SIZE_UNION_ARRAY_TOTALU16/2) )){
	  	//countStart = 0;
	  	if( &ramArray.u16_adc[countStart+0] !=  &ramArray.u16_IoIt[0][0]) { xputs("Error array\n\f"); return; }
	}else if(count == 2 && ( (u16_adc__finishWL - u16_adc__startWL) < (SIZE_UNION_ARRAY_TOTALU16/2) )){
	  	countStart = SIZE_UNION_ARRAY_TOTALU16/2;
	  	if( &ramArray.u16_adc[countStart+0] !=  &ramArray.u16_IoIt[1][0]) { xputs("Error array\n\f"); return; }
	}

	pDataUART1rx[1]='N';
	while( !SF120_ReadKey_START() && pDataUART1rx[1]=='N' ){ __NOP(); }
	LCD_setPrint(0,7,font_5x7,1); LCD_puts((uint8_t*)"Press STOP to break  ");
	pDataUART1rx[1]='N';
	for(uint16_t i=0; i<(u16_adc__finishWL-u16_adc__startWL); i++)
	{
		  SF120_motor_StepN(1, MOTOR_MODE_WL);
		  if(u16_adc__startWL >= MOTOR_WL_START_HALOGEN)
		  {//якщо start < MOTOR_WL_START_HALOGEN, то всі вимірювання пройдуть без зміни фільтрів (це я використовую при відлагоджуванні)
			  if(        sf120_WL==MOTOR_WL_PHIOLET_TO_BLUE /*Phiol-Blue*/
					  || sf120_WL==MOTOR_WL_BLUE_TO_GREEN /*Blue-Green*/
					  || sf120_WL==MOTOR_WL_GREEN_TO_YELLOW /*Green-Yellow*/
					  || sf120_WL==MOTOR_WL_YELLOW_TO_RED /*Yellow-Red*/ )
			  { SF120_motor_StepN((int32_t)(MOTOR_FIL_NONE1-MOTOR_FIL_NONE2), MOTOR_MODE_FIL); }
		  }

		#if 1
			if(SF120_ReadKey_STOP() || pDataUART1rx[1] != 'N'){ break;}
		#elif 0
			if (SF120_keyRx()!=0 )//if press any key
		#elif 0//Дуже довго чекати на спектр
		  if (SF120_keyRx()==8 || (timeout+1200000)<HAL_GetTick())//if press ESC or timeout
		  {
				debug_str("getSpec aborted\r\n");
				return;
		  }
		#endif

		  //SF120_LCD_WL(0); //Показ на екран можна закоментувати щоб пришвидшити роботу
		  LCD_putOneByte(((sf120_WL-u16_adc__startWL)*128)/(u16_adc__finishWL-u16_adc__startWL), 1,0xFF);//Progress_bar
		  //HAL_Delay(1);

			#if defined(AD7705_H)
				#if 0//dont use ramArray
		  	  		xprintf("%04d,%04d,%05d\n\f",sf120_WL, sf120_getAdc(&hadc1), ad7705GetADCSync(AD7705_CH_SF120));
				#elif defined(u16_adc__startWL) && defined(u16_adc__finishWL) && defined(u16_adc__min) && defined(u16_adc__max)
		  	  		uint32_t tempVal = ad7705GetADCSync(AD7705_CH_SF120);
		  	  		for(uint8_t j=0; j<4; j++)
		  	  		{
		  	  			tempVal += ad7705GetADCSync(AD7705_CH_SF120);
		  	  		}
		  	  		ramArray.u16_adc[countStart+i] = tempVal / 5;
					//xprintf("%04d,%04d,%05d\n\f",sf120_WL, sf120_getAdc(&hadc1), ramArray.u16_adc[i]);//use ADC_STM32 and AD7705
		  	  		xprintf("%04d,%05d\n\f",sf120_WL, ramArray.u16_adc[countStart+i]);//use only AD7705
					if(count == 0 || count == 1){
						if( ramArray.u16_adc[countStart+i] > u16_adc__max ) { u16_adc__max = ramArray.u16_adc[countStart+i]; }
						if( ramArray.u16_adc[countStart+i] < u16_adc__min ) { u16_adc__min = ramArray.u16_adc[countStart+i]; }
					}
				#endif
			#else
				#if 0//dont use ramArray
					uint16_t adcU16 = sf120_getAdc(&hadc1);
					xprintf("%04d,%04d\n\f",sf120_WL, adcU16);
				#elif defined(u16_adc__startWL) && defined(u16_adc__finishWL) && defined(u16_adc__min) && defined(u16_adc__max)
					ramArray.u16_adc[countStart+i]=sf120_getAdc(&hadc1);
					xprintf("%04d,%04d\n\f",sf120_WL, ramArray.u16_adc[i]);
					if( ramArray.u16_adc[countStart+i] > u16_adc__max ) { u16_adc__max = ramArray.u16_adc[countStart+i]; }
					if( ramArray.u16_adc[countStart+i] < u16_adc__min ) { u16_adc__min = ramArray.u16_adc[countStart+i]; }
				#endif
			#endif
	}
	#if defined(u16_adc__startWL) && defined(u16_adc__finishWL) && defined(u16_adc__min) && defined(u16_adc__max)
		if(count == 2){
			for(uint16_t i=0; i<(u16_adc__finishWL-u16_adc__startWL); i++)
			{
				//ramArray.u16_IoIt[1][i] = ramArray.u16_IoIt[0][i] - ramArray.u16_IoIt[1][i];
				//ramArray.u16_IoIt[0][i] = ramArray.u16_IoIt[0][i] - ramArray.u16_IoIt[1][i];
				float tempVal = log10( (float)ramArray.u16_IoIt[0][i] / ramArray.u16_IoIt[1][i]);
				ramArray.u16_IoIt[0][i] = round(tempVal * 10000);
				if( ramArray.u16_IoIt[0][i] > u16_adc__max ) { u16_adc__max = ramArray.u16_IoIt[0][i]; }
				if( ramArray.u16_IoIt[0][i] < u16_adc__min ) { u16_adc__min = ramArray.u16_IoIt[0][i]; }
			}
		}
		for(uint16_t i=0; i<(u16_adc__finishWL-u16_adc__startWL); i++)
		{
			if( ramArray.u16_adc[countStart+i] ==  u16_adc__max ) {  u16_adc__maxValWL = u16_adc__startWL + i; break; }
			/*По суті це одне і те саме*/ //if( ramArray.u16_IoIt[0][i]        ==  u16_adc__max ) {  u16_adc__maxValWL = u16_adc__startWL + i; break; }
		}
		if( count == 0 || count == 2){
			SF120_LCD_prepareSpec(0);//ця функція перезаписує дані, тому при першому випадку її не викликаємо
		}
	#endif

	xputs("-----------------------\n\f");

	//sf120_LCD_print_mode(sf120_mode);
}
//=====================================================================================================================
void SF120_keyTx(uint8_t u08tx)
{
	#if 1 //disable to debug
	HAL_GPIO_WritePin(HC595_LCD_E_GPIO_Port, HC595_LCD_E_Pin, GPIO_PIN_SET);//Enable LCD previos operation
	KEY_TX_delay_NOP(40);  //HAL_Delay(40);//LCD_delay_NOP(40);
	HAL_GPIO_WritePin(HC595_LCD_E_GPIO_Port, HC595_LCD_E_Pin, GPIO_PIN_RESET);//HC595 CristalSellect
	KEY_TX_delay_NOP(1);  //HAL_Delay(1);//LCD_delay_NOP(1);

	#if defined(KEY_ULN2003_CONNECT_TO_HALSPI_USE_HC595)
		#if (KEY_ULN2003_HC595_SPI_DATALEN == 8)
			#if 0 //Good
				uint16_t tx16bit = ((uint16_t)u08tx | ((LCD_HC595_WIRES_CS2>>1) | LCD_HC595_WIRES_CS2) );
				tx16bit = ( (tx16bit>>8)|((tx16bit & 0xFF)<<8) );//Change bytes
			#else //Good too
				uint16_t tx16bit = ((uint16_t)u08tx<<8 | ((LCD_HC595_WIRES_CS2>>1) | LCD_HC595_WIRES_CS2) );
			#endif
			HAL_SPI_Transmit(&KEY_ULN2003_HC595_SPI,  (uint8_t*)&tx16bit, 2, 0xFF);
		#elif (KEY_ULN2003_HC595_SPI_DATALEN == 16)
			uint16_t tx16bit = ((uint16_t)u08tx | ((LCD_HC595_WIRES_CS2>>1) | LCD_HC595_WIRES_CS2) );
			HAL_SPI_Transmit(&KEY_ULN2003_HC595_SPI, (uint8_t*)&tx16bit, 1, 0xFF);
		#else
			#error
		#endif
	#else
		#error
	#endif
	KEY_TX_delay_NOP(1);  //HAL_Delay(1);//LCD_delay_NOP(1);
	HAL_GPIO_WritePin(HC595_LCD_E_GPIO_Port, HC595_LCD_E_Pin, GPIO_PIN_SET);//HC595 load, and Enable LCD operation
	KEY_TX_delay_NOP(1);  //HAL_Delay(1);//LCD_delay_NOP(1);
	#endif
}
uint8_t SF120_keyRx(void)
{
#if 0
	SF120_keyTx(1<<5);
	HAL_Delay(10);
	if(HAL_GPIO_ReadPin(SF120_KeyR3_GPIO_Port, SF120_KeyR3_Pin) == GPIO_PIN_SET){return 1;}
	if(HAL_GPIO_ReadPin(SF120_KeyR2_GPIO_Port, SF120_KeyR2_Pin) == GPIO_PIN_SET){return 2;}
	if(HAL_GPIO_ReadPin(SF120_KeyR1_GPIO_Port, SF120_KeyR1_Pin) == GPIO_PIN_SET){return 3;}
	SF120_keyTx(1<<6);
	HAL_Delay(10);
	if(HAL_GPIO_ReadPin(SF120_KeyR3_GPIO_Port, SF120_KeyR3_Pin) == GPIO_PIN_SET){return 4;}
	if(HAL_GPIO_ReadPin(SF120_KeyR2_GPIO_Port, SF120_KeyR2_Pin) == GPIO_PIN_SET){return 5;}
	if(HAL_GPIO_ReadPin(SF120_KeyR1_GPIO_Port, SF120_KeyR1_Pin) == GPIO_PIN_SET){return 6;}
	SF120_keyTx(1<<7);
	HAL_Delay(10);
	if(HAL_GPIO_ReadPin(SF120_KeyR3_GPIO, SF120_KeyR3_Pin) == GPIO_PIN_SET){return 7;}
	if(HAL_GPIO_ReadPin(SF120_KeyR2_GPIO, SF120_KeyR2_Pin) == GPIO_PIN_SET){return 8;}
	if(HAL_GPIO_ReadPin(SF120_KeyR1_GPIO, SF120_KeyR1_Pin) == GPIO_PIN_SET){return 9;}
#else
	for(int8_t i=2; i>=0; i--)
	//for(uint8_t i=0; i<3; i++)
	{
		SF120_keyTx(1<<(i+5));
		KEY_TX_delay_NOP(1);  //HAL_Delay(1);
		#if 1
		if(HAL_GPIO_ReadPin(SF120_KeyR3_GPIO_Port, SF120_KeyR3_Pin) == GPIO_PIN_SET){return 1+i*3;}
		if(HAL_GPIO_ReadPin(SF120_KeyR2_GPIO_Port, SF120_KeyR2_Pin) == GPIO_PIN_SET){return 2+i*3;}
		if(HAL_GPIO_ReadPin(SF120_KeyR1_GPIO_Port, SF120_KeyR1_Pin) == GPIO_PIN_SET){return 3+i*3;}
		#else //Так просто тут тут не буде, тому оставив як було
		uint32_t temp = (SF120_KeyR1_GPIO_Port->IDR & (SF120_KeyR3_Pin|SF120_KeyR2_Pin|SF120_KeyR1_Pin))<<7;
		if(temp) {return temp +i*3;}
		#endif
	}
#endif
	return 0;
}
//-----------------------------------------------------------------------------------------------------------


void getOpticalAdsorbance(uint8_t countIndex)
{
	//SF120_LCD_print_DigFloat(FLOAT_MAXIMAL, 1);
	//LCD_setPrint( 0,2,Terminal8x14, 1); LCD_puts((uint8_t*)"");//14 chars
	LCD_setPrint( 0,2,font_5x7,1); LCD_puts((uint8_t*)"                     ");//21 chars
	LCD_setPrint( 0,3,font_5x7,1); LCD_puts((uint8_t*)"                     ");//21 chars
	LCD_setPrint( 0,4,font_5x7,1); LCD_puts((uint8_t*)"                     ");//21 chars
	uint8_t str4_set[4]="SET";
	uint8_t str4_get[4]="Ads";
	if(countIndex == 1 ) { SF120_LCD_l4_END(str4_set,1); }
	else if(countIndex == 2 ) { SF120_LCD_l4_END(str4_get,1); }
	else { return; }

	uint8_t str3[4]="   ";
	uint32_t tempVal=0;

	if(countIndex == 1)
	{
		for(uint8_t i=0; i<8; i++)
		{
			ad7705ResetSoftSync();
			ad7705ClockSetupSync(AD7705_MASTERCLK_MHZ49152, AD7705_UPDATE_HZ50, AD7705_CH_SF120);
			ad7705SetupAndCalibrateSync(i, AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_UNIPOLAR/*AD7705_SETUP_BIPOLAR*/, AD7705_CH_SF120);
			HAL_Delay(2000);//minimum 2000ms
			tempVal = ad7705GetADCSyncRound(AD7705_CH_SF120);
			xprintf("ADC Io = %.4E\n\f", round((float)tempVal/ad7705_floatDiv[ad7705_enumDiv[AD7705_CH_SF120]]) );
			if( tempVal > 32000 )
			{
				adcVal[countIndex] = tempVal;
				SF120_LCD_print_DigFloat(adcVal[countIndex]/ad7705_floatDiv[ad7705_enumDiv[AD7705_CH_SF120]], 1);
				str3[1] = i+'0';
				SF120_LCD_l3_END(str3,1);
				return;
			}
		}
	}else if(countIndex == 2 ) {
		tempVal = ad7705GetADCSyncRound(AD7705_CH_SF120);
		adcVal[countIndex] = tempVal;
		xprintf("ADC It = %.4E\n\f", round((float)adcVal[countIndex]/ad7705_floatDiv[ad7705_enumDiv[AD7705_CH_SF120]] ) );
		SF120_LCD_l3_END(str3,0);
		SF120_LCD_print_DigFloat((float)log10( (float)(adcVal[1]-adcVal[0])/(float)(adcVal[2]-adcVal[0]) ), 0);
		xprintf("Ads = %.4E\n\f", adcVal[countIndex]/ad7705_floatDiv[ad7705_enumDiv[AD7705_CH_SF120]] );
	}
}


