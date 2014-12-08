/*
 *  cd354mp.c
 *
 *  Roadstar CD-354MP face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "cd354mp.h"
#include "delay.h"
#include "font.h"

__align(4) uint8_t display_data[20];

static PIN pins[]={
  {{CLK_Pin,CLK_Speed,CLK_Mode},CLK_Port,CLK_Bus},
  {{DO_Pin,DO_Speed,DO_Mode},DO_Port,DO_Bus},
  {{CE_Pin, CE_Speed, CE_Mode}, CE_Port, CE_Bus},
  {{INH_Pin, INH_Speed, INH_Mode}, INH_Port, INH_Bus},
  {{KEY1_Pin, KEY1_Speed, KEY1_Mode}, KEY1_Port, KEY1_Bus},
  {{KEY2_Pin, KEY2_Speed, KEY2_Mode}, KEY2_Port, KEY2_Bus},
};

void GPIO_Configuration(void) {

  uint32_t i;
  SPI_InitTypeDef  SPI_InitStructure;

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
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
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

void CD354MP_init(void) {

  DMA_Configuration();
  GPIO_SetBits(INH_Port,INH_Pin);
  DWT_Delay(100000);
}

static void sendByte(uint8_t byte) {

  SPI_I2S_SendData(SPI2,byte);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){};
}

void CD354MP_send(void) {

  //uint32_t j;

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){};

  GPIO_ResetBits(CE_Port,CE_Pin);
  sendByte(CMD_INPUT);
  GPIO_SetBits(CE_Port,CE_Pin);

  display_data[19]&=0xF0;

  DMA_Cmd(DMA1_Channel5,DISABLE);
  DMA_SetCurrDataCounter(DMA1_Channel5,20);
  DMA_Cmd(DMA1_Channel5,ENABLE);
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

//  for(j=0;j<20;j++) {
//    sendByte(display_data[j]);
//  }

  //GPIO_ResetBits(CE_Port,CE_Pin);
}

void CD354MP_char(char chr,uint8_t pos,uint8_t flush) {

  switch(pos) {
    case 0:
      display_data[0]=(FONT[chr-0x20][0]>>1)|(display_data[0]&0x80);
      display_data[1]=((FONT[chr-0x20][0]&0x01)<<7)|(FONT[chr-0x20][1]>>1)|(display_data[1]&0x09);
      break;
    case 1:
      display_data[2]=FONT[chr-0x20][0];
      display_data[3]=FONT[chr-0x20][1]|(display_data[3]&0x13);
      break;
    case 2:
      display_data[3]=((FONT[chr-0x20][0]&0x80)>>7)|(display_data[3]&0xFE);
      display_data[4]=((FONT[chr-0x20][0])<<1)|((FONT[chr-0x20][1]&0x80)>>7);
      display_data[5]=((FONT[chr-0x20][1])<<1)|(display_data[5]&0x27);
      break;
    case 3:
      display_data[5]=((FONT[chr-0x20][0]&0xC0)>>6)|(display_data[5]&0xFC);
      display_data[6]=((FONT[chr-0x20][0])<<2)|((FONT[chr-0x20][1]&0xC0)>>6);
      display_data[7]=((FONT[chr-0x20][1])<<2)|(display_data[7]&0x4F);
      break;
    case 4:
      display_data[7]=((FONT[chr-0x20][0]&0xE0)>>5)|(display_data[7]&0xF8);
      display_data[8]=((FONT[chr-0x20][0])<<3)|((FONT[chr-0x20][1]&0xE0)>>5);
      display_data[9]=((FONT[chr-0x20][1])<<3)|(display_data[9]&0x90);
      break;
    case 5:
      display_data[9]=((FONT[chr-0x20][0]&0xF0)>>4)|(display_data[9]&0xF0);
      display_data[10]=((FONT[chr-0x20][0])<<4)|((FONT[chr-0x20][1]&0xF0)>>4)|(display_data[10]&0x01);
      display_data[11]=((FONT[chr-0x20][1])<<4)|(display_data[11]&0x3F);
      break;
    case 6:
      display_data[11]=((FONT[chr-0x20][0]&0xF8)>>3)|(display_data[11]&0xE0);
      display_data[12]=((FONT[chr-0x20][0])<<5)|((FONT[chr-0x20][1]&0xF8)>>3)|(display_data[12]&0x02);
      display_data[13]=((FONT[chr-0x20][1])<<5)|(display_data[13]&0x7F);
      break;
    case 7:
      display_data[13]=((FONT[chr-0x20][0]&0xFC)>>2)|(display_data[13]&0xC0);
      display_data[14]=((FONT[chr-0x20][0])<<6)|((FONT[chr-0x20][1]&0xFC)>>2)|(display_data[14]&0x04);
      break;
  }
  if(flush) {
    CD354MP_send();
  }
}

void CD354MP_text(char *pString,uint8_t start,uint8_t clear,uint8_t flush) {

  if(clear) {
    CD354MP_clear_text(FALSE);
  }
  while (*pString) {
    CD354MP_char(*pString++,start++,flush);
    if(start>7) break;
  }
  if(flush) {
    CD354MP_send();
  }
}

void CD354MP_clear_text(uint8_t flush) {

  display_data[0]&=SYMBOL_STEREO_BIT;
  display_data[1]&=(SYMBOL_MP3_BIT|SYMBOL_DIGIT_TOP_BIT);
  display_data[2]=0x00;
  display_data[3]&=(SYMBOL_AF_BIT|SYMBOL_DIGIT_TOP_RIGHT_BIT);
  display_data[4]=0x00;
  display_data[5]&=(SYMBOL_TA_BIT|SYMBOL_DIGIT_MIDDLE_BIT);
  display_data[6]=0x00;
  display_data[7]&=(SYMBOL_TP_BIT|SYMBOL_DIGIT_BOTTOM_RIGHT_BIT);
  display_data[8]=0x00;
  display_data[9]&=(SYMBOL_DIR_BIT|0x80);
  display_data[10]&=SYMBOL_POINT_BIT;
  display_data[11]&=SYMBOL_PTY_BIT;
  display_data[12]&=SYMBOL_DIGIT_BOTTOM_BIT;
  display_data[13]&=SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
  display_data[14]&=SYMBOL_DIGIT_TOP_LEFT_BIT;

  if(flush) {
    CD354MP_send();
  }
}

void CD354MP_symbol(uint8_t val,uint8_t bit,uint8_t on,uint8_t flush) {

  if(on) {
    display_data[val]|=bit;
  }
  else {
    display_data[val]&=~bit;
  }
  if(flush) {
    CD354MP_send();
  }
}

void CD354MP_digit(uint8_t val,uint8_t flush) {

  switch(val) {
    case 0:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]|=SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]|=SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]|=SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]&=~SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]|=SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]|=SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 1:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]|=SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]&=~SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]&=~SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]&=~SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]&=~SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]&=~SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 2:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]|=SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]|=SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]&=~SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]|=SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]&=~SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]|=SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]|=SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 3:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]|=SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]|=SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]&=~SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]|=SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]&=~SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]|=SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 4:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]|=SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]&=~SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]|=SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]|=SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]&=~SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]&=~SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 5:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]&=~SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]|=SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]|=SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]|=SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]&=~SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]|=SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 6:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]&=~SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]|=SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]|=SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]|=SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]|=SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]|=SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 7:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]|=SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]|=SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]&=~SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]&=~SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]&=~SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]&=~SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 8:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]|=SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]|=SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]|=SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]|=SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]|=SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]|=SYMBOL_DIGIT_BOTTOM_BIT;
      break;
    case 9:
      display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]|=SYMBOL_DIGIT_TOP_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_TOP_POS]|=SYMBOL_DIGIT_TOP_BIT;
      display_data[SYMBOL_DIGIT_TOP_LEFT_POS]|=SYMBOL_DIGIT_TOP_LEFT_BIT;
      display_data[SYMBOL_DIGIT_MIDDLE_POS]|=SYMBOL_DIGIT_MIDDLE_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]|=SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]&=~SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
      display_data[SYMBOL_DIGIT_BOTTOM_POS]|=SYMBOL_DIGIT_BOTTOM_BIT;
      break;
  }
  if(flush) {
    CD354MP_send();
  }
}

void CD354MP_digit_off(uint8_t flush) {

  display_data[SYMBOL_DIGIT_TOP_RIGHT_POS]&=~SYMBOL_DIGIT_TOP_RIGHT_BIT;
  display_data[SYMBOL_DIGIT_TOP_POS]&=~SYMBOL_DIGIT_TOP_BIT;
  display_data[SYMBOL_DIGIT_TOP_LEFT_POS]&=~SYMBOL_DIGIT_TOP_LEFT_BIT;
  display_data[SYMBOL_DIGIT_MIDDLE_POS]&=~SYMBOL_DIGIT_MIDDLE_BIT;
  display_data[SYMBOL_DIGIT_BOTTOM_RIGHT_POS]&=~SYMBOL_DIGIT_BOTTOM_RIGHT_BIT;
  display_data[SYMBOL_DIGIT_BOTTOM_LEFT_POS]&=~SYMBOL_DIGIT_BOTTOM_LEFT_BIT;
  display_data[SYMBOL_DIGIT_BOTTOM_POS]&=~SYMBOL_DIGIT_BOTTOM_BIT;

  if(flush) {
    CD354MP_send();
  }
}
