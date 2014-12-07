/*
 *  ir_decode.c
 *
 *  Author: Kestutis Bivainis
 */

#include <stdio.h>
#include "stm32f10x_conf.h"
#include "ir_decode.h"
#include "delay.h"
#include "queue.h"
#include "nec_decode.h"
#include "mp6090r.h"

volatile static uint16_t captCount=0;
volatile uint8_t recState=0;
volatile static IR_PACKET ir_packet;

static uint8_t cmdprev=0;
static uint16_t addrprev=0;

static int cnt=0;
static int rptcnt=0;

void IR_DeInit(void) {

  TIM_DeInit(IR_TIM);
  GPIO_DeInit(REMO_Port);
}

void IR_Init(void) {

  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  TIM_TimeBaseInitTypeDef timer_base;

  RCC_APB1PeriphClockCmd(IR_TIM_CLK, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  // Remap TIM4 to PD12
  GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = IR_TIM_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_TimeBaseStructInit(&timer_base);
  timer_base.TIM_Prescaler = TIM_PRESCALER;
  TIM_TimeBaseInit(TIM4, &timer_base);

  TIM_ICInitStructure.TIM_Channel = IR_TIM_Channel;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(IR_TIM, &TIM_ICInitStructure);

  TIM_SelectInputTrigger(IR_TIM, TIM_TS_TI1FP1);

  TIM_SelectSlaveMode(IR_TIM, TIM_SlaveMode_Reset);

  TIM_SelectMasterSlaveMode(IR_TIM, TIM_MasterSlaveMode_Enable);

  TIM_UpdateRequestConfig(IR_TIM, TIM_UpdateSource_Regular);

  TIM_SetAutoreload(IR_TIM, IT_TIMEOUT_MAX_US);

  TIM_ClearFlag(IR_TIM, TIM_FLAG_Update);

  TIM_ITConfig(IR_TIM, TIM_IT_Update, ENABLE);

  TIM_ITConfig(IR_TIM, TIM_IT_CC1, ENABLE);

  TIM_ClearFlag(IR_TIM, TIM_FLAG_CC1);

  TIM_Cmd(IR_TIM, ENABLE);
}

void TIM4_IRQHandler (void) {

  uint16_t ICValue1;
  uint16_t ICValue2;


  if(IR_TIM->SR & TIM_FLAG_CC1) { //if((TIM_GetFlagStatus(IR_TIM, TIM_FLAG_CC1) != RESET))

    IR_TIM->SR=(uint16_t)~TIM_FLAG_CC1;//TIM_ClearFlag(IR_TIM, TIM_FLAG_CC1);
    IR_TIM->SR=(uint16_t)~TIM_IT_Update;//TIM_ClearITPendingBit(IR_TIM, TIM_IT_Update);

    ICValue1 = IR_TIM->CCR1;//TIM_GetCapture1(IR_TIM);
    ICValue2 = IR_TIM->CCR2;//TIM_GetCapture2(IR_TIM);

    if (recState == REC_Recording) {

      captCount++;

      if(captCount==1) {

        if(checkVal(ICValue1, IR_NEC1_HEADER_WIDTH, IR_NEC_TOL_PCT) &&
                checkVal(ICValue2, IR_NEC1_HEADER_PULSE, IR_NEC_TOL_PCT) ) {
           ir_packet.type=PROTOCOL_NEC;
           IR_TIM->ARR = IR_NEC_TIMEOUT_US;//TIM_SetAutoreload(IR_TIM,IR_NEC_TIMEOUT_US);
           GPIOD->BSRR = GPIO_Pin_6;//GPIO_SetBits(GPIOD, GPIO_Pin_6);
        }
        else if(checkVal(ICValue1, IR_NEC2_REPEAT_WIDTH, IR_NEC_TOL_PCT) &&
                checkVal(ICValue2, IR_NEC2_REPEAT_PULSE, IR_NEC_TOL_PCT) ) {
           ir_packet.type=PROTOCOL_NEC;
           IR_TIM->ARR = IR_NEC_TIMEOUT_US;//TIM_SetAutoreload(IR_TIM,IR_NEC_TIMEOUT_US);
           GPIOD->BSRR = GPIO_Pin_6;//GPIO_SetBits(GPIOD, GPIO_Pin_6);
        }
        else{
          recState = REC_Captured;
          GPIOD->BRR = GPIO_Pin_6;//GPIO_ResetBits(GPIOD, GPIO_Pin_6);
        }
      }
      else {
        if(checkVal(ICValue1, IR_NEC_0_WIDTH, IR_NEC_TOL_PCT) &&
           checkVal(ICValue2, IR_NEC_0_PULSE, IR_NEC_TOL_PCT)) {
          ir_packet.data = ir_packet.data >> 1;
        }
        else if(checkVal(ICValue1, IR_NEC_1_WIDTH, IR_NEC_TOL_PCT) &&
                checkVal(ICValue2, IR_NEC_1_PULSE, IR_NEC_TOL_PCT)) {
               ir_packet.data = (ir_packet.data >> 1) | 0x80000000;
             }
        else {
          recState = REC_Captured;
          GPIOD->BRR = GPIO_Pin_6;//GPIO_ResetBits(GPIOD, GPIO_Pin_6);
        }
      }
    }

    if (recState == REC_Idle) {
      recState=REC_Recording;
      captCount=0;
      ir_packet.type=0;
      ir_packet.data=0;
    }
  }

  else if (IR_TIM->SR & TIM_FLAG_Update) {//else if ((TIM_GetFlagStatus(IR_TIM, TIM_FLAG_Update) != RESET))

    IR_TIM->SR=(uint16_t)~TIM_FLAG_Update;//TIM_ClearFlag(IR_TIM, TIM_FLAG_Update);
    if (recState == REC_Recording) {
      GPIOD->BRR = GPIO_Pin_6;//GPIO_ResetBits(GPIOD, GPIO_Pin_6);
      ir_packet.lastc=IR_TIM->CCR2;
      qstore((IR_PACKET*)&ir_packet);
      recState = REC_Idle;
      IR_TIM->ARR = IT_TIMEOUT_MAX_US;//TIM_SetAutoreload(IR_TIM,IT_TIMEOUT_MAX_US);
    }
  }
}

void IR_Decode(IR_PACKET *irp) {

  char bufa[5];
  uint16_t addr;
  uint8_t cmd,cmdinv;

  switch(captCount) {
    case 1: // NEC1 repeat
      if(irp->type==PROTOCOL_NEC) {
        rptcnt++;
        cmdprev = 0;
        addrprev = 0;
        return;
      }
      break;
    case 33://NEC,SAMSUNG
      if(irp->type==PROTOCOL_NEC) {

        cmd=(irp->data & 0x00ff0000)>>16;
        cmdinv=(irp->data & 0xff000000)>>24;
        addr=(irp->data & 0x0000ffff);

        if(((cmd | cmdinv) == 0xFF) && ((cmd & ~cmdinv) == cmd)) {
        }
        else {
        }

        if((cmdprev == cmd) && (addrprev == addr)){
          cnt++;
        }
        else {
          sprintf(bufa,"%04X",addr);
          MP6090R_text(bufa,0,0,TRUE,FALSE);
          sprintf(bufa,"%02X",cmd);
          MP6090R_text(bufa,5,0,FALSE,FALSE);

          cnt=1;
          cmdprev = cmd;
          addrprev = addr;
        }
        sprintf(bufa,"%02d",cnt);
        MP6090R_text(bufa,8,0,FALSE,TRUE);
      }
      break;
    default:
      break;
  }
  rptcnt=0;
}

