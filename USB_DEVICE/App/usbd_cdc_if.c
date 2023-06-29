/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v2.0_Cube
  * @brief          : Usb device for Virtual Com Port.
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
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */

#include "xprintf.h"
//#include "/media/odmin/MyBook/STM_proj_from_save220111/lib_Third_Party/FatFs_R0_12b__elm_chan/ffsample_stm32/xprintf.h"
//#include "/media/odmin/MyBook/embed/stm32/libThird_Party/FatFs_Chan/ff15_ffsample_stm32/xprintf.h"


	uint8_t USBD_CDC_Tx( uint8_t  *pbuff, uint16_t length);
	uint8_t FLAG_CDC_ECHO=1; //set 0 to off echo

	#if defined(USE_STM3210E_EVAL) //set def in IDE: STM32F103xG,USE_STM3210E_EVAL
		#define RX_DATA_SIZE  200
		//#define TX_DATA_SIZE  2048
	#elif (defined(USE_STM3210E_EVAL__TO_BLUEPILL) || defined(BOARD__BluePill) )//set def in IDE: STM32F103xE,USE_STM3210E_EVAL__TO_BLUEPILL
		#define RX_DATA_SIZE  200
		//#define TX_DATA_SIZE  2048
	#elif defined(USE_STM32373C_EVAL__TO_BOARD__STM32F373CCT_Minimal_LQFP48)
		#define RX_DATA_SIZE  200
		//#define TX_DATA_SIZE  2048
	#else
		#error
	#endif
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */
#if 0
/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */
#endif

	#ifdef RX_DATA_SIZE
	  uint8_t buffRxData[RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */
	#endif
	#ifdef TX_DATA_SIZE
	  uint8_t buffTxData[TX_DATA_SIZE];/* Received Data over UART (CDC interface) are stored in this buffer */
	#endif
	  //
	#if 0 //uncoment if use USB_CDC UART converter
	  USBD_CDC_LineCodingTypeDef LineCoding = {
		115200,                       /* baud rate */
		0x00,                         /* stop bits-1 */
		0x00,                         /* parity - none */
		0x08                          /* nb. of bits 8 */
	  };
	#endif
/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */
static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
#if 0
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
#endif
static int8_t CDC_Itf_Receive_to_cmdline(uint8_t * data, uint32_t * len);
static int8_t CDC_Itf_Receive_to_xfunc_in(uint8_t * data, uint32_t * len);
static int8_t CDC_Itf_Receive_and_run_CLI(uint8_t * data, uint32_t * len);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
		CDC_Init_FS,
		CDC_DeInit_FS,
		CDC_Control_FS,
		CDC_Itf_Receive_to_cmdline
};

void USBD_CDC_setReceiveFunction(int8_t f)
{
	switch(f)
	{
		case 0:  USBD_Interface_fops_FS.Receive = CDC_Itf_Receive_to_cmdline; break;
		case 1:  USBD_Interface_fops_FS.Receive = CDC_Itf_Receive_to_xfunc_in; break;
		case 2:  USBD_Interface_fops_FS.Receive = CDC_Itf_Receive_and_run_CLI; break;
		default: USBD_Interface_fops_FS.Receive = CDC_Itf_Receive_to_cmdline; break;
	}
}
#if 0
/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
#endif
static int8_t CDC_Init_FS(void)
{
  /* Set Application Buffers */
  //USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  //USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
	#ifdef RX_DATA_SIZE
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, buffRxData);
	#endif
	#ifdef TX_DATA_SIZE
	USBD_CDC_SetTxBuffer(&hUsbDeviceFS, buffTxData, 0);
	#endif
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
	#if 1 //debug
		if(cmd == CDC_SET_CONTROL_LINE_STATE || cmd == CDC_SEND_BREAK || cmd == CDC_SET_LINE_CODING || cmd == CDC_GET_LINE_CODING)
		{
			//
		}else{
			xprintf("in func \"CDC_Control_FS\" arguments:\n\tcmd>%d;\n\tlen=%d\n\tpbuf[]=\"%s\"\n\f", cmd, length, (char*)pbuf);
			//xput_dump (pbuf, 0, length, sizeof(char)); /*xputc('\f');*/
		}
	#endif
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING://length always ==7
		#if 0 //uncoment if use USB_CDC UART converter
			LineCoding.bitrate = (uint32_t) (pbuf[0] | (pbuf[1] << 8) | (pbuf[2] << 16) | (pbuf[3] << 24));
			LineCoding.format = pbuf[4];
			LineCoding.paritytype = pbuf[5];
			LineCoding.datatype = pbuf[6];
			//ComPort_Config();/* Set the new configuration */
		#endif
    break;

    case CDC_GET_LINE_CODING://length always ==7
		#if 0 //uncoment if use USB_CDC UART converter
			pbuf[0] = (uint8_t) (LineCoding.bitrate);
			pbuf[1] = (uint8_t) (LineCoding.bitrate >> 8);
			pbuf[2] = (uint8_t) (LineCoding.bitrate >> 16);
			pbuf[3] = (uint8_t) (LineCoding.bitrate >> 24);
			pbuf[4] = LineCoding.format;
			pbuf[5] = LineCoding.paritytype;
			pbuf[6] = LineCoding.datatype;
		#endif
    break;

    case CDC_SET_CONTROL_LINE_STATE: //length always ==0
    	xputs ("\nToggle DTR\r\n\f");
    break;

    case CDC_SEND_BREAK: //length always ==0
    	xputs ("\nSend Break\r\n\f");//if(press Set_Break){one signal}else if(press Clear_Break){two signal}
    break;

  default:
    break;
  }

  return (USBD_OK);
}
#if 0
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
#endif

