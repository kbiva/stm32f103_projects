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
#include "PCF8833.h"
#include "rotating_cube_checkboard.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

volatile uint8_t framebuffer_transfer_complete;
volatile uint8_t framebuffer_clear_complete;
uint8_t active_framebuffer;
extern __align(4) uint8_t framebuffer1[]; // 19602 (17424)
extern __align(4) uint8_t framebuffer2[]; // 19602

void EXTI2_IRQHandler(void)
{
  //Check if EXTI_Line2 is asserted
  if(EXTI_GetITStatus(EXTI_Line2) != RESET) {
     PCF8833_Sleep();
  }
  //we need to clear line pending bit manually
  EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI3_IRQHandler(void)
{
  //Check if EXTI_Line3 is asserted
  if(EXTI_GetITStatus(EXTI_Line3) != RESET) {
    PCF8833_Wakeup();
  }
  //we need to clear line pending bit manually
  EXTI_ClearITPendingBit(EXTI_Line3);
}

void DMA1_Channel3_IRQHandler(void)
{
  if(DMA1->ISR&DMA1_IT_TC3)  //if(DMA_GetITStatus(DMA1_IT_TC3))
  {
    //framebuffer transfer completed
    DMA1->IFCR = DMA1_IT_TC3;//DMA_ClearITPendingBit(DMA1_IT_TC3);
    DMA1_Channel3->CCR &= ~DMA_IT_TC; //DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, DISABLE);
    DMA1_Channel3->CCR &= (uint16_t)~DMA_CCR1_EN; //DMA_Cmd(DMA1_Channel3, DISABLE);
    SPI1->CR2 &= (uint16_t)~SPI_I2S_DMAReq_Tx; //SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);

    framebuffer_transfer_complete=1;

    // Init DMA1 Channel6 to clear frame buffer
    DMA1_Channel6->CNDTR = MAX_X*MAX_Y*9/32; //DMA_SetCurrDataCounter(DMA1_Channel6,MAX_X*MAX_Y*9/32);

    if(active_framebuffer) {
      DMA1_Channel6->CPAR =(uint32_t)framebuffer1;
    } else {
      DMA1_Channel6->CPAR =(uint32_t)framebuffer2;
    }
    DMA1_Channel6->CCR |= DMA_IT_TC;// DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
    DMA1_Channel6->CCR |= DMA_CCR1_EN; //DMA_Cmd(DMA1_Channel6, ENABLE);

  }
}

void DMA1_Channel6_IRQHandler(void)
{
  if(DMA1->ISR&DMA1_IT_TC6)  //if(DMA_GetITStatus(DMA1_IT_TC6))
  {
    //framebuffer clear completed
    DMA1->IFCR = DMA1_IT_TC6; //DMA_ClearITPendingBit(DMA1_IT_TC6);
    DMA1_Channel6->CCR &= ~DMA_IT_TC; //DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, DISABLE);
    DMA1_Channel6->CCR &= (uint16_t)~DMA_CCR1_EN; //DMA_Cmd(DMA1_Channel6, DISABLE);

    framebuffer_clear_complete=1;
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

}

/**
  * @brief  This function handles SDIO global interrupt request.
  * @param  None
  * @retval None
  */
void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */

}

void DMA2_Channel4_5_IRQHandler(void)
{

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
