//cmd_func_user.c

#include "main.h"
#include "stm32F1_hal_analog.h"
#include "sf120_if.h"
#include "ad7705.h"

extern uint16_t adcVal[3]; //Dark; I0; It

//#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Third_Party/FatFs_R0_12b__elm_chan/ffsample_stm32/xprintf.h"
//#include "/media/odmin/MyBook/embed/stm32/libThird_Party/FatFs_Chan/ff15_ffsample_stm32/xprintf.h"

#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_github_com/just4fun/AVR_CMD/cmd_exec_v_Argonte/cmd_exec.h"

#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Argonte/itoa/atoi.h"
#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Argonte/itoa/atoi.c"

#define FUNC_NUM        3

#if ( defined(_STRFUNC_ChaN) || defined(XPRINTF_DEF) )
  const char msg_cmd_err[]="ERROR!";
  const char msg_cmd[]    ="cmd: ";
//-----------
const uint8_t help_text[] = {	
#if 0	
"SF120 command shell v1.0\n\
Currently supports three commands:\n\
\tsf getSpec 2795 8783 -Зняти спектр (криву ацп даних) без переміни фільтрів на ENUM_FIL_NONE1\
\tsf getSpec 2785 8783 -Зняти спектр (криву ацп даних) без переміни фільтрів на ENUM_FIL_NONE2\
\tsf getSpec 2785 8783 -Зняти спектр (криву ацп даних) без переміни фільтрів на ENUM_FIL_NONE2"
#else
"?\n"
#endif	
};
#endif

