/**
  ******************************************************************************
  * @file    stm32f30x_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_it.h"
//#include "main.h"
//#include "usb_istr.h"
#include "uart.h"
#include "diskio.h"

/** @addtogroup STM32F3-Discovery_Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t UserButtonPressed;
extern __IO uint8_t DataReady;
extern __IO uint32_t USBConnectTimeOut;
__IO uint32_t i =0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	disk_timerproc();
}

extern char test1[];
extern char test2;
extern char indeks;
extern volatile char tryb;
extern volatile char bufor_odbiorczy[100];
extern volatile char licznik;
extern char AT_Wyslij_sms[30];
extern volatile char * komenda;

//przerwanie od USART1
void USART1_IRQHandler(void)
{

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
		{
					USART_SendData(USART1, komenda[indeks++]);

					if(komenda[indeks] == '\0')
						{
							indeks = 0;
							USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
						}
		}
		
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{
			uart_receive(USART_ReceiveData(USART1));
		}
	
}

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles EXTI0_IRQ Handler.
  * @param  None
  * @retval None
  */
//void EXTI0_IRQHandler(void)
//{ 
//
//  if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
//  {
//    /* Delay */
//    for(i=0; i<0x7FFFF; i++);
//    
//    /* Wait for SEL button to be pressed  */
//    while(STM_EVAL_PBGetState(BUTTON_USER) != RESET); 
//    /* Delay */
//    for(i=0; i<0x7FFFF; i++);
//    UserButtonPressed++;
//    
//    if (UserButtonPressed > 0x2)
//    {
//      UserButtonPressed = 0x0;
//    }
//   
//    /* Clear the EXTI line pending bit */
//    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
//  }
//}

//#if defined (USB_INT_DEFAULT)
//void USB_LP_CAN1_RX0_IRQHandler(void)
//#elif defined (USB_INT_REMAP)
//void USB_LP_IRQHandler(void)
//#endif
//{
//   USB_Istr();
//}
//
//#if defined (USB_INT_DEFAULT)
//void USBWakeUp_IRQHandler(void)
//#elif defined (USB_INT_REMAP)
//void USBWakeUp_RMP_IRQHandler(void)
//#endif
//{
//  /* Initiate external resume sequence (1 step) */
//  Resume(RESUME_EXTERNAL);  
//  EXTI_ClearITPendingBit(EXTI_Line18);
//}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

