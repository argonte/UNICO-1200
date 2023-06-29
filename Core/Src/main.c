/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


extern USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_StatusTypeDef  USBD_Stop(USBD_HandleTypeDef *pdev);
//

extern void char_from_xput_to_USBCDC_buff(unsigned char c);
extern void cmd_process_loop(void);
extern uint8_t cmdline[];//use in cli_relase.c
//

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#if 0
static inline unsigned long ITM_SendChar (unsigned long ch)
{
  if (((ITM->TCR & ITM_TCR_ITMENA_Msk) != 0UL) &&      /* ITM enabled */
      ((ITM->TER & 1UL               ) != 0UL)   )     /* ITM Port #0 enabled */
  {
    while (ITM->PORT[0U].u32 == 0UL)
    {
      __asm("nop");
    }
    ITM->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}

int _write(int file, char *ptr, int len)
{
    //return usart_write(platform_get_console(), (u8 *)ptr, len);
      int i=0;
      for(i=0 ; i<len ; i++)
        ITM_SendChar((*ptr++));
      return len;
}
#endif
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

IWDG_HandleTypeDef hiwdg;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */
uint8_t pDataUART1rx[100];
void LED_Blink_SOS(uint8_t count);

//int16_t  motorStepCounter[2]={0,0};
int16_t sf120_WL =0;
int16_t sf120_WL_keyUpOrDown =0;
int16_t sf120_FIL=0;
uint8_t str_buff[32];
uint8_t sf120_mode=0;
uint16_t adcVal[3]={0,0,0}; //Dark; I0; It

union ramArray_u ramArray;
union ramLCD_u ramLCD;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_IWDG_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	xfunc_output  = 0;
	//
	u16_adc__startWL = MOTOR_WL_START_HALOGEN;
	u16_adc__finishWL = MOTOR_WL_FINISH_HALOGEN;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  //MX_USB_DEVICE_Init();
  MX_ADC1_Init();
  MX_IWDG_Init();
  MX_SPI1_Init();
//  MX_SPI2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	#if defined(AD7705_USE_SOFTSPI)
  	  	  //
	#else
		#if defined(USE_EXTERNAL_AD7705_MODULE_CONNECT_TO_SPI2_NOREMAP)
  	  	  MX_SPI2_Init();
		#elif defined(USE_INTERNAL_AD7705_CONNECT_TO_SPI1_NOREMAP)
			//
		#else
			#error
		#endif
	#endif
  HC595_OE_MOTOR_OFF();
  HAL_UART_Receive_IT(&huart1, (uint8_t*)pDataUART1rx, 1);
  //ITM_SendChar ('D');
//  __HAL_IWDG_START(&hiwdg);
//
#if 0
  HAL_Delay(500);//Must be pause for connect to PC
  //if ( 0 ){
  if ( hUsbDeviceFS.dev_old_state == 1 ){
	  xputs ((const char*) "SF-120 \"USB CDC connect\"\n\f");
	  //uint16_t buff[10]={1,2,3,4,5,6,7,8,9,0};
	  //xputs("ADC dump\n"); for(uint8_t i=0; i<10; i++) { xput_dump (buff+i, i, 1, DW_DECIMAL16); }; xputc('\f');
  }
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  LED_OFF();
  //
  lcd_Init();
  lcd_Clear();
	#if 0 //dedug SF120_LCD_print_Dig
	  SF120_LCD_print_Dig(12345, 0);HAL_Delay(2000);
	  SF120_LCD_print_Dig(54321, 0);HAL_Delay(2000);
	  SF120_LCD_print_Dig(10, 0);HAL_Delay(2000);
	  SF120_LCD_print_Dig(100, 0);HAL_Delay(2000);
	  SF120_LCD_print_Dig(1, 0);HAL_Delay(2000);
	  SF120_LCD_print_DigFloat(123456789, 0);HAL_Delay(2000);
	  SF120_LCD_print_DigFloat(876543210, 1);HAL_Delay(2000);
	  SF120_LCD_print_DigFloat(FLOAT_MAXIMAL, 0);HAL_Delay(2000);
	  SF120_LCD_print_DigFloat(FLOAT_MAXIMAL, 1);HAL_Delay(2000);
	#endif
	#if 0 //debug LCD print Terminal8x14
	  LCD_setPrint( 0,2,Terminal8x14, 0); LCD_puts((uint8_t*)"012345");
	  LCD_setPrint( 0,4,Terminal8x14, 1); LCD_puts((uint8_t*)"987654");
	  HAL_Delay(10000);
	#endif
	#if 0 //debug LCD print font_3x4
	  LCD_setPrint( 0,7,font_3x4, (4<<1)|0); LCD_puts((uint8_t*)"  0123456789");
	  LCD_setPrint(50,7,font_3x4, (4<<1)|1); LCD_puts((uint8_t*)"9876543210  ");
	  HAL_Delay(10000);
	#endif
  LCD_setPrint( 0,0,Terminal8x14, 1); LCD_puts((uint8_t*)" Start SF-120 ");
  LCD_setPrint( 0,3,font_5x7,0); LCD_puts((uint8_t*)"  I. HORBACHEVSKY    ");
  LCD_setPrint( 0,4,font_5x7,0); LCD_puts((uint8_t*)"  TERNOPIL NATIONAL  ");
  LCD_setPrint( 0,5,font_5x7,0); LCD_puts((uint8_t*)" MEDICAL UNIVERSITY  ");
  LCD_setPrint( 0,7,font_5x7,0); LCD_puts((uint8_t*)"dep General Chemisrty");
  #if defined(AD7705_H)
    //xsprintf((char*)cmdline, "sf init ADC 7 %c0", (AD7705_CH_SF120+'A') );cmd_process_loop();
	ad7705ResetSoftSync();
	ad7705ClockSetupSync(AD7705_MASTERCLK_MHZ49152, AD7705_UPDATE_HZ50, AD7705_CH_SF120);
	ad7705SetupAndCalibrateSync(0, AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_UNIPOLAR/*AD7705_SETUP_BIPOLAR*/, AD7705_CH_SF120);
  #endif
  xsprintf((char*)cmdline, "sf init ADC 1 A");cmd_process_loop();
  xsprintf((char*)cmdline, "sf setFil 3");cmd_process_loop();
  lcd_Clear();
  LCD_Line_H(0, 6, 128, 0x01<<7);//0b10000000 is Down line
  LCD_setPrint(0,7,font_5x7,1); LCD_puts((uint8_t*)" Please select MODE! ");
//  LCD_putBitmap(62, 2, 67, font_5x7+(('A'-32)*5), 20, 1);//OK
#if 0
    LCD_putBitmap( 0+18*0, 2,  0+18*1, Consolas18x24+(('0'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    LCD_putBitmap(19+18*0, 2, 19+18*1, Consolas18x24+(('1'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    LCD_putBitmap(20+18*1, 2, 20+18*2, Consolas18x24+(('2'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    LCD_putBitmap(21+18*2, 2, 21+18*3, Consolas18x24+(('3'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    LCD_putBitmap(22+18*3, 2, 22+18*4, Consolas18x24+(('4'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    LCD_putBitmap(23+18*4, 2, 23+18*5, Consolas18x24+(('5'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
 //   LCD_putBitmap(24+18*5, 2, 24+18*6, Consolas18x24+(('6'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
#elif 0
    LCD_putBitmap(19+18*0, 2, 19+18*1, Consolas18x24+(('0'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    LCD_putBitmap(20+18*1, 2, 20+18*2, Consolas18x24+(('9'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    LCD_putBitmap(21+18*2, 2, 21+18*3, Consolas18x24+(('8'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    //LCD_putBitmap(22+18*3, 2, 22+18*4, Consolas18x24+(('4'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
    //LCD_putBitmap(23+18*4, 2, 23+18*5, Consolas18x24+(('5'-'0'+3)*(18*24)/8), (18*24)/8, 0);//OK
#elif 1

#endif

//  LCD_putBytes(50, 0, font5x7['0'-32], 128, 0);
//  LCD_putBytes(50, 0, font5x7['0'-32], 8, 0);
//  LCD_putBytes(0, 0, font5x7['H'-32], 5, 0);
//  LCD_putBytes(55, 4, font5x7['0'-32], 400);
//  LCD_putBytes(0, 0, font5x7[0], 64*8*2);
//  LCD_setPrint(20, 2, font5x7[0], 0);
  //LCD_putc('H');
  //LCD_setPrint(0,0,font_5x7,0);
  //LCD_puts((uint8_t*)"WL = 123.5 nm  ");

  //
  //HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"Start SF120\r\n", 13);
  debug_str( "Start SF120\r\n" );
//  xprintf("Start SF%d\n\f", 120);
//  SF120_keyTx(0);

  while (1)
  {
	  //ITM_SendChar ('#');
	  cmd_process_loop();
	  sf120_process_scanKey();
	  HAL_Delay(25);
	  if(pDataUART1rx[0] == '\0' && pDataUART1rx[1] == 'U')
	  {
		  switch(pDataUART1rx[2]){
			  case 'u': MX_USB_DEVICE_Init(); break;
			  case 'A': SF120_LCD_print_Dig(ADC7705_ReadData(1), 0); break;
			  default: break;
		  }
		  pDataUART1rx[1] = '\0';
	  }
//	  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"HAL_UART_Transmit_DMA huart1\r\n", 30);
//	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//	  LED_Blink_SOS(5);



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  HAL_IWDG_Refresh(&hiwdg);//Цього достатньо щоб собака мовчала
  }//END while(1){...}
}//END main(){...}
//======================================================================

//----------------------------------------------------------------------
static uint8_t flag_UART_TxCpltCallback =0;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		flag_UART_TxCpltCallback =0;//
	}
}
static inline uint8_t str_len(const uint8_t* str)
{
	uint8_t l=0;
	while (str[l]!=0) l++;
	return l;
}
void debug_str(char* str)
{
	while(flag_UART_TxCpltCallback ){ __NOP(); }
	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)str, str_len((uint8_t *)str) );
	flag_UART_TxCpltCallback =1;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		if(pDataUART1rx[0]=='U'){//USB
			//getUsbConnect:
			  if ( hUsbDeviceFS.dev_old_state == 1 ){
				  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"SF-120 \"USB CDC connect\"\r\n", 26);
			  }else{
				  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"SF-120 \"USB CDC disconnect\"\r\n", 29);
			  }
		}else if(pDataUART1rx[0]=='u'){//USB connect
			pDataUART1rx[1] = 'U';
			pDataUART1rx[2] = pDataUART1rx[0];//MX_USB_DEVICE_Init();
			//goto getUsbConnect;
		}else if(pDataUART1rx[0]=='L'){//LED
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}else if(pDataUART1rx[0]=='R'){//RESET
			NVIC_SystemReset();
		}else if(pDataUART1rx[0]=='A'){//ADC
			pDataUART1rx[1] = 'U';
			pDataUART1rx[2] = pDataUART1rx[0];//SF120_LCD_print_Dig(ADC7705_ReadData(1), 0);
		}else if(pDataUART1rx[0]=='0'){
			//SF120_KEY_Tx(0);
		}else if(pDataUART1rx[0]=='1'){
			//SF120_KEY_Tx(1<<7);
		}else if(pDataUART1rx[0]=='2'){
			//SF120_KEY_Tx(1<<6);
		}else if(pDataUART1rx[0]=='3'){
			//SF120_KEY_Tx(1<<5);
		}else if(pDataUART1rx[0]=='4'){
			//SF120_KEY_Tx(1<<4);
		}else if(pDataUART1rx[0]=='5'){
			//SF120_KEY_Tx(1<<3);
		}else if(pDataUART1rx[0]=='6'){
			//SF120_KEY_Tx(1<<3);
		}else if(pDataUART1rx[0]=='7'){
			//SF120_KEY_Tx(1<<3);
		}else if(pDataUART1rx[0]=='8'){
			//SF120_KEY_Tx(0xFF);
		}
		pDataUART1rx[0]='\0';
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"HAL_UART_Transmit_DMA huart1\r\n", 30);
		HAL_UART_Receive_IT(&huart1, (uint8_t*)pDataUART1rx, 1);
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */
#if 0
  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
#endif
  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */
#if 0
  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Reload = 1000;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */
  __HAL_IWDG_START(&hiwdg);
#endif
  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */
#if 0
  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */
#else
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) { Error_Handler(); }
#endif
  __HAL_SPI_ENABLE(&hspi1);
  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */
#if 0
  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
#elif 1
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	#if defined(AD7705_USE_SPI16)
	  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
	#else
	  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	#endif
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; //from 2 to 256
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK) { Error_Handler(); }
#endif
  //
  __HAL_SPI_ENABLE(&hspi2);
  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HC595_OE_MOTOR_GPIO_Port, HC595_OE_MOTOR_Pin|HC595_Latch_MOTOR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : NotUsed_Pin */
  GPIO_InitStruct.Pin = NotUsed_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(NotUsed_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : HC595_OE_MOTOR_Pin HC595_Latch_MOTOR_Pin */
  GPIO_InitStruct.Pin = HC595_OE_MOTOR_Pin|HC595_Latch_MOTOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HC595_OE_MOTOR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_IN_Pin */
  GPIO_InitStruct.Pin = BOOT1_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BOOT1_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SF120_KeyR3_Pin SF120_KeyR2_Pin SF120_KeyR1_Pin */
  GPIO_InitStruct.Pin = SF120_KeyR3_Pin|SF120_KeyR2_Pin|SF120_KeyR1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SF120_KeyR1_GPIO_Port, &GPIO_InitStruct);

//}

/* USER CODE BEGIN 4 */
//#error "delete previos {"

	#if 1 //USB_EN_pin   mov from Target/usbd_conf.c
		  //HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);//USB_DM and USB_DP is default function for dis pins.
		  //GPIO_InitTypeDef GPIO_InitStruct = {0};
		  //USBEN_GPIO_Port__HAL_RCC_GPIOn_CLK_ENABLE();
		  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
		  //GPIO_InitStruct.Pull  = GPIO_NOPULL;
		  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;//GPIO_SPEED_FREQ_MEDIUM;//GPIO_SPEED_FREQ_HIGH;
		  GPIO_InitStruct.Pin   = GPIO_PIN_15;
		  HAL_GPIO_WritePin(GPIOA, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
		  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	#endif


	#if 0 //(SF120_KeyR1_GPIO_Port == SF120_KeyR2_GPIO_Port && SF120_KeyR2_GPIO_Port == SF120_KeyR3_GPIO_Port )
	  /*Configure GPIO pins : SF120_KeyR3_Pin SF120_KeyR2_Pin SF120_KeyR1_Pin */
	  GPIO_InitStruct.Pin = SF120_KeyR3_Pin|SF120_KeyR2_Pin|SF120_KeyR1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	  HAL_GPIO_Init(SF120_KeyR3_GPIO_Port, &GPIO_InitStruct);
	#endif
#if ( defined(HC595_STEPMOTOR_NSS_GPIO_Port) && defined(HC595_STEPMOTOR_OE_GPIO_Port) \
   && defined(HC595_STEPMOTOR_NSS_Pin)       && defined(HC595_STEPMOTOR_OE_Pin) )

  HAL_GPIO_WritePin(HC595_STEPMOTOR_NSS_GPIO_Port, HC595_STEPMOTOR_NSS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(HC595_STEPMOTOR_OE_GPIO_Port, HC595_STEPMOTOR_OE_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pin = HC595_STEPMOTOR_NSS_Pin;
  HAL_GPIO_Init(HC595_STEPMOTOR_NSS_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = HC595_STEPMOTOR_OE_Pin;
  HAL_GPIO_Init(HC595_STEPMOTOR_OE_GPIO_Port, &GPIO_InitStruct);
#endif

#if 1 //defined( HC595_LCD_E )//( defined(HC595_LCD_E_GPIO_Port) && defined(HC595_LCD_E_Pin) )
  /*This pin is /CS, is pin connect to LCD_HC595Latch and between connect LCD_E*/
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  //GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  HAL_GPIO_WritePin(GPIOA, GPIO_InitStruct.Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif


#if defined(AD7705_USE_SOFTSPI)
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	//GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
	HAL_GPIO_WritePin(GPIOB, GPIO_InitStruct.Pin, GPIO_PIN_SET);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN; //GPIO_PULLUP; //NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	//HAL_GPIO_WritePin(PIOB, GPIO_InitStruct.Pin, GPIO_PIN_SET);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	//ADC7705_Init();
	//ADC7705_Disable();
	//uint16_t ADC7705_ReadData(uint8_t channel);
#else
	#if defined(USE_EXTERNAL_AD7705_MODULE_CONNECT_TO_SPI2_NOREMAP)
		#if defined(ADC7705_CLK)
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				//GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				GPIO_InitStruct.Pin = GPIO_PIN_12;
				HAL_GPIO_WritePin(GPIOB, GPIO_InitStruct.Pin, GPIO_PIN_SET);
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		#endif
		#if defined(ADC7705_RESET)
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				//GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				GPIO_InitStruct.Pin = GPIO_PIN_11;
				HAL_GPIO_WritePin(GPIOB, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		#endif
		#if defined(ADC7705_DRDY)
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				GPIO_InitStruct.Pull = GPIO_PULLUP; //GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				GPIO_InitStruct.Pin = GPIO_PIN_10;
				//HAL_GPIO_WritePin(GPIOB, GPIO_InitStruct.Pin, GPIO_PIN_SET);
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		#endif
	#elif defined(USE_INTERNAL_AD7705_CONNECT_TO_SPI1_NOREMAP)
		#if defined(ADC7705_CS)
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				//GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				GPIO_InitStruct.Pin = GPIO_PIN_3;
				HAL_GPIO_WritePin(GPIOA, GPIO_InitStruct.Pin, GPIO_PIN_SET);
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		#else
			#error "ADC7705_CS pin must be def"
		#endif
		#if defined(ADC7705_RESET)
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				//GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				GPIO_InitStruct.Pin = GPIO_PIN_11;
				HAL_GPIO_WritePin(GPIOB, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		#endif
		#if defined(ADC7705_DRDY)
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				GPIO_InitStruct.Pull = GPIO_PULLDOWN; //GPIO_PULLUP; //GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				GPIO_InitStruct.Pin = GPIO_PIN_10;
				//HAL_GPIO_WritePin(GPIOB, GPIO_InitStruct.Pin, GPIO_PIN_SET);
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		#endif
	#else
		#error
	#endif
#endif

}
static void delay_NOP(uint32_t count)
{
	uint32_t temp = count;
	#define NOP_COUNT_DELAY_1ms 4000
	while(temp--){ for(uint32_t i = 0; i<NOP_COUNT_DELAY_1ms; i++) { __NOP(); } }
	#undef NOP_COUNT_DELAY_1ms
}
void LED_Blink_SOS(uint8_t count)
{
	uint8_t temp = count;
	//LED_OFF();
	while(temp--)
	{
		//SOS (СОС) — международный сигнал бедствия в радиотелеграфной (с использованием азбуки Морзе) связи.
		//Сигнал представляет собой последовательность «три точки — три тире — три точки», передаваемую без каких-либо межбуквенных интервалов
		uint8_t i=0;
		for(i=0 ; i<3; i++) { LED_ON(); delay_NOP(300); LED_OFF(); delay_NOP(100);}//...
		for(i=0 ; i<3; i++) { LED_ON(); delay_NOP(800); LED_OFF(); delay_NOP(100);}//---
		for(i=0 ; i<3; i++) { LED_ON(); delay_NOP(300); LED_OFF(); delay_NOP(100);}//...
		delay_NOP(6000);
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"SOS\tError_Handler\r\n", 19);
  __disable_irq();
  while (1)
  {
	  LED_Blink_SOS(10);
	  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"SOS\tError_Handler\r\n", 19);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
