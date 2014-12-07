/*
 *  mp6090r.c
 *
 *  Kenwood KDC-MP6090R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "mp6090r.h"
#include "delay.h"
#include "font.h"

static uint8_t display_data[MAX_SYMBOLS*MAX_WIDTH];
static uint8_t control_data1,control_data2;
uint8_t key_data[4];

static PIN pins[]={
  {{DIM_Pin, DIM_Speed, DIM_Mode}, DIM_Port, DIM_Bus},
  {{CLK_Pin,CLK_Speed,CLK_Mode},CLK_Port,CLK_Bus},
  {{DATAL_Pin,DATAL_Speed,DATAL_Mode},DATAL_Port,DATAL_Bus},
  {{DATAS_Pin,DATAS_Speed,DATAS_Mode},DATAS_Port,DATAS_Bus},
  {{RESET_Pin, RESET_Speed, RESET_Mode}, RESET_Port, RESET_Bus},
  {{CE_Pin, CE_Speed, CE_Mode}, CE_Port, CE_Bus},
  {{REMO_Pin, REMO_Speed, REMO_Mode}, REMO_Port, REMO_Bus},
};

void GPIO_Configuration(void) {

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
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
}

void MP6090R_init(CONTRAST c) {

  GPIO_SetBits(RESET_Port,RESET_Pin);
  DWT_Delay(10000);

  control_data1=0xFC;
  control_data2=0;

  MP6090R_clear_buf(FALSE);
  MP6090R_contrast(c);
}

static uint8_t sendByte(uint8_t byte) {

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){};
  SPI_I2S_SendData(SPI2,byte);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){};
  return(SPI_I2S_ReceiveData(SPI2));
}

void MP6090R_receive(void){

  GPIO_ResetBits(CE_Port,CE_Pin);
  sendByte(CMD_OUTPUT);
  GPIO_SetBits(CE_Port,CE_Pin);

  key_data[0] = sendByte(0);
  key_data[1] = sendByte(0);
  key_data[2] = sendByte(0);
  key_data[3] = sendByte(0);

  GPIO_ResetBits(CE_Port,CE_Pin);
}

void MP6090R_send(void) {

  uint32_t i,j,dd=0;

  for(i=0;i<4;i++) {
    GPIO_ResetBits(CE_Port,CE_Pin);
    sendByte(CMD_INPUT);
    GPIO_SetBits(CE_Port,CE_Pin);

    // display data
    for(j=0;j<15;j++) {
      sendByte(display_data[i*15+j]);
    }
    // dd
    sendByte(dd++);
  }

  GPIO_ResetBits(CE_Port,CE_Pin);
  sendByte(CMD_INPUT);
  GPIO_SetBits(CE_Port,CE_Pin);

  // control data 1
  sendByte(control_data1);
  // control data 2
  sendByte(control_data2);
  // dd
  sendByte(dd);

  GPIO_ResetBits(CE_Port,CE_Pin);
}

// DT1,DT2 must be 0
// SP=1 sleep mode; SP=0 normal
// SC=0 segments on,; SC=1 segments off

void MP6090R_text(char *pString,uint8_t start,uint8_t pos,uint8_t clear,uint8_t flush){

  if(clear){
    MP6090R_clear_buf(FALSE);
  }
  while(*pString) {
    MP6090R_char(*pString++,start++,pos);
    if(start>=MAX_SYMBOLS) break;
  }
  if(flush){
    MP6090R_send();
  }
}
void MP6090R_char(char chr,uint8_t start,uint8_t pos){

  display_data[start*MAX_WIDTH+pos]=FONT5x7[chr-0x20][0]|(display_data[start*MAX_WIDTH+pos]&0x01);
  display_data[start*MAX_WIDTH+pos+1]=FONT5x7[chr-0x20][1]|(display_data[start*MAX_WIDTH+pos+1]&0x01);
  display_data[start*MAX_WIDTH+pos+2]=FONT5x7[chr-0x20][2]|(display_data[start*MAX_WIDTH+pos+2]&0x01);
  display_data[start*MAX_WIDTH+pos+3]=FONT5x7[chr-0x20][3]|(display_data[start*MAX_WIDTH+pos+3]&0x01);
  display_data[start*MAX_WIDTH+pos+4]=FONT5x7[chr-0x20][4]|(display_data[start*MAX_WIDTH+pos+4]&0x01);
}

void MP6090R_clear_buf(uint8_t flush) {

  uint32_t i;

  for(i=0;i<MAX_SYMBOLS*MAX_WIDTH;i++)
    display_data[i]=0x00;

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_fill_buf(uint8_t flush) {

  uint32_t i;

  for(i=0;i<MAX_SYMBOLS*MAX_WIDTH;i++)
    display_data[i]=0xFF;

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_clear_text(uint8_t flush) {

  uint32_t i;

  for(i=0;i<MAX_SYMBOLS*MAX_WIDTH;i++)
    display_data[i]&=0x01;

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_clear_symbols(uint8_t flush) {

  uint32_t i;

  for(i=0;i<MAX_SYMBOLS*MAX_WIDTH;i++)
    display_data[i]&=0xFE;

  if(flush){
    MP6090R_send();
  }
}


void MP6090R_scroll_left(uint8_t pos,uint8_t flush) {

  uint32_t i;

  for(i=0;i<=MAX_SYMBOLS*MAX_WIDTH-1-pos;i++)
    display_data[i]=(display_data[i+pos]&0xFE)|(display_data[i]&0x01);

  for(i=MAX_SYMBOLS*MAX_WIDTH-pos;i<MAX_SYMBOLS*MAX_WIDTH;i++)
    display_data[i]&=0x01;

  if(flush){
    MP6090R_send();
  }
}
void MP6090R_scroll_right(uint8_t pos,uint8_t flush) {

  int32_t i;

  for(i=MAX_SYMBOLS*MAX_WIDTH-1;i>=pos;i--)
    display_data[i]=(display_data[i-pos]&0xFE)|(display_data[i]&0x01);

  for(i=pos-1;i>=0;i--)
    display_data[i]&=0x01;

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_scroll_up(uint8_t pos,uint8_t flush) {

  uint32_t i;

  for(i=0;i<MAX_SYMBOLS*MAX_WIDTH;i++)
    display_data[i]=((display_data[i]&0xFE)<<pos)|(display_data[i]&0x01);

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_scroll_down(uint8_t pos,uint8_t flush) {

  uint32_t i;

  for(i=0;i<=MAX_SYMBOLS*MAX_WIDTH-1;i++)
    display_data[i]=((display_data[i]>>pos)&0xFE)|(display_data[i]&0x01);

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_digit(uint8_t val,uint8_t flush) {

  switch(val){
    case 0:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_TOP]|=0x01;
      display_data[SYMBOL_DIGIT_TOP_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_MIDDLE]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM]|=0x01;
      break;
    case 1:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_TOP]&=0xFE;
      display_data[SYMBOL_DIGIT_TOP_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_MIDDLE]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM]&=0xFE;
      break;
    case 2:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_TOP]|=0x01;
      display_data[SYMBOL_DIGIT_TOP_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_MIDDLE]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM]|=0x01;
      break;
    case 3:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_TOP]|=0x01;
      display_data[SYMBOL_DIGIT_TOP_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_MIDDLE]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM]|=0x01;
      break;
    case 4:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_TOP]&=0xFE;
      display_data[SYMBOL_DIGIT_TOP_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_MIDDLE]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM]&=0xFE;
      break;
    case 5:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]&=0xFE;
      display_data[SYMBOL_DIGIT_TOP]|=0x01;
      display_data[SYMBOL_DIGIT_TOP_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_MIDDLE]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM]|=0x01;
      break;
    case 6:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]&=0xFE;
      display_data[SYMBOL_DIGIT_TOP]|=0x01;
      display_data[SYMBOL_DIGIT_TOP_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_MIDDLE]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM]|=0x01;
      break;
    case 7:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_TOP]|=0x01;
      display_data[SYMBOL_DIGIT_TOP_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_MIDDLE]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM]&=0xFE;
      break;
    case 8:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_TOP]|=0x01;
      display_data[SYMBOL_DIGIT_TOP_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_MIDDLE]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM]|=0x01;
      break;
    case 9:
      display_data[SYMBOL_DIGIT_TOP_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_TOP]|=0x01;
      display_data[SYMBOL_DIGIT_TOP_LEFT]|=0x01;
      display_data[SYMBOL_DIGIT_MIDDLE]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]|=0x01;
      display_data[SYMBOL_DIGIT_BOTTOM_LEFT]&=0xFE;
      display_data[SYMBOL_DIGIT_BOTTOM]|=0x01;
      break;
  }

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_digit_off(uint8_t flush) {

  display_data[SYMBOL_DIGIT_TOP_RIGHT]&=0xFE;
  display_data[SYMBOL_DIGIT_TOP]&=0xFE;
  display_data[SYMBOL_DIGIT_TOP_LEFT]&=0xFE;
  display_data[SYMBOL_DIGIT_MIDDLE]&=0xFE;
  display_data[SYMBOL_DIGIT_BOTTOM_RIGHT]&=0xFE;
  display_data[SYMBOL_DIGIT_BOTTOM_LEFT]&=0xFE;
  display_data[SYMBOL_DIGIT_BOTTOM]&=0xFE;

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_symbol(uint8_t val,uint8_t on,uint8_t flush) {

  if(on) {
    display_data[val]|=0x01;
  }
  else {
    display_data[val]&=0xFE;
  }

  if(flush){
    MP6090R_send();
  }
}

void MP6090R_contrast(CONTRAST val) {

  control_data2=(control_data2&0xC1)|val|CTC_ON;
  MP6090R_send();
}
void MP6090R_leds_red(void) {

  control_data1|=LEDS_RED;
  control_data2&=~LEDS_GREEN;
  MP6090R_send();
}
void MP6090R_leds_green(void) {

  control_data1&=~LEDS_RED;
  control_data2|=LEDS_GREEN;
  MP6090R_send();
}
void MP6090R_leds_off(void) {

  control_data1&=~LEDS_RED;
  control_data2&=~LEDS_GREEN;
  MP6090R_send();
}

void MP6090R_dimmer(uint8_t val) {

  if(val) {
    GPIO_ResetBits(DIM_Port,DIM_Pin);
  }
  else {
    GPIO_SetBits(DIM_Port,DIM_Pin);
  }
}
