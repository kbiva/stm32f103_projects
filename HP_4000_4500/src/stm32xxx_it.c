/**
  ******************************************************************************
  * @file    stm32xxx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    11-October-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
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
#include <stdint.h>
#include "stm32f10x_conf.h"
#include "hd66710.h"

extern volatile uint32_t millis;
extern volatile uint16_t inputValues;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

// Button4
void EXTI2_IRQHandler(void)
{
  //Check if EXTI_Line2 is asserted
  if(EXTI_GetITStatus(EXTI_Line2) != RESET) {

  }
  //we need to clear line pending bit manually
  EXTI_ClearITPendingBit(EXTI_Line2);
}

// Button3
void EXTI3_IRQHandler(void)
{
  //Check if EXTI_Line3 is asserted
  if(EXTI_GetITStatus(EXTI_Line3) != RESET) {

  }
  //we need to clear line pending bit manually
  EXTI_ClearITPendingBit(EXTI_Line3);
}

// Button2
void EXTI4_IRQHandler(void)
{
  //Check if EXTI_Line3 is asserted
  if(EXTI_GetITStatus(EXTI_Line4) != RESET) {

  }
  //we need to clear line pending bit manually
  EXTI_ClearITPendingBit(EXTI_Line4);
}

// Button1
void EXTI9_5_IRQHandler(void)
{
  //Check if EXTI_Line3 is asserted
  if(EXTI_GetITStatus(EXTI_Line5) != RESET) {

  }
  //we need to clear line pending bit manually
  EXTI_ClearITPendingBit(EXTI_Line5);
}

void SPI1_IRQHandler(void) {

  if(SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE)==SET) {
    inputValues = SPI_I2S_ReceiveData(SPI1);
  }
}


/******************************************************************************/
/*             Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */


void SysTick_Handler(void)
{
  millis++;
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
  {}
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
  {}
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
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
