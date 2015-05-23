/*
 *  KDC_5060R.c
 *
 *  Kenwood KDC-KDC_5060R face plate demo
 *  LCD controller: Sanyo LC75883
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "KDC_5060R.h"
#include "delay.h"
#include "font14seg.h"
#include "adc.h"

static uint8_t display_data[27];
uint8_t key_data[4];

static PIN pins[]={
  {{CLK_Pin,CLK_Speed,CLK_Mode},CLK_Port,CLK_Bus},
  {{DATAL_Pin,DATAL_Speed,DATAL_Mode},DATAL_Port,DATAL_Bus},
  {{DATAS_Pin,DATAS_Speed,DATAS_Mode},DATAS_Port,DATAS_Bus},
  {{RESET_Pin, RESET_Speed, RESET_Mode}, RESET_Port, RESET_Bus},
  {{CE_Pin, CE_Speed, CE_Mode}, CE_Port, CE_Bus},
};

static uint8_t font_pos[MAX_POSITIONS][MAX_SEGMENTS]={
  {POS1_TOP,POS1_TOP_RIGHT,POS1_BOTTOM_RIGHT,POS1_BOTTOM,POS1_BOTTOM_LEFT,POS1_TOP_LEFT,POS1_MIDDLE_LEFT,POS1_MIDDLE_RIGHT,
   POS1_TOP_LEFT_LEANED,POS1_TOP_MIDDLE,POS1_TOP_RIGHT_LEANED,POS1_BOTTOM_RIGHT_LEANED,POS1_BOTTOM_MIDDLE,POS1_BOTTOM_LEFT_LEANED},
  {POS2_TOP,POS2_TOP_RIGHT,POS2_BOTTOM_RIGHT,POS2_BOTTOM,POS2_BOTTOM_LEFT,POS2_TOP_LEFT,POS2_MIDDLE_LEFT,POS2_MIDDLE_RIGHT,
   POS2_TOP_LEFT_LEANED,POS2_TOP_MIDDLE,POS2_TOP_RIGHT_LEANED,POS2_BOTTOM_RIGHT_LEANED,POS2_BOTTOM_MIDDLE,POS2_BOTTOM_LEFT_LEANED},
  {POS3_TOP,POS3_TOP_RIGHT,POS3_BOTTOM_RIGHT,POS3_BOTTOM,POS3_BOTTOM_LEFT,POS3_TOP_LEFT,POS3_MIDDLE_LEFT,POS3_MIDDLE_RIGHT,
   POS3_TOP_LEFT_LEANED,POS3_TOP_MIDDLE,POS3_TOP_RIGHT_LEANED,POS3_BOTTOM_RIGHT_LEANED,POS3_BOTTOM_MIDDLE,POS3_BOTTOM_LEFT_LEANED},
  {POS4_TOP,POS4_TOP_RIGHT,POS4_BOTTOM_RIGHT,POS4_BOTTOM,POS4_BOTTOM_LEFT,POS4_TOP_LEFT,POS4_MIDDLE_LEFT,POS4_MIDDLE_RIGHT,
   POS4_TOP_LEFT_LEANED,POS4_TOP_MIDDLE,POS4_TOP_RIGHT_LEANED,POS4_BOTTOM_RIGHT_LEANED,POS4_BOTTOM_MIDDLE,POS4_BOTTOM_LEFT_LEANED},
  {POS5_TOP,POS5_TOP_RIGHT,POS5_BOTTOM_RIGHT,POS5_BOTTOM,POS5_BOTTOM_LEFT,POS5_TOP_LEFT,POS5_MIDDLE_LEFT,POS5_MIDDLE_RIGHT,
   POS5_TOP_LEFT_LEANED,POS5_TOP_MIDDLE,POS5_TOP_RIGHT_LEANED,POS5_BOTTOM_RIGHT_LEANED,POS5_BOTTOM_MIDDLE,POS5_BOTTOM_LEFT_LEANED},
  {POS6_TOP,POS6_TOP_RIGHT,POS6_BOTTOM_RIGHT,POS6_BOTTOM,POS6_BOTTOM_LEFT,POS6_TOP_LEFT,POS6_MIDDLE_LEFT,POS6_MIDDLE_RIGHT,
   POS6_TOP_LEFT_LEANED,POS6_TOP_MIDDLE,POS6_TOP_RIGHT_LEANED,POS6_BOTTOM_RIGHT_LEANED,POS6_BOTTOM_MIDDLE,POS6_BOTTOM_LEFT_LEANED},
  {POS7_TOP,POS7_TOP_RIGHT,POS7_BOTTOM_RIGHT,POS7_BOTTOM,POS7_BOTTOM_LEFT,POS7_TOP_LEFT,POS7_MIDDLE_LEFT,POS7_MIDDLE_RIGHT,
   POS7_TOP_LEFT_LEANED,POS7_TOP_MIDDLE,POS7_TOP_RIGHT_LEANED,POS7_BOTTOM_RIGHT_LEANED,POS7_BOTTOM_MIDDLE,POS7_BOTTOM_LEFT_LEANED},
  {POS8_TOP,POS8_TOP_RIGHT,POS8_BOTTOM_RIGHT,POS8_BOTTOM,POS8_BOTTOM_LEFT,POS8_TOP_LEFT,POS8_MIDDLE_LEFT,POS8_MIDDLE_RIGHT,
   POS8_TOP_LEFT_LEANED,POS8_TOP_MIDDLE,POS8_TOP_RIGHT_LEANED,POS8_BOTTOM_RIGHT_LEANED,POS8_BOTTOM_MIDDLE,POS8_BOTTOM_LEFT_LEANED},
};

static void GPIO_Configuration(void) {

  uint32_t i;
  SPI_InitTypeDef  SPI_InitStructure;

  for(i=0;i<sizeof(pins)/sizeof(PIN);i++) {
    RCC_APB2PeriphClockCmd(pins[i].GPIO_Bus,ENABLE);
    GPIO_Init(pins[i].GPIOx,&pins[i].GPIO_InitStructure);
  }

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
}

void KDC_5060R_init(void) {

  GPIO_Configuration();
  ADC_DMA_Configuration();
  DWT_Delay(10000);

  // S0=0 S1=0 K0=0 K1=1 P0=1 P1=1 SC=0 DR=0 DD=0
  display_data[8]=0x0E;
  // DD=1
  display_data[17]=0x80;
  // DD=2
  display_data[26]=0x40;
  KDC_5060R_send();
}

static uint8_t sendByte(uint8_t byte) {

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){};
  SPI_I2S_SendData(SPI2,byte);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){};
  return(SPI_I2S_ReceiveData(SPI2));
}

void KDC_5060R_receive(void){

  GPIO_ResetBits(CE_Port,CE_Pin);
  sendByte(CMD_OUTPUT);
  GPIO_SetBits(CE_Port,CE_Pin);

  key_data[0]=sendByte(0);
  key_data[1]=sendByte(0);
  key_data[2]=sendByte(0);
  key_data[3]=sendByte(0);

  GPIO_ResetBits(CE_Port,CE_Pin);
}

void KDC_5060R_send(void) {

  uint32_t i,j;

  for(i=0;i<3;i++) {
    GPIO_ResetBits(CE_Port,CE_Pin);
    sendByte(CMD_INPUT);
    GPIO_SetBits(CE_Port,CE_Pin);

    // display data + control data + fixed data + dd
    for(j=0;j<9;j++) {
      sendByte(display_data[i*9+j]);
    }
  }

  GPIO_ResetBits(CE_Port,CE_Pin);
}

void KDC_5060R_text(char *pString,int8_t pos,uint8_t clear,uint8_t flush) {

  if(clear){
    KDC_5060R_clear_text(FALSE);
  }
  while(*pString) {
    if((pos>=0)&&(pos<MAX_POSITIONS)) {
      KDC_5060R_char(*pString,pos);
    }
    pos++;
    pString++;
    if(pos>=MAX_POSITIONS) break;
  }
  if(flush){
    KDC_5060R_send();
  }
}

void KDC_5060R_char(char chr,int8_t pos) {

  uint8_t i,databyte,databit;
  uint16_t ch=FONT14SEG[chr-0x20];

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

void KDC_5060R_clear_text(uint8_t flush) {

  uint32_t i;

  for(i=0;i<MAX_POSITIONS;i++)
    KDC_5060R_char(' ',i);

  if(flush){
    KDC_5060R_send();
  }
}

void KDC_5060R_digit(uint8_t val,uint8_t flush) {

  switch(val){
    case 0:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,TRUE,FALSE);
      break;
    case 1:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 2:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,TRUE,FALSE);
      break;
    case 3:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 4:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 5:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 6:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,TRUE,FALSE);
      break;
    case 7:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
    case 8:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,TRUE,FALSE);
      break;
    case 9:
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,TRUE,FALSE);
      KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);
      break;
  }

  if(flush){
    KDC_5060R_send();
  }
}

void KDC_5060R_digit_off(uint8_t flush) {

  KDC_5060R_symbol(SYMBOL_DIGIT_TOP_RIGHT,FALSE,FALSE);
  KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_RIGHT,FALSE,FALSE);
  KDC_5060R_symbol(SYMBOL_DIGIT_TOP,FALSE,FALSE);
  KDC_5060R_symbol(SYMBOL_DIGIT_MIDDLE,FALSE,FALSE);
  KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM,FALSE,FALSE);
  KDC_5060R_symbol(SYMBOL_DIGIT_TOP_LEFT,FALSE,FALSE);
  KDC_5060R_symbol(SYMBOL_DIGIT_BOTTOM_LEFT,FALSE,FALSE);

  if(flush){
    KDC_5060R_send();
  }
}

void KDC_5060R_symbol(uint8_t val,uint8_t on,uint8_t flush) {

  uint8_t databyte,databit;

  databit=1<<(val%8);
  databyte=val/8;

  if(on) {
    display_data[databyte]|=databit;
  }
  else {
    display_data[databyte]&=~databit;
  }

  if(flush){
    KDC_5060R_send();
  }
}

void KDC_5060R_char_all(uint8_t pos,uint8_t on,uint8_t flush) {

  uint8_t j,databyte,databit;

  for(j=0;j<MAX_SEGMENTS;j++) {
    databit=1<<(font_pos[pos][j]%8);
    databyte=font_pos[pos][j]/8;
    display_data[databyte]&=~databit;
    if(on) {
      display_data[databyte]|=databit;
    }
  }

  if(flush){
    KDC_5060R_send();
  }
}

void KDC_5060R_text_all(uint8_t on,uint8_t flush) {

  uint8_t i;

  for(i=0;i<MAX_POSITIONS;i++) {
    KDC_5060R_char_all(i,on,flush);
  }

  if(flush){
    KDC_5060R_send();
  }
}

void KDC_5060R_leds_red(void) {

  display_data[0]=0x00;
  display_data[1]|=0x02;
  KDC_5060R_send();
}

void KDC_5060R_leds_green(void) {

  display_data[0]=0x40;
  display_data[1]&=0xF0;
  KDC_5060R_send();
}

void KDC_5060R_leds_yellow(void) {

  display_data[0]=0x40;
  display_data[1]|=0x02;
  KDC_5060R_send();
}

void KDC_5060R_leds_off(void) {

  display_data[0]=0x00;
  display_data[1]&=0xF0;
  KDC_5060R_send();
}