/**
  * @brief  CDC_Itf_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
extern uint8_t cmdline[];
static int8_t CDC_Itf_Receive_to_cmdline(uint8_t * data, uint32_t * len)
{
	#if defined(USB_CDC_INDICATE__RxStart)
		USB_CDC_INDICATE__RxStart();
	#endif
	if(FLAG_CDC_ECHO) USBD_CDC_Tx( data, *len );/*echo*/

	static uint8_t count=0;
	static uint8_t dataFirstChar='\0';
	//
	for(uint8_t i=0; i<*len; i++)
	{
			cmdline[count++]=data[i];
			if(data[i]=='\b') { count -= 2;}
	}
	if(cmdline[0] != '\0')
	{
		dataFirstChar = cmdline[0];
		cmdline[0]   = '\0';
	}
	//
	#if 0 //Program Size: Code=14768 RO-data=456 RW-data=524 ZI-data=3492
	uint8_t temp = data[(*len)-1];
	if(temp == '\0' || temp == '\n' || temp == '\r' || temp == 0x03 || temp == 0x0A)
	#else //Program Size: Code=14768 RO-data=456 RW-data=524 ZI-data=3492
	if(data[(*len)-1] == '\0' || data[(*len)-1] == '\n' || data[(*len)-1] == '\r' || data[(*len)-1] == 0x03 ||data[(*len)-1] == 0x0A)
	#endif
	{
		cmdline[0] = dataFirstChar;
		cmdline[count-1] = '\0';
		count=0;
	}

	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, buffRxData);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	#if defined(USB_CDC_INDICATE__RxStop)
		USB_CDC_INDICATE__RxStop();
	#endif
  return (USBD_OK);
}

static int8_t CDC_Itf_Receive_to_xfunc_in(uint8_t * data, uint32_t * len)
{
	#if defined(USB_CDC_INDICATE__RxStart)
		USB_CDC_INDICATE__RxStart();
	#endif
	if(FLAG_CDC_ECHO) USBD_CDC_Tx( data, *len );//echo
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, buffRxData);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	#if defined(USB_CDC_INDICATE__RxStop)
		USB_CDC_INDICATE__RxStop();
	#endif
  return (USBD_OK);
}
static int8_t CDC_Itf_Receive_and_run_CLI(uint8_t * data, uint32_t * len)
{
	#if defined(USB_CDC_INDICATE__RxStart)
		USB_CDC_INDICATE__RxStart();
	#endif
	if(FLAG_CDC_ECHO) USBD_CDC_Tx( data, *len );//echo
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, buffRxData);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	#if defined(USB_CDC_INDICATE__RxStop)
		USB_CDC_INDICATE__RxStop();
	#endif
  return (USBD_OK);
}

/**
  * @}
  */

/**
  * @}
  */
//======================================================

uint8_t USBD_CDC_Tx( uint8_t  *pbuff, uint16_t length)
{
	uint32_t timeout=0;
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)&hUsbDeviceFS.pClassData;
	if(hUsbDeviceFS.dev_old_state != 1){return USBD_FAIL;}
	while(hcdc->TxState != USBD_OK)
	{
		if(timeout++ == 0x1FFFF) { return USBD_BUSY; }
	}
	//if (hcdc->TxState == USBD_OK)
	//{
		USBD_CDC_SetTxBuffer(&hUsbDeviceFS, pbuff, length);//hcdc->TxBuffer = pbuff; hcdc->TxLength = length;//USBD_CDC_SetTxBuffer(&USBD_Device, pbuff, length);
		timeout=0;
    while(USBD_CDC_TransmitPacket(&hUsbDeviceFS) != USBD_OK)
		{
			if(timeout++ == 0x1FFFF) { return USBD_BUSY; }
		}
		return USBD_OK;
  //}else{
		//return USBD_FAIL;
	//}
}

void USBD_CDC_puts(uint8_t* str)
{
	uint8_t len=0;
	while( str[len++] != 0){;};
	USBD_CDC_Tx(str, len-1);
}

uint8_t USBD_CDC_GetTxState(void)
{
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)&hUsbDeviceFS.pClassData;
	return hcdc->TxState;//0-is free
}
uint8_t USBD_CDC_GetRxState(void)
{
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)&hUsbDeviceFS.pClassData;
	return hcdc->RxState;//0-is free
}


