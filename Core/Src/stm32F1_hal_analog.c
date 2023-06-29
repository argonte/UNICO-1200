#include "stm32F1_hal_analog.h"

extern DMA_HandleTypeDef hdma_adc1;

//#define debug_Error_Handler() {;}
extern void debug_str(char* str);
//#define debug_Error_Handler() do{ debug_str("\r\nError\r\n"); debug_str(__FILE__); /*debug_str(__LINE__);*/ }while(0)
extern void xsprintf (char* buff, const char*	fmt, ...);
#define debug_Error_Handler() do{char str[100]; xsprintf(str, "\r\nError\r\n%s (%d)\r\n", __FILE__, __LINE__); debug_str(str); }while(0)
//============================================================================================================
uint16_t sf120_getAdc(ADC_HandleTypeDef* hadc)
{
#if 0 //
	if (HAL_ADC_Start(hadc) != HAL_OK) { debug_Error_Handler(); } // запускаем преобразование сигнала АЦП
	if (HAL_ADC_PollForConversion(hadc, 100) != HAL_OK) { debug_Error_Handler(); } // ожидаем окончания преобразования
	uint16_t adcU16 = HAL_ADC_GetValue(hadc); // читаем полученное значение в переменную adc
	if (HAL_ADC_Stop(hadc) != HAL_OK) { debug_Error_Handler(); } // останавливаем АЦП (не обязательно)
	return adcU16;
#elif 1 //
	uint32_t adcU32=0;
	for(uint8_t i=0; i<16; i++)
	{
		if (HAL_ADC_Start(hadc) != HAL_OK) { debug_Error_Handler(); } // запускаем преобразование сигнала АЦП
		if (HAL_ADC_PollForConversion(hadc, 100) != HAL_OK) { debug_Error_Handler(); } // ожидаем окончания преобразования
		adcU32 += HAL_ADC_GetValue(hadc); // читаем полученное значение в переменную adc
		if (HAL_ADC_Stop(hadc) != HAL_OK) { debug_Error_Handler(); } // останавливаем АЦП (не обязательно)
	}
	return adcU32/16;
#endif
}
//==============================================================================================================
void ADC_ConfigChannel(ADC_HandleTypeDef* hadc, uint32_t Channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	/** Configure Regular Channel */
	sConfig.Channel = Channel;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) { debug_Error_Handler(); }
	HAL_ADCEx_Calibration_Start(hadc);
}
void ADC_Init(ADC_HandleTypeDef* hadc)
{
	//hadc.Instance = ADC1;
	if(hadc->Instance != ADC1 && hadc->Instance != ADC2) { debug_Error_Handler(); } else {
		hadc->Init.ScanConvMode = ADC_SCAN_DISABLE;
		hadc->Init.ContinuousConvMode = DISABLE;
		hadc->Init.DiscontinuousConvMode = DISABLE;
		hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
		hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
		hadc->Init.NbrOfConversion = 1;
		if (HAL_ADC_Init(hadc) != HAL_OK) { debug_Error_Handler(); }
		//ADC_ConfigChannel(hadc, ADC_CHANNEL_0);
	}
}
//=======================================================================================================================
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hadc->Instance==ADC1)
	{
		__HAL_RCC_ADC1_CLK_ENABLE();/* Peripheral clock enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC1 GPIO Configuration: PA0-WKUP ---> ADC1_IN0; PA1 ---> ADC1_IN1 */
		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		#if 0 /* ADC1 DMA Init */ /* ADC1 Init */
			hdma_adc1.Instance = DMA1_Channel1;
			hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
			hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
			hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
			hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
			hdma_adc1.Init.Mode = DMA_NORMAL;
			hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
			if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) { debug_Error_Handler(); }
			__HAL_RCC_DMA1_CLK_ENABLE();
			HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);		  /* DMA1_Channel1_IRQn interrupt configuration */
			HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
			__HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
		#endif
	}
}
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_DISABLE();/* Peripheral clock disable */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1); /**ADC1 GPIO Configuration PA0-WKUP     ------> ADC1_IN0;  PA1     ------> ADC1_IN1   */
    //HAL_DMA_DeInit(hadc->DMA_Handle);/* ADC1 DMA DeInit */
  }
}
//=======================================================================================================================
void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_adc1);
}