void run_Print(uint8_t* p_arg[],uint8_t num_args)
{
	static uint32_t count=0;
	uint32_t value;
	char** str;
	long val_L;
	//
	if(num_args==0)
	{
		//"print"
			if (FLAG_TERMINALPRINTF)
	    {
				#if 0 //is debug
			  xputs("xputs\n0123456789012345678901234567890123456789\n\f");
			  xprintf("xprintf\n0123456789012345678901234567890123456789\nCount= %05d\n\f", count++);
			  xputs ((const char*) "Dis is a string \"xputs\"\n\f");
			  uint16_t buff[10]={1,2,3,4,5,6,7,8,9,0};
			  xputs("ADC dump\n"); for(uint8_t i=0; i<10; i++) { xput_dump (&buff[i], i, 1, DW_DECIMAL16); }; xputc('\f');
				#elif 0
				xputs((uint8_t*)help_text);
				#endif
		  }
}else if(  str_equal_pgm(p_arg[0], (const uint8_t*)"ID")){
		/*96-битный уникальный идентификатор устройства обеспечивает ссылочный номер, который является уникальным для любого
      устройства и в любом контексте. Эти биты не могут быть изменены пользователем.*/
		#if defined(UID_BASE)
		  xputs("\nUID_BASE (unique device identifier)\n\f");
		  xput_dump ((uint8_t*)UID_BASE, UID_BASE, 3, DW_LONG);xputc('\f');/*!<0x1FFFF7E8UL Unique device ID register base address */
		#endif
		/**/
		#if defined(DBGMCU_BASE)
		  xputs("\nDBGMCU_IDCODE\n\f");
		  xput_dump ((uint8_t*)DBGMCU_BASE, DBGMCU_BASE, 1, DW_LONG);xputc('\f');/*!<0xE0042000UL Debug MCU registers base address*/
		  //value =(uint32_t) *((uint32_t*)((uint8_t*)DBGMCU_BASE )) & (uint32_t)0x0FFF;//DEV_ID
		  //value =*((uint32_t*)DBGMCU_BASE ) & (uint32_t)0x0FFF;//DEV_ID
		  value =*((uint16_t*)DBGMCU_BASE ) & (uint16_t)0x0FFF;//DEV_ID
		  xprintf("DEV_ID = 0x%03X  (", value);
		#if 0 //Program Size: Code=14924 RO-data=456 RW-data=528 ZI-data=3688  
		  switch (value)
			{
				case 0x412: xputs("Low-density devices)\n\f");break;// low-density devices, the device ID is 0x412
				case 0x410: xputs("Medium-density devices)\n\f");break;// medium-density devices, the device ID is 0x410
				case 0x414: xputs("High-density devices)\n\f");break;// high-density devices, the device ID is 0x414
				case 0x430: xputs("XL-density devices)\n\f");break;// XL-density devices, the device ID is 0x430
				case 0x418: xputs("Connectivity devices)\n\f");break;// connectivity devices, the device ID is 0x418
        default: xputs("Unknovn devices)\n\f");break;
			}
			#else //Program Size: Code=14920 RO-data=456 RW-data=528 ZI-data=3688  
			  if     (value == 0x412) { xputs("Low-density devices)\n\f" ); }
				else if(value == 0x410) { xputs("Medium-density devices)\n\f" ); }
				else if(value == 0x414) { xputs("High-density devices)\n\f" ); }
				else if(value == 0x430) { xputs("XL-density devices)\n\f" ); }
				else if(value == 0x418) { xputs("Connectivity devices)\n\f" ); }
				else                    { xputs("Unknovn devices)\n\f" ); }
			#endif
			//value =(uint32_t) *((uint32_t*)((uint8_t*)DBGMCU_BASE )) >> 16;//REV_ID//Program Size: Code=14712 RO-data=456 RW-data=528 ZI-data=3688   
			//value =*((uint32_t*)DBGMCU_BASE ) >> 16;//REV_ID                     //Program Size: Code=14712 RO-data=456 RW-data=528 ZI-data=3688  
			value =*((uint16_t*)DBGMCU_BASE +1 );//REV_ID                      //Program Size: Code=14712 RO-data=456 RW-data=528 ZI-data=3688  
			xprintf("REV_ID = 0x%04X\n\f", value);
		#endif
		/**/
		#if defined(FLASHSIZE_BASE)
		  xputs("\nFLASHSIZE_BASE (FLASH Size in kilobytes)\n");
			xput_dump ((uint8_t*)FLASHSIZE_BASE, FLASHSIZE_BASE, 1, DW_SHORT);
		  xprintf("FLASH Size is %d kb\n\f", (*((uint16_t *)FLASHSIZE_BASE)) );
    	#endif
		return;
	}else if(  str_equal_pgm(p_arg[0], (const uint8_t*)"flash"))
	{
		//"print flash 100"
		//
		xprintf("\nFLASH Size is %d kb\n\f", (*((uint16_t *)FLASHSIZE_BASE)) );//#define FLASHSIZE_BASE 0x1FFFF7E0UL  //in stm32f103xe.h
		if(num_args==1)
		{
			//xputs("uncnovn args");
		}else if(num_args==2){
			if((p_arg[1][0]>='0') && (p_arg[1][0]<='9'))
			{
				#if 0
				  value= atou16(p_arg[1]);//16 bit not dostatno
				#elif 0//Program Size: Code=14252 RO-data=456 RW-data=528 ZI-data=3688  
				  str = (char**)&p_arg[1];
				  xatoi( str, (long*)&value);
				#elif 1 // Program Size: Code=14236 RO-data=456 RW-data=528 ZI-data=3688    
				  str = (char**)&p_arg[1];
				  xatoi( str, (long*)&val_L);
				  value = val_L;
				#else
				  #error
				#endif
				#if 1

				if(value > 1024 * (*((uint16_t *)FLASHSIZE_BASE)) || value == 0) value = 1024 * (*((uint16_t *)FLASHSIZE_BASE) );
				xprintf("value of print flash  = %d\n", (uint32_t)value);
				xputs("=========FLASH=========================\n");
				//Realterm при великих обємах (>1000) зависає і зупиняє MCU. В режимі Capture працює нормально.
				for(uint32_t i=FLASH_BASE; i< (FLASH_BASE + value); i+=16)
				{
					xput_dump ((uint8_t*)i, i, 16, DW_CHAR);
				}
				xputs("END FLASH =========================\n\f");
				#endif
			}
		}
		
		return;
	}
	else if(num_args==3)
	{
		//
	}
	return;
}


