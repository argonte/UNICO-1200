/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v2.0_Cube
  * @brief          : This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */
#include "xprintf.h"
#if defined(XPRINTF_DEF)
	#if (XPRINTF_DEF == 2021)
		extern void (*xfunc_output)(unsigned char);	/* Pointer to the output stream if use XPRINTF_DEF 2021*/
	#else
		#error
	#endif
#elif defined(_STRFUNC_ChaN)
	#if (_STRFUNC_ChaN == 2011)
		extern void (*xfunc_out)(unsigned char);	/* Pointer to the output stream if use XPRINTF_DEF 2011*/
	#else
		#error
	#endif
#else
	#error "Please in main.h file #include "/media/odmin/MyBook/embed/stm32/libThird_Party/FatFs_Chan/ff15_ffsample_stm32/xprintf.h""
#endif

extern void char_from_xput_to_USBCDC_buff(unsigned char c);
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */
	#if 0
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pin   = GPIO_PIN_11;//USB_DM
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  HAL_GPIO_Init( GPIOA, &GPIO_InitStruct);
	  GPIO_InitStruct.Pin   = /*GPIO_PIN_15|*/GPIO_PIN_12;//USB_EN | USB_DP
	  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	  HAL_GPIO_Init( GPIOA, &GPIO_InitStruct);
	  HAL_Delay(100);//Must be pause for stabilized signals
	#if 0//Досліджую яким чином визначити чи підєднаний пристрій до компютера
	  for(uint64_t i=0; i<0xFFFFFFFF; i++)
	  {
		  //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8));//при витягнутому(на флоаті) кабелі мерегтить, при вставленому світить. При пулдовн постійно світить. При пулуп постійно погашений, була невдала спроба законектитись
		  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11));//при витягнутому(на флоаті) кабелі мерегтить, при вставленому світить. При пулдовн постійно світить. При пулуп правильно реагує на витягування кабеля
		  //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12));//при витягнутому(на флоаті) кабелі мерегтить, при вставленому світить. При пулдовн постійно світить. Не провіряв, думаю аналогічно до Pin8, бо це його підтяжка
	  }
	#endif
	  if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET)
	  {
		  return;
	  }else{
		  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);//USB is default function for dis pins.
	  }
	#endif
  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
  HAL_Delay(500);//Must be pause for connect to PC
	//  if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET ) {LED_ON();}
	#if 0 //Досліджую яким чином можна проконтролювати успішний конект пристрою до компютера
	  uint32_t temp=0;
	  temp = hUsbDeviceFS.dev_state;//if (USB cable ==OFF || ON){return 1;}
	  HAL_Delay(temp);
	  temp = hUsbDeviceFS.dev_connection_status;//if (USB cable ==OFF || ON){return 0;}
	  HAL_Delay(temp);
	  temp = hUsbDeviceFS.dev_config_status;//if (USB cable ==OFF || ON){return 0;}
	  HAL_Delay(temp);
	  temp = hUsbDeviceFS.dev_old_state;//if (USB cable ==OFF){return 0;}else if (USB cable ==ON){return 1;}
	  HAL_Delay(temp);
	  temp = hUsbDeviceFS.ep0_state;//if (USB cable ==OFF || ON){return 0;}
	  HAL_Delay(temp);
	#endif

	#if 1
	  //if ( 0 ){
	  if ( hUsbDeviceFS.dev_old_state == 1 )
	  {
		#if defined(XPRINTF_DEF)
			#if (XPRINTF_DEF == 2021)
				  xfunc_output = char_from_xput_to_USBCDC_buff;
			#else
				#error
			#endif
		#elif defined(_STRFUNC_ChaN)
			#if (_STRFUNC_ChaN == 2011)
				  xfunc_out  = char_from_xput_to_USBCDC_buff;
			#else
				#error
			#endif
		#else
			#error
		#endif

	  }else{
		  USBD_Stop(&hUsbDeviceFS);
		  xfunc_output  = 0;
//		  HAL_Delay(500);//Must be pause for disconnect from PC
//		  if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_RESET ) {LED_OFF();}
	  }
//		  xputs ((const char*) "Dis is a string \"xputs\"\n\f");
//		  uint16_t buff[10]={1,2,3,4,5,6,7,8,9,0};
//		  xputs("ADC dump\n"); for(uint8_t i=0; i<10; i++) { xput_dump (buff+i, i, 1, DW_DECIMAL16); }; xputc('\f');
	#endif
  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
