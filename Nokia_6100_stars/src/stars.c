/*
 *  stars.c
 *
 *  Moving stars demo.
 *  Nokia 6100/6020 LCD connected to HY-STM32 board.
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "stars.h"
#include "PCF8833.h"
#include "colors_16bit.h"
#include "delay.h"
#include <stdlib.h>
#include "xorshift.h"

extern volatile uint16_t ADCConvertedValue[];
uint16_t actual_stars;

#ifdef STARS_COLOR_8BIT
uint8_t frame[MAX_X*MAX_Y*9/8];
#elif defined STARS_COLOR_16BIT
uint8_t frame[MAX_X*MAX_Y*2*9/8];
#endif

TStar stars[MAXSTARS];

#ifdef STARS_COLOR_8BIT
//uint8_t clr[]={TO_8BIT_COLOR(WHITE),TO_8BIT_COLOR(YELLOW),TO_8BIT_COLOR(AQUA)};
uint8_t clr[]={TO_8BIT_COLOR(WHITE),TO_8BIT_COLOR(YELLOW),TO_8BIT_COLOR(CYAN)};
//uint8_t clr[]={TO_8BIT_COLOR(WHITE),TO_8BIT_COLOR(WHITE),TO_8BIT_COLOR(WHITE)};
#elif defined STARS_COLOR_16BIT
//uint16_t clr[]={WHITE,YELLOW,AQUA};
uint16_t clr[]={WHITE,YELLOW,CYAN};
//uint16_t clr[]={WHITE,WHITE,WHITE};
#endif

void display_stars(void) {
  uint32_t i;
  uint16_t bits9a
#ifdef STARS_COLOR_16BIT
          ,bits9b
#endif
  ;
  uint8_t d;

  stars_DMA_Init();

  for (i=0; i<MAXSTARS; i++) {
    stars[i].x = xor128() % MAX_X;
    stars[i].y = xor128() % MAX_Y;
    stars[i].plane = xor128() % 3;     // star colour between 0 and 2
  }

  while(1) {

    stars_DMA_ClearBuf();

#ifdef STARS_COLOR_8BIT
    bits9a=TO_8BIT_COLOR(BLACK)|0x100;
#elif  defined STARS_COLOR_16BIT
    bits9a=((BLACK&0xFF00)>>8)|0x100;
    bits9b=(BLACK&0x00FF)|0x100;
#endif

#ifdef STARS_COLOR_8BIT
    for(i=0;i<MAX_X*MAX_Y*9/8-1;i++){
#elif defined  STARS_COLOR_16BIT
    for(i=0;i<MAX_X*MAX_Y*2*9/8-2;i++){
#endif

      d=i%9;
      frame[i] |= bits9a >> (1+d);
      frame[i+1] |= bits9a << (7-d);
#ifdef STARS_COLOR_16BIT
      frame[i+1] |= bits9b >> (1+d);
      frame[i+2] |= bits9b << (7-d);
#endif
    }

    actual_stars=ADCConvertedValue[1];
    for (i=0; i<actual_stars; i++) {
      stars[i].x += (int)((1+stars[i].plane));
      if (stars[i].x > MAX_X-1) {
        stars[i].x = 0;
        stars[i].y = xor128() % MAX_Y;
      }
#ifdef STARS_COLOR_8BIT
      bits9a=(clr[stars[i].plane]&0xFF)|0x100;
      d=((stars[i].x*MAX_X+stars[i].y)*9/8)%9;
      frame[(stars[i].x*MAX_X+stars[i].y)*9/8] |= bits9a >> (1+d);
      frame[(stars[i].x*MAX_X+stars[i].y)*9/8+1] |= bits9a << (7-d);
#elif  defined STARS_COLOR_16BIT
      bits9a=((clr[stars[i].plane]&0xFF00)>>8)|0x100;
      bits9b=(clr[stars[i].plane]&0x00FF)|0x100;
      d=((stars[i].x*MAX_X+stars[i].y)*2*9/8)%9;
      frame[(stars[i].x*MAX_X+stars[i].y)*2*9/8] |= bits9a >> (1+d);
      frame[(stars[i].x*MAX_X+stars[i].y)*2*9/8+1] |= bits9a << (7-d);
      frame[(stars[i].x*MAX_X+stars[i].y)*2*9/8+1] |= bits9b >> (1+d);
      frame[(stars[i].x*MAX_X+stars[i].y)*2*9/8+2] |= bits9b << (7-d);
#endif
    }
    PCF8833_SPI9bits_Flush();
    PCF8833_SPI9bits(PCF8833_NOP);
    PCF8833_SPI9bits(PCF8833_PASET);
    PCF8833_SPI9bits(1|0x100);
    PCF8833_SPI9bits(MAX_X|0x100);
    PCF8833_SPI9bits(PCF8833_CASET);
    PCF8833_SPI9bits(1|0x100);
    PCF8833_SPI9bits(MAX_Y|0x100);
    PCF8833_SPI9bits(PCF8833_RAMWR);

    stars_DMA_SpiSend();

    DWT_Delay(ADCConvertedValue[0]*10);
  }
}

uint32_t zero=0;

void stars_DMA_Init(void) {

  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(STARS_SPI_DMA_CHANNEL);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(STARS_SPI->DR));
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(STARS_SPI_DMA_CHANNEL, &DMA_InitStructure);

  DMA_DeInit(DMA1_Channel6);

  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&zero;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;

  DMA_Init(DMA1_Channel6, &DMA_InitStructure);

/*
  STARS_SPI_DMA_CHANNEL->CCR = 0;
  STARS_SPI_DMA_CHANNEL->CCR |= DMA_DIR_PeripheralDST |
                                DMA_Mode_Normal |
                                DMA_PeripheralInc_Disable |
                                DMA_MemoryInc_Enable |
                                DMA_PeripheralDataSize_Byte |
                                DMA_MemoryDataSize_Byte |
                                DMA_Priority_High |
                                DMA_M2M_Disable;

  STARS_SPI_DMA_CHANNEL->CPAR = (uint32_t)(&(STARS_SPI->DR));

  DMA1_Channel6->CCR = 0;
  DMA1_Channel6->CCR |= DMA_DIR_PeripheralDST |
                        DMA_Mode_Normal |
                        DMA_PeripheralInc_Enable |
                        DMA_MemoryInc_Disable |
                        DMA_PeripheralDataSize_Word |
                        DMA_MemoryDataSize_Word |
                        DMA_Priority_VeryHigh |
                        DMA_M2M_Enable;

  DMA1_Channel6->CMAR = (uint32_t)&zero;
*/
}