extern void SF120_LCD_print_WL(uint16_t wl, uint8_t backColor);

extern ADC_HandleTypeDef hadc1;


int16_t atoi08decimal(char* s)
{
#if 1
	uint8_t i = 0;
	int16_t n=0;
    //n = 0;
    for ( ; s[i] >= '0' && s[i] <= '9'; ++i)
    {
    	n = 10 * n + (s[i] - '0');
    }
    if(s[i]=='-') { return n*(-1); }
    else { return n; }
#else
	uint8_t i = 0;
	int16_t sign;
	int16_t n=0;
    //n = 0;
	if(s[i]=='-'){sign = -1; i++;}
	else if(s[i]=='+'){sign = 1; i++;}
    for ( ; s[i] >= '0' && s[i] <= '9'; )
    {
    	n = 10 * n + (s[i++] - '0');
    }
    return n*sign;
#endif
}
void func_Test(uint8_t* p_arg[],uint8_t num_args)
{
//	static uint32_t count=0;
//	long value;
//	uint16_t u16Temp;

	if(num_args==0)
	{
	}
	else if(  str_equal_pgm(p_arg[0], (const uint8_t*)"echo"))
	{
		//"test echo 11"//11-Вивыд дозволено. 01-то в термыналі можна влючити софтварний ехо. 00-бінарний режим передачі даних
		//FLAG_CDC_ECHO = p_arg[1][0]-'0';
		//FLAG_TERMINALPRINTF = p_arg[1][1]-'0';
		//if(FLAG_TERMINALPRINTF) xprintf("FLAG_TERMINALPRINTF %d\nFLAG_CDC_ECHO %d\n",FLAG_TERMINALPRINTF, FLAG_CDC_ECHO);
	}
	else if(num_args==2)
	{
		if(str_equal_pgm(p_arg[0], (const uint8_t*)"motor") /*&& p_arg[1][0]>='+' &&  p_arg[1][0]<='9'*/)
		{
			//uint8_t value = atou16(p_arg[1]);
			xputs(" motor... ");
#if 0
			int16_t value = atoi08decimal((char*)p_arg[1]);
			xprintf("SF120_motorTx(%d);\n\f",value);
			HC595_OE_MOTOR_ON();
			SF120_motorTx((uint8_t)value);
#elif 1
			char** str;
			long val_L;
			str = (char**)&p_arg[1];
			xatoi( str, (long*)&val_L);
			xprintf("SF120_motor_StepN(%d);\n\f",(int32_t)val_L);
			SF120_motor_StepN((int32_t)val_L, 1);
			SF120_LCD_WL(0);
			xprintf("SF120_SetWL(%d);\n\f",(int32_t)val_L);
#elif 1
			int16_t value = atoi08decimal((char*)p_arg[1]);
			HC595_OE_MOTOR_ON();
			if(value>0){
			for(int16_t i=0; i<value; i++)
			{
				SF120_motorTx(0x01);
				HAL_Delay(20);
				SF120_motorTx(0x04);
				HAL_Delay(20);
				SF120_motorTx(0x02);
				HAL_Delay(20);
				SF120_motorTx(0x01);
				HAL_Delay(20);
			}}else{
			for(int16_t i=0; i<value; i++)
			{
				SF120_motorTx(1<<3);
				HAL_Delay(20);
				SF120_motorTx(1<<1);
				HAL_Delay(20);
				SF120_motorTx(1<<2);
				HAL_Delay(20);
				SF120_motorTx(1<<0);
				HAL_Delay(20);
			}
			}
			HC595_OE_MOTOR_OFF();
#endif

		}
	}
	//return;
}
//--------------------
void run_SpectroFotometrDevice(uint8_t* p_arg[],uint8_t num_args)
{
	if(num_args==0)
	{
		xputs("SF120 is run\n\f");
		return;
	}else{
	if(  str_equal_pgm(p_arg[0], (const uint8_t*)"res")){
		xputs("RESET!\n\f");
		HAL_Delay(2000);
		NVIC_SystemReset();
	}else if(  str_equal_pgm(p_arg[0], (const uint8_t*)"init")){
		/*sf init ADC 1 A*/  /*Init ADC1 CHANNEL_0 */
		/*sf init ADC 1 B*/  /*Init ADC1 CHANNEL_1 */
		/*sf init ADC 1 N*/  /*DeInit ADC1 */
		if(num_args==4)
		{
			if(  str_equal_pgm(p_arg[1], (const uint8_t*)"ADC") )
			{
				if(p_arg[2][0]=='1')
				{
					hadc1.Instance = ADC1;
					ADC_Init(&hadc1);
					xprintf("ADC1 Init\n\f");
					if(p_arg[3][0]=='A')
					{
						ADC_ConfigChannel(&hadc1, ADC_CHANNEL_0);
						xprintf("ADC1 Ch0\n\f");
						return;
					}else if(p_arg[3][0]=='B')
					{
						ADC_ConfigChannel(&hadc1, ADC_CHANNEL_1);
						xprintf("ADC1 Ch1\n\f");
						return;
					}else if(p_arg[3][0]=='N'){
						HAL_ADC_DeInit(&hadc1);
						xprintf("ADC1 DeInit\n\f");
						return;
					}
				}else if(p_arg[2][0]=='7'){/*AD7705*/
					//ADC7705_Init();
					//HAL_Delay(2000);
					/*sf init ADC 7 A0*/

					ad7705ResetSoftSync();

					xprintf("\nAD7705 Init ");
					if(p_arg[3][0]=='A' || p_arg[3][0]=='B')
					{
						ad7705ClockSetupSync(AD7705_MASTERCLK_MHZ49152, AD7705_UPDATE_HZ50, (p_arg[3][0]-'A')/*AD7705_CH1*/);
						if( p_arg[3][1]>='0' && p_arg[3][1]<='7' )
						{
							if(p_arg[3][0] == 'A' || p_arg[3][0]=='B')
							{
								ad7705SetupAndCalibrateSync((p_arg[3][1]-'0')/*7*/, AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_UNIPOLAR/*AD7705_SETUP_BIPOLAR*/, (p_arg[3][0]-'A')/*AD7705_CH1 or 2*/);
							}else if(p_arg[3][0] == 'C' || p_arg[3][0]=='D')
							{
								ad7705SetupNormalSync((p_arg[3][1]-'0')/*0to7*/, AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_UNIPOLAR/*AD7705_SETUP_BIPOLAR*/, (p_arg[3][0]-'C')/*AD7705_CH1 or 2*/);
								p_arg[3][0]-2;// conv 'C' to 'A' or 'D' to 'B'
							}
							xprintf("CH%d x%d Buff Unipolar, div=%f\n\f", (p_arg[3][0]-'A'+1), (p_arg[3][1]-'0'), (p_arg[3][0]-'A'),  ad7705_floatDiv[p_arg[3][1]-'0']);
							//p_arg[3][0]='d';
							p_arg[3][2]='\0';
							SF120_LCD_l2_END(&p_arg[3][0], 0);//wiew chanel and divIndex
							HAL_Delay(2000);
							return;
						}
					}
				}
			}
		}
	}else if(  str_equal_pgm(p_arg[0], (const uint8_t*)"getAdc")){
		if(num_args==1)
		{
			uint16_t adcU16[2];
			adcU16[0]= sf120_getAdc(&hadc1);
			SF120_LCD_print_Dig(adcU16[0], 1);
			xprintf("adc[0] = %05d\n\f",adcU16[0]);
			uint32_t timeout = HAL_GetTick();
			while( 1 )//Очікування на заміну кювети
			{
				//if (SF120_keyRx()!=0 )//if press any key
				if (SF120_keyRx()==9 )//if press Enter
				{
					//xprintf("break\n\f");
					break;
				}
				if( (timeout+60000)<HAL_GetTick() || SF120_keyRx()==8) //Timeout or Esc
				{//Відміна, друге АЦП не потібне
					//xprintf("return\n\f");
					return;
				}
			}
			adcU16[1]= sf120_getAdc(&hadc1);
			SF120_LCD_print_Dig(adcU16[1], 1);
			xprintf("adc[1] = %05d\n\f",adcU16[1]);
			while( 1 )//Очікування щоб бачити результт
			{
				if (SF120_keyRx()!=0 )//if press any key
				{
					//xprintf("break\n\f");
					break;
				}
			}
			SF120_LCD_print_Dig(adcU16[1]-adcU16[0], 0);
			xprintf("T = %05d\n\f",adcU16[1]-adcU16[0]);
			return;
		}else if(num_args==2){
			if(p_arg[1][0]=='d'){/*STM32_ADC*/
				sf120_setFilter(0);
				adcVal[0]= sf120_getAdc(&hadc1);
				SF120_LCD_print_Dig(adcVal[0], 1);
			}else if(p_arg[1][0]=='n'){/*STM32_ADC*/
				adcVal[1]= sf120_getAdc(&hadc1);
				SF120_LCD_print_Dig(adcVal[1], 1);
			}else if(p_arg[1][0]=='t'){/*STM32_ADC*/
				adcVal[2]= sf120_getAdc(&hadc1);
				SF120_LCD_print_Dig(adcVal[1]-adcVal[2]-adcVal[0], 0);
			}else if(p_arg[1][0]=='D'){/*AD7705_CH1*/
				sf120_setFilter(0);
				adcVal[0]= ad7705GetADCSyncRound(AD7705_CH1);
				SF120_LCD_print_DigFloat(adcVal[0]/ad7705_floatDiv[AD7705_CH1], 1);
			}else if(p_arg[1][0]=='E'){/*AD7705_CH2*/
				sf120_setFilter(0);
				adcVal[0]= ad7705GetADCSyncRound(AD7705_CH2);
				SF120_LCD_print_DigFloat(adcVal[0]/ad7705_floatDiv[AD7705_CH2], 1);
			}else if(p_arg[1][0]=='N'){/*AD7705_CH1*/
					adcVal[1]= ad7705GetADCSyncRound(AD7705_CH1);
					SF120_LCD_print_DigFloat(adcVal[1]/ad7705_floatDiv[AD7705_CH1], 1);
			}else if(p_arg[1][0]=='O'){/*AD7705_CH2*/
					adcVal[1]= ad7705GetADCSyncRound(AD7705_CH2);
					SF120_LCD_print_DigFloat(adcVal[1]/ad7705_floatDiv[AD7705_CH2], 1);
			}else if(p_arg[1][0]=='I'){/*Io*/
				getOpticalAdsorbance(1);
				return;
			}else if(p_arg[1][0]=='T'){/*It*/
				getOpticalAdsorbance(2);
				return;
			}
			xprintf("wl=%d steps; fil=%d steps\n\tadc[0]=%d\n\tadc[1]=%d\n\tadc[2]=%d\n\f", sf120_WL, sf120_FIL, adcVal[0],adcVal[1],adcVal[2]);
			SF120_LCD_WL(0);
			return;
		}else if(num_args==3){
			adcVal[0] = 0;
			if(p_arg[1][0]=='A' && p_arg[2][0]=='0'){
				adcVal[1]= ADC7705_ReadData(0);
				SF120_LCD_print_Dig(adcVal[1], 0);
			}else if(p_arg[1][0]=='A' && p_arg[2][0]=='1'){
				adcVal[2]= ADC7705_ReadData(1);
				SF120_LCD_print_Dig(adcVal[2], 0);
			}else if(p_arg[1][0]=='A' && p_arg[2][0]=='2'){
				uint16_t data[20];
				AD7705_codeFromDatasheet(data, 20);
				xputs("AD7705 data\n\f");
				xput_dump ((uint8_t*)data,       0, 10, DW_DECIMAL16);
				xput_dump ((uint8_t*)&data[10], 10, 10, DW_DECIMAL16);
				return;
			}else if(p_arg[1][0]=='A' && p_arg[2][0]=='4'){
				AD7705_softwareReset();
				AD7705_init();
				    HAL_Delay(1);
				    xprintf("AD7705: %d, %d, %d, %d\n\f", AD7705_ReadData(), AD7705_ReadData(), AD7705_ReadData(), AD7705_ReadData() );
				    return;
			}else if(p_arg[1][0]=='A' && p_arg[2][0]=='5'){
				ADC7705_Init();
				HAL_Delay(3000);
				//https://gist.github.com/tspspi/b97777ffe5cc31e46dd9786f4bab4e0d
				ad7705ResetSoftSync();
					ad7705ClockSetupSync(AD7705_MASTERCLK_MHZ49152, AD7705_UPDATE_HZ50, AD7705_CH_SF120);
					ad7705SetupAndCalibrateSync(0/*0to7*/, AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_UNIPOLAR/*AD7705_SETUP_BIPOLAR*/, AD7705_CH_SF120);
					//ad7705SetupNormalSync(0/*0to7*/, AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_UNIPOLAR/*AD7705_SETUP_BIPOLAR*/, AD7705_CH_SF120);
					HAL_Delay(3000);
					xprintf("AD7705: %d, %d, %d, %d\n\f", ad7705GetADCSync(AD7705_CH_SF120), ad7705GetADCSync(AD7705_CH_SF120), ad7705GetADCSync(AD7705_CH_SF120), ad7705GetADCSync(AD7705_CH_SF120));
				return;
			}else if(p_arg[1][0]=='N'){/*AD7705_CH1*/
				if(p_arg[2][0]=='w')
				{
					xputs("ADC while ------------------------------\n\f");
					uint8_t lcdX=0;
					uint32_t tempVal = ad7705GetADCSyncRound(AD7705_CH_SF120);
					while(1)
					{
						tempVal = ad7705GetADCSyncRound(AD7705_CH_SF120);

						uint8_t temp = 64 - (tempVal*64)/0xFFFF;
						xprintf("%09d,%05d\n\f", HAL_GetTick(), tempVal);
						if(temp>=64) { temp = 63; }
						LCD_putPix( lcdX++, temp );
						lcdX %=128;
						for(uint8_t l= 0; l<8; l++)
						{
							LCD_putOneByte(lcdX,   l, 0x00);
							LCD_putOneByte(lcdX+1, l, 0xFF);
						}
						//if(SF120_ReadKey_STOP()){ break;}//закоментував бо всеодно після дисплейної функції клава не читається
						//HAL_Delay(1000);
					}
				}
				return;

			#if 0 //debug
			}else if(p_arg[1][0]=='A' && p_arg[2][0]=='C'){
				ADC7705_CS = 1;
				return;
			}else if(p_arg[1][0]=='A' && p_arg[2][0]=='c'){
				ADC7705_CS = 0;
				return;
			#if defined(ADC7705_RESET)
			}else if(p_arg[1][0]=='A' && p_arg[2][0]=='R'){
				ADC7705_RESET = 1;
				return;
			}else if(p_arg[1][0]=='A' && p_arg[2][0]=='r'){
				ADC7705_RESET = 0;
				return;
			#endif
			#endif//END debug
			}
			xprintf("wl=%d steps; fil=%d steps\n\tadc[0]=%d\n\tadc[1]=%d\n\tadc[2]=%d\n\f", sf120_WL, sf120_FIL, adcVal[0],adcVal[1],adcVal[2]);
			SF120_LCD_WL(0);
			return;
		}

	}else if(  str_equal_pgm(p_arg[0], (const uint8_t*)"mot")){
		/*sf mot 1 100*//*Rotation WL motor*/
		/*sf mot 1 -100*//*Rotation WL motor*/
		/*sf mot 2 102*//*Rotation FIL motor */
		/*sf mot 2 -102*//*Rotation FIL motor*/
		//if(num_args==3 && ((p_arg[2][0]>='0' && p_arg[2][0]<='9') || (p_arg[2][0]=='-') || (p_arg[2][0]=='+') ) ) {
			int16_t value = atoi08decimal((char*)p_arg[2]);
			SF120_motor_StepN(value, p_arg[1][0]-'0');
			SF120_LCD_WL(0);
			return;
		//}
	}else if( str_equal_pgm(p_arg[0], (const uint8_t*)"getSpec") ){
		if(num_args >=3 ){
			u16_adc__startWL = atoi08decimal((char*)p_arg[1]);
			u16_adc__finishWL = atoi08decimal((char*)p_arg[2]);
		}else{
			u16_adc__startWL = MOTOR_WL_PHIOLET_TO_BLUE; //MOTOR_WL_START_HALOGEN;
			u16_adc__finishWL = MOTOR_WL_YELLOW_TO_RED; //MOTOR_WL_FINISH_HALOGEN;
		}
		if(num_args==1)
		{
			sf120_printSpectr(0);
			return;
		}else if(num_args==3){
			sf120_printSpectr(0);
			return;
		}else if(num_args==4){
			if(p_arg[3][0] == 'o') { sf120_printSpectr(1); }
			else if(p_arg[3][0] == 't') { sf120_printSpectr(2); }
			else { sf120_printSpectr(0); }
			return;
		}
	}else if( str_equal_pgm(p_arg[0], (const uint8_t*)"getSpecWL") ){
		if(num_args >=3 ){
			u16_adc__startWL = atoi08decimal((char*)p_arg[1]);
			u16_adc__finishWL = atoi08decimal((char*)p_arg[2]);
			float temp = round(((float)u16_adc__startWL-B_StepToWl)/A_StepToWl);
			u16_adc__startWL = temp;
			temp = round(((float)u16_adc__finishWL-B_StepToWl)/A_StepToWl);
			u16_adc__finishWL = temp;
		}else{
			u16_adc__startWL = MOTOR_WL_PHIOLET_TO_BLUE; //MOTOR_WL_START_HALOGEN;
			u16_adc__finishWL = MOTOR_WL_YELLOW_TO_RED; //MOTOR_WL_FINISH_HALOGEN;
		}
		if(num_args==1)
		{
			sf120_printSpectr(0);
			return;
		}else if(num_args==3){
			sf120_printSpectr(0);
			return;
		}else if(num_args==4){
			if(p_arg[3][0] == 'o') { sf120_printSpectr(1); }
			else if(p_arg[3][0] == 't') { sf120_printSpectr(2); }
			else { sf120_printSpectr(0); }
			return;
		}
	#if 0 //Потрібно тільки при відлагодженні для отримання спектру при обертанні фільтрів
	}else if( str_equal_pgm(p_arg[0], (const uint8_t*)"getFil") ){
					xputs("SF120 spectrogramm filters data table on MOTOR_WL_BLUE\n\f");
					SF120_motor_StepN(((-1)*MOTOR_WL_FULL_PERIOD), MOTOR_MODE_WL);
					SF120_motor_StepN(MOTOR_WL_BLUE, MOTOR_MODE_WL);
					sf120_FIL = 0;
					for(uint16_t i=0; i<(MOTOR_FIL_FULL_PERIOD+100); i++)
					{
						  SF120_motor_StepN(1, MOTOR_MODE_FIL);
						  HAL_Delay(1);
						  uint16_t adcU16 = sf120_getAdc();
						  xprintf("%05d,%05d\n\f",sf120_FIL, adcU16);
						  if(SF120_ReadKey_STOP() ){ break;}
					}
					xputs("-----------------------\n\f");
					return;
				}
	#endif
	}else if( str_equal_pgm(p_arg[0], (const uint8_t*)"setFil") ){
		if(num_args == 2) { sf120_setFilter(p_arg[1][0]-'0'); } else {  sf120_setFilter(ENUM_FIL_ENUMS); }
		//xprintf("fil %d steps\n\f", sf120_FIL);
		SF120_LCD_WL(0);
		return;
	}else if(  str_equal_pgm(p_arg[0], (const uint8_t*)"setWL")){
			if(num_args==2 && p_arg[1][0]>='0' && p_arg[1][0]<='9' )
			{
				int16_t value = atoi08decimal((char*)p_arg[1]);
				//
				if( value > MOTOR_WL_YELLOW_TO_RED ) { sf120_setFilter(ENUM_FIL_RED); } else
					if( value > MOTOR_WL_GREEN_TO_YELLOW ) { sf120_setFilter(ENUM_FIL_YELLOW); } else
						if( value > MOTOR_WL_BLUE_TO_GREEN ) { sf120_setFilter(ENUM_FIL_GREEN); } else
							if( value > MOTOR_WL_PHIOLET_TO_BLUE ) { sf120_setFilter(ENUM_FIL_BLUE); } else
								if( value >= MOTOR_WL_START_HALOGEN ) { sf120_setFilter(ENUM_FIL_PHIOLET); } else
									if( value >= MOTOR_WL_WHITE ) { sf120_setFilter(ENUM_FIL_NONE1); } else
										{sf120_setFilter(ENUM_FIL_DARK);}
				//
				SF120_motor_StepN(((-1)*MOTOR_WL_FINISH_HALOGEN), MOTOR_MODE_WL);//set sf120_WL=0
				SF120_motor_StepN(value, MOTOR_MODE_WL);
				xprintf("wl=%d steps; fil=%d steps\n\f", sf120_WL, sf120_FIL);
				SF120_LCD_WL(0);
				return;
			}

	}
	return;
	}
	xputs("SF120 is not support this command!\n\f");
}
//=================================
//Function table
void (*sys_func[])(uint8_t* p_arg[],uint8_t num_args) = {
    run_Print,
	func_Test,
	run_SpectroFotometrDevice
};