//==========================================================================================================
#if 1 //CDC_Transmit_FS або HAL_UART_Transmit_DMA (пакетна передача без циклічних FIFO)
  //-------------------Copy next code in usbd_cdc_if.c------------------------
#define TRANSMITT_BUFFER_SIZE 60 //max_60 //максимальна посилка обмежена USB_MAX_EP0_SIZE==64, мінус кілька службових байт
//#define TRANSMITT_BUFFER(pBuff, size)    HAL_UART_Transmit_DMA(&hlpuart1, pBuff, size)
//#define LOOP_TO_FREE_TRANSMITT_BUFFER()  do{uint32_t timeout=0X00FFFFFF; while(hlpuart1.gState != HAL_UART_STATE_READY && --timeout) { __NOP(); } }while(0)
#define TRANSMITT_BUFFER(pBuff, size)    USBD_CDC_Tx(pBuff, size)
#define LOOP_TO_FREE_TRANSMITT_BUFFER()  do{uint32_t timeout=0X00FFFFFF; while(0 && --timeout) { __NOP(); } }while(0)
#ifdef TRANSMITT_BUFFER_SIZE
    void char_from_xput_to_USBCDC_buff(unsigned char c)
    {
            LOOP_TO_FREE_TRANSMITT_BUFFER();
			#if 1 //Program Size: Code=14768 RO-data=456 RW-data=524 ZI-data=3492
			static char buff_c[TRANSMITT_BUFFER_SIZE]="\0";
				#if (TRANSMITT_BUFFER_SIZE < 0x100)
					static uint8_t buff_i=0;
				#elif (TRANSMITT_BUFFER_SIZE < 0x10000)
					static uint16_t buff_i=0;
				#else
					static uint32_t buff_i=0;
				#endif

			switch (c)
			{
				case '\0'://'\0' == 0x00 == NULL
				case 0x03://0x03 == ETX End Of Text
				case '\f'://'\f' == 0x0C == FORM FEED
					buff_c[buff_i]='\0';
				  TRANSMITT_BUFFER((uint8_t*)buff_c, buff_i);
					buff_i=0;
					break;
				case '\b'://'\b' == 0x08 == BACKSPACE
					if(buff_i != 0)buff_i--;
				  break;
				default: //if (c != '\0')
					buff_c[buff_i++]=c;
				  if(buff_i == TRANSMITT_BUFFER_SIZE)
					{
						TRANSMITT_BUFFER((uint8_t*)buff_c, buff_i);
						buff_i=0;
					}
				}//end switch
			#else //Program Size: Code=14780 RO-data=456 RW-data=524 ZI-data=3500
			#if (TRANSMITT_BUFFER_SIZE < 0x100)
			//struct buff_struct{
			//	char c[TRANSMITT_BUFFER_SIZE];
			//	uint8_t i;
			//};
			typedef struct {
				char c[TRANSMITT_BUFFER_SIZE];
				uint8_t i;} buff_t;
	    #elif (TRANSMITT_BUFFER_SIZE<0x10000)
			struct buff_struct{
				char c[TRANSMITT_BUFFER_SIZE];
				uint16_t i;
			};
	    #else
			struct buff_struct{
				char c[TRANSMITT_BUFFER_SIZE];
				uint32_t i;
			};
			#endif
			//static struct buff_struct buff = {"\0", 0};
			static buff_t buff = {"\0", 0};
			switch (c)
			{
				case '\0'://'\0' == 0x00 == NULL
				case 0x03://0x03 == ETX End Of Text
				case '\f'://'\f' == 0x0C == FORM FEED
					buff.c[buff.i]='\0';
				  TRANSMITT_BUFFER((uint8_t*)buff.c, buff.i);
					buff.i=0;
					break;
				case '\b'://'\b' == 0x08 == BACKSPACE
					if(buff.i != 0)buff.i--;
				  break;
				default: //if (c != '\0')
					buff.c[buff.i++]=c;
				  if(buff.i == TRANSMITT_BUFFER_SIZE)
					{
						TRANSMITT_BUFFER((uint8_t*)buff.c, buff.i);
						buff.i=0;
					}
				}//end switch
			#endif
    }
  #endif
  #undef LOOP_TO_FREE_TRANSMITT_BUFFER
  #undef TRANSMITT_BUFFER
  #undef TRANSMITT_BUFFER_SIZE
	//------------------------End Copy code in usbd_cdc_if.c------------------------
#endif //end

#if 0 //next code copy to main
		//main.c
		extern void char_from_xput_to_USBCDC_buff(unsigned char c);
		void main(void)
		{
			//.............................................................................
			xfunc_out = char_from_xput_to_USBCDC_buff;
			HAL_Delay(500);//інколи без паузи після інніціювання заходить в HardFault_Handler()
			xputs ((const char*) "Dis is a string \"xputs\"\n\f");
			uint16_t buff[10]={1,2,3,4,5,6,7,8,9,0};
			xputs("ADC dump\n"); for(uint8_t i=0; i<10; i++) { xput_dump (buff+i, i, 1, DW_DECIMAL16); }; xputc('\f');
			//.............................................................................
		}
#endif //END example
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