void stars_DMA_ClearBuf(void) {

#ifdef STARS_COLOR_8BIT
  DMA1_Channel6->CNDTR = MAX_X*MAX_Y*9/32;
#elif  defined STARS_COLOR_16BIT
  DMA1_Channel6->CNDTR = MAX_X*MAX_Y*2*9/32;
#endif

  DMA1_Channel6->CPAR =(uint32_t)frame;

  DMA1_Channel6->CCR |= DMA_CCR1_EN; //DMA_Cmd(DMA1_Channel6, ENABLE);
  while(!(DMA1->ISR&DMA1_FLAG_TC6)); //while (DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET);
  DMA1->IFCR |= ((uint32_t)(DMA_ISR_GIF6 | DMA_ISR_TCIF6 | DMA_ISR_HTIF6 | DMA_ISR_TEIF6));
  DMA1_Channel6->CCR &= (uint16_t)(~DMA_CCR1_EN); //DMA_Cmd(DMA1_Channel6, DISABLE);
}

void stars_DMA_SpiSend(void) {

#ifdef STARS_COLOR_8BIT
  STARS_SPI_DMA_CHANNEL->CNDTR = MAX_X*MAX_Y*9/8;
#elif  defined STARS_COLOR_16BIT
  STARS_SPI_DMA_CHANNEL->CNDTR = MAX_X*MAX_Y*2*9/8;
#endif

  STARS_SPI_DMA_CHANNEL->CMAR =(uint32_t)frame;

  STARS_SPI_DMA_CHANNEL->CCR |= DMA_IT_TC;// DMA_ITConfig(STARS_SPI_DMA_CHANNEL, DMA_IT_TC, ENABLE);
  STARS_SPI_DMA_CHANNEL->CCR |= DMA_CCR1_EN; //DMA_Cmd(STARS_SPI_DMA_CHANNEL, ENABLE);
  STARS_SPI->CR2 |= SPI_I2S_DMAReq_Tx; //SPI_I2S_DMACmd(STARS_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
  while(!(DMA1->ISR&STARS_SPI_DMA_TC));
  DMA1->IFCR |= ((uint32_t)(STARS_SPI_DMA_TCIF));
  STARS_SPI_DMA_CHANNEL->CCR &= ~DMA_IT_TC; //DMA_ITConfig(STARS_SPI_DMA_CHANNEL, DMA_IT_TC, DISABLE);
  STARS_SPI_DMA_CHANNEL->CCR &= (uint16_t)~DMA_CCR1_EN; //DMA_Cmd(STARS_SPI_DMA_CHANNEL, DISABLE);
  STARS_SPI->CR2 &= (uint16_t)~SPI_I2S_DMAReq_Tx; //SPI_I2S_DMACmd(STARS_SPI, SPI_I2S_DMAReq_Tx, DISABLE);

}
