/*
 *  mcx80.c
 *
 *  Scott MCX80 face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "mcx80.h"
#include "adc.h"
#include "delay.h"
#include "font13seg.h"

__align(4) uint8_t display_data[20];

static PIN pins[]={
  {{CLK_Pin,CLK_Speed,CLK_Mode},CLK_Port,CLK_Bus},
  {{DI_Pin,DI_Speed,DI_Mode},DI_Port,DI_Bus},
  {{CE_Pin, CE_Speed, CE_Mode}, CE_Port, CE_Bus},
  {{INH_Pin, INH_Speed, INH_Mode}, INH_Port, INH_Bus},
  {{KEY1_Pin, KEY1_Speed, KEY1_Mode}, KEY1_Port, KEY1_Bus},
  {{KEY2_Pin, KEY2_Speed, KEY2_Mode}, KEY2_Port, KEY2_Bus},
  {{VOLMINUS_Pin, VOLMINUS_Speed, VOLMINUS_Mode}, VOLMINUS_Port, VOLMINUS_Bus},
  {{VOLPLUS_Pin, VOLPLUS_Speed, VOLPLUS_Mode}, VOLPLUS_Port, VOLPLUS_Bus},
};

static uint8_t font_pos[MAX_POSITIONS][MAX_SEGMENTS]={
  {POS1+6,POS1+12,POS1+11,POS1+7,POS1+1,POS1,POS1+2,POS1+8,POS1+3,POS1+5,POS1+9,POS1+10,POS1+4},
  {POS2+6,POS2+12,POS2+11,POS2+7,POS2+1,POS2,POS2+2,POS2+8,POS2+3,POS2+5,POS2+9,POS2+10,POS2+4},
  {POS3+6,POS3+12,POS3+11,POS3+7,POS3+1,POS3,POS3+2,POS3+8,POS3+3,POS3+5,POS3+9,POS3+10,POS3+4},
  {POS4+6,POS4+12,POS4+11,POS4+7,POS4+1,POS4,POS4+2,POS4+8,POS4+3,POS4+5,POS4+9,POS4+10,POS4+4},
  {POS5+6,POS5+12,POS5+11,POS5+7,POS5+1,POS5,POS5+2,POS5+8,POS5+3,POS5+5,POS5+9,POS5+10,POS5+4},
  {POS6+6,POS6+12,POS6+11,POS6+7,POS6+1,POS6,POS6+2,POS6+8,POS6+3,POS6+5,POS6+9,POS6+10,POS6+4},
  {POS7+6,POS7+12,POS7+11,POS7+7,POS7+1,POS7,POS7+2,POS7+8,POS7+3,POS7+5,POS7+9,POS7+10,POS7+4},
  {POS8+6,POS8+12,POS8+11,POS8+7,POS8+1,POS8,POS8+2,POS8+8,POS8+3,POS8+5,POS8+9,POS8+10,POS8+4},
};

static void GPIO_Configuration(void) {

  uint32_t i;
  SPI_InitTypeDef  SPI_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

  for(i=0;i<sizeof(pins)/sizeof(PIN);i++) {
    RCC_APB2PeriphClockCmd(pins[i].GPIO_Bus,ENABLE);
    GPIO_Init(pins[i].GPIOx,&pins[i].GPIO_InitStructure);
  }

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);

  // Encoder configuration
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

  TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period=3;
  TIM_TimeBaseInitStruct.TIM_Prescaler=0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

  TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);

  NVIC_EnableIRQ(TIM4_IRQn);
}

static void DMA_Configuration(void) {

  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(SPI2->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)display_data;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 20;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
}

void MCX80_init(void) {

  GPIO_Configuration();
  DMA_Configuration();
  ADC_DMA_Configuration();
  GPIO_SetBits(INH_Port,INH_Pin);
  DWT_Delay(100000);
}

static void sendByte(uint8_t byte) {

  SPI_I2S_SendData(SPI2,byte);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){};
}

void MCX80_send(void) {

  //uint32_t j;

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){};

  GPIO_ResetBits(CE_Port,CE_Pin);
  sendByte(CMD_INPUT);
  GPIO_SetBits(CE_Port,CE_Pin);

  display_data[19]&=0x0F;

  DMA_Cmd(DMA1_Channel5,DISABLE);
  DMA_SetCurrDataCounter(DMA1_Channel5,20);
  DMA_Cmd(DMA1_Channel5,ENABLE);
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

//  for(j=0;j<20;j++) {
//    sendByte(display_data[j]);
//  }

  //GPIO_ResetBits(CE_Port,CE_Pin);
}

void MCX80_char(char chr,int8_t pos) {

  uint8_t i,databyte,databit;
  uint16_t ch=FONT13SEG[chr-0x20];

  for(i=0;i<MAX_SEGMENTS;i++) {
    databit=1<<(font_pos[pos][i]%8);
    databyte=font_pos[pos][i]/8;
    display_data[databyte]&=~databit;
    if(ch&0x01) {
      display_data[databyte]|=databit;
    }
    ch>>=1;
  }
}

void MCX80_text(char *pString,int8_t pos,uint8_t clear,uint8_t flush) {

  if(clear) {
    MCX80_clear_text(FALSE);
  }
  while (*pString) {
    if((pos>=0)&&(pos<=MAX_POSITIONS)) {
      MCX80_char(*pString,pos);
    }
    pos++;
    pString++;
    if(pos>=MAX_POSITIONS) break;
  }
  if(flush) {
    MCX80_send();
  }
}

void MCX80_clear_text(uint8_t flush) {

  uint32_t i;

  for(i=0;i<MAX_POSITIONS;i++)
    MCX80_char(' ',i);

  if(flush) {
    MCX80_send();
  }
}

void MCX80_symbol(uint8_t val,uint8_t on,uint8_t flush) {

  uint8_t databyte,databit;

  databit=1<<(val%8);
  databyte=val/8;

  if(on) {
    display_data[databyte]|=databit;
  }
  else {
    display_data[databyte]&=~databit;
  }

  if(flush) {
    MCX80_send();
  }
}

void MCX80_digit(uint8_t val,uint8_t flush) {

  switch(val/10) {
    case 1:MCX80_symbol(SYMBOL_DIGIT_DEC,TRUE,FALSE);
      break;
    default:
      MCX80_symbol(SYMBOL_DIGIT_DEC,FALSE,FALSE);
      break;
  }

  switch(val%10) {
    case 0:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,TRUE,FALSE);
      break;
    case 1:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 2:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,TRUE,FALSE);
      break;
    case 3:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 4:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 5:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 6:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,TRUE,FALSE);
      break;
    case 7:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 8:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,TRUE,FALSE);
      break;
    case 9:
      MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
  }

  if(flush) {
    MCX80_send();
  }
}

void MCX80_digit_off(uint8_t flush) {

  MCX80_symbol(SYMBOL_DIGIT_DEC,FALSE,FALSE);

  MCX80_symbol(SYMBOL_DIGIT_TOP_RIGHT,FALSE,FALSE);
  MCX80_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,FALSE,FALSE);
  MCX80_symbol(SYMBOL_DIGIT_TOP,FALSE,FALSE);
  MCX80_symbol(SYMBOL_DIGIT_MIDDLE,FALSE,FALSE);
  MCX80_symbol(SYMBOL_DIGIT_BOTTOM,FALSE,FALSE);
  MCX80_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
  MCX80_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);

  if(flush) {
    MCX80_send();
  }
}