//Command line and alias table

const uint8_t cmd_01[]= {"print"};
const uint8_t cmd_02[]= {"test"};
const uint8_t cmd_03[]= {"sf"};//Spectro Fotometr Device

const uint8_t* sys_func_names[] = {
    cmd_01,
	cmd_02,
	cmd_03
};
//==================================================================================================
//--------------------------------------------------------------------------------------------------
//Треба включити наступні файли в проект. Можна це зробити і тут (тоді їх вміст не буде доступний через пошук)
  #include "/media/odmin/MyBook/STM_proj_from_save220111/lib_github_com/just4fun/AVR_CMD/cmd_exec_v_Argonte/cmd_exec.c"
		#if defined(CMD_EXEC_H)
      #include "/media/odmin/MyBook/STM_proj_from_save220111/lib_github_com/just4fun/AVR_CMD/cmd_exec_v_Argonte/cli_relase.c"
    #endif
//--------------------------------------------------------------------------------------------------
//Треба включити наступний файл в проект. Можна це зробити і тут (тоді його вміст не буде доступний через пошук)
//#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Third_Party/FatFs_R0_12b__elm_chan/ffsample_stm32/xprintf.c"
//#include "/media/odmin/MyBook/embed/stm32/libThird_Party/FatFs_Chan/ff15_ffsample_stm32/xprintf.c"
//--------------------------------------------------------------------------------------------------
//Потрібно відредагувати соурс файл інтерфейсу usbd_cdc_if.c, за зразок беру F:\RAMDISC_T\STM_proj\lib_ST_com\Middlewares_ST\STM32_USB_Device_Library_V2_5_3\Class\CDC\Src\usbd_cdc_xprint_cli.c
//==================================================================================================
