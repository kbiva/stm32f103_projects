/*
 *  drx7375.c
 *
 *  Clarion DRX7375R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "drx7375r.h"
#include "delay.h"
#include "font.h"

uint8_t key_data[4];

DATA_LC75854 packet_LC75854[4];
DATA_LC75824 packet_LC75824[4];

uint16_t symbols_a[32]={
 SYMBOL_LC75854_RING1,
 SYMBOL_LC75854_RING2,
 SYMBOL_LC75854_RING3,
 SYMBOL_LC75854_RING4,
 SYMBOL_LC75854_RING5,
 SYMBOL_LC75854_RING6,
 SYMBOL_LC75854_RING7,
 SYMBOL_LC75854_RING8,
 SYMBOL_LC75854_RING9,
 SYMBOL_LC75854_RING10,
 SYMBOL_LC75854_RING11,
 SYMBOL_LC75854_RING12,
 SYMBOL_LC75854_RING13,
 SYMBOL_LC75854_RING14,
 SYMBOL_LC75854_RING15,
 SYMBOL_LC75854_RING16,
 SYMBOL_LC75854_B,
 SYMBOL_LC75854_EQ,
 SYMBOL_LC75854_U,
 SYMBOL_LC75854_ISR,
 SYMBOL_LC75854_AF,
 SYMBOL_LC75854_REG,
 SYMBOL_LC75854_TA,
 SYMBOL_LC75854_TP,
 SYMBOL_LC75854_PTY,
 SYMBOL_LC75854_BRACKETS,
 SYMBOL_LC75854_6,
 SYMBOL_LC75854_5,
 SYMBOL_LC75854_4,
 SYMBOL_LC75854_3,
 SYMBOL_LC75854_2,
 SYMBOL_LC75854_1
};

uint16_t symbols_b[66]={
 SYMBOL_LC75824_LETTER4UP,
 SYMBOL_LC75824_LETTER3DOWNBACKSLASH,
 SYMBOL_LC75824_LETTER3DOWNRIGHTBAR,
 SYMBOL_LC75824_LETTER4UPRIGHTBAR,
 SYMBOL_LC75824_LETTER4MIDDLELINE,
 SYMBOL_LC75824_LETTER3MIDDLEBAR,
 SYMBOL_LC75824_LETTER3MIDDLELINE,
 SYMBOL_LC75824_LETTER4UPLEFTBAR,
 SYMBOL_LC75824_LETTER4DOWNLINE,
 SYMBOL_LC75824_LETTER4DOWNLEFTBAR,
 SYMBOL_LC75824_LETTER3UPRIGHTBAR,
 SYMBOL_LC75824_LETTER4DOWNRIGHTBAR,
 SYMBOL_LC75824_ADJ,
 SYMBOL_LC75824_LESS_MORE,
 SYMBOL_LC75824_A_M,
 SYMBOL_LC75824_TITLE,
 SYMBOL_LC75824_SRCH,
 SYMBOL_LC75824_LESS_MORE_FILLED,
 SYMBOL_LC75824_VOL,
 SYMBOL_LC75824_UP_DOWN_FILLED,
 SYMBOL_LC75824_LETTER1L,
 SYMBOL_LC75824_LETTER1MIDDLEBAR,
 SYMBOL_LC75824_LETTER1UPLINE,
 SYMBOL_LC75824_DSP,
 SYMBOL_LC75824_DISC,
 SYMBOL_LC75824_S,
 SYMBOL_LC75824_DSP_2,
 SYMBOL_LC75824_DSP_U,
 SYMBOL_LC75824_TAPE,
 SYMBOL_LC75824_DSP_1,
 SYMBOL_LC75824_DSP_B,
 SYMBOL_LC75824_ST,
 SYMBOL_LC75824_BLS,
 SYMBOL_LC75824_LETTER2LEFTBAR,
 SYMBOL_LC75824_LETTER3UPSLASH,
 SYMBOL_LC75824_LETTER3UPLINE,
 SYMBOL_LC75824_LETTER2MIDDLELINE,
 SYMBOL_LC75824_LETTER2SLASHES,
 SYMBOL_LC75824_LETTER3LEFTBAR,
 SYMBOL_LC75824_LETTER3UPBASKSLASH,
 SYMBOL_LC75824_LETTER2MIDDLEBAR,
 SYMBOL_LC75824_LETTER2DOWNLINE,
 SYMBOL_LC75824_LETTER2UPLINE,
 SYMBOL_LC75824_LETTER3DOWNSLASH,
 SYMBOL_LC75824_LETTER3DOWNLINE,
 SYMBOL_LC75824_LETTER2RIGHTBAR,
 SYMBOL_LC75824_B,
 SYMBOL_LC75824_C,
 SYMBOL_LC75824_NR,
 SYMBOL_LC75824_PERIODMIDDLEUP,
 SYMBOL_LC75824_APERIOD,
 SYMBOL_LC75824_LD,
 SYMBOL_LC75824_PERIODMIDDLEDOWN,
 SYMBOL_LC75824_BRACKETS_DISKS,
 SYMBOL_LC75824_DISKS,
 SYMBOL_LC75824_TV,
 SYMBOL_LC75824_DIGITAL,
 SYMBOL_LC75824_PERIODRIGHTUP,
 SYMBOL_LC75824_DISC_FRAMED,
 SYMBOL_LC75824_PERIODRIGHTDOWN,
 SYMBOL_LC75824_SCN_FRAME,
 SYMBOL_LC75824_SCN,
 SYMBOL_LC75824_RPT_FRAME,
 SYMBOL_LC75824_RDM_FRAME,
 SYMBOL_LC75824_RDM,
 SYMBOL_LC75824_RPT
};

static PIN pins[]={
  {{CLK_Pin,CLK_Speed,CLK_Mode},CLK_Port,CLK_Bus},
  {{DATAL_Pin,DATAL_Speed,DATAL_Mode},DATAL_Port,DATAL_Bus},
  {{DATAS_Pin,DATAS_Speed,DATAS_Mode},DATAS_Port,DATAS_Bus},
  {{CE_Pin, CE_Speed, CE_Mode}, CE_Port, CE_Bus},
  {{REMO_Pin, REMO_Speed, REMO_Mode}, REMO_Port, REMO_Bus},
  {{DIM_Pin, DIM_Speed, DIM_Mode}, DIM_Port, DIM_Bus},
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
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);

}

static uint8_t sendByte(uint8_t byte) {

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){};
  SPI_I2S_SendData(SPI2,byte);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){};
  return(SPI_I2S_ReceiveData(SPI2));
}

void DRX7375R_init(void) {

  uint32_t i;

  GPIO_ResetBits(CE_Port,CE_Pin);

  DWT_Delay(10000);

  for(i=0;i<4;i++){
    packet_LC75854[i].ccb_address=CMD_INPUT_LC75854;
    packet_LC75854[i].data[6]=i;
    packet_LC75824[i].ccb_address=CMD_INPUT_LC75824;
    packet_LC75824[i].data[7]=i;
  }

  packet_LC75854[0].data[6]|=BIAS_LC75854|KEY_SCAN_LC75854;
  packet_LC75824[0].data[7]|=BIAS_LC75824;

  DRX7375R_clear(FALSE);
}

void DRX7375R_fill(uint32_t flush) {

  DRX7375R_fill_LC75854(flush);
  DRX7375R_fill_LC75824(flush);
}

void DRX7375R_fill_LC75854(uint32_t flush) {

  uint32_t i,j;

  for(i=0;i<4;i++){
    for(j=0;j<5;j++){
      packet_LC75854[i].data[j]=0xFF;
    }
  }
  packet_LC75854[0].data[5]|=0xF0;

  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_fill_LC75824(uint32_t flush) {

  uint32_t i,j;

  for(i=0;i<4;i++){
    for(j=0;j<6;j++){
      packet_LC75824[i].data[j]=0xFF;
    }
  }
  packet_LC75824[0].data[6]|=0xF0;
  packet_LC75824[1].data[6]|=0xF0;
  packet_LC75824[2].data[6]|=0xF0;

  if(flush){
    DRX7375R_send_LC75824();
  }
}

void DRX7375R_clear(uint32_t flush) {

  DRX7375R_clear_LC75854(flush);
  DRX7375R_clear_LC75824(flush);
}

void DRX7375R_clear_LC75854(uint32_t flush) {

  uint32_t i,j;

  for(i=0;i<4;i++){
    for(j=0;j<5;j++){
      packet_LC75854[i].data[j]=0x00;
    }
  }
  packet_LC75854[0].data[5]&=0x0F;

  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_clear_LC75824(uint32_t flush) {

  uint32_t i,j;

  for(i=0;i<4;i++){
    for(j=0;j<6;j++){
      packet_LC75824[i].data[j]=0x00;
    }
  }
  packet_LC75824[0].data[6]&=0x0F;
  packet_LC75824[1].data[6]&=0x0F;
  packet_LC75824[2].data[6]&=0x0F;

  if(flush){
    DRX7375R_send_LC75824();
  }
}

void DRX7375R_send(void) {

  DRX7375R_send_LC75854();
  DRX7375R_send_LC75824();
}

void DRX7375R_send_LC75854(void) {

  uint32_t i,j;

  for(i=0;i<4;i++) {

    GPIO_ResetBits(CE_Port,CE_Pin);
    sendByte(packet_LC75854[i].ccb_address);
    GPIO_SetBits(CE_Port,CE_Pin);

    for(j=0;j<7;j++) {
      sendByte(packet_LC75854[i].data[j]);
    }
  }
  GPIO_ResetBits(CE_Port,CE_Pin);
}

void DRX7375R_send_LC75824(void) {

  uint32_t i,j;

  for(i=0;i<4;i++) {

    GPIO_ResetBits(CE_Port,CE_Pin);
    sendByte(packet_LC75824[i].ccb_address);
    GPIO_SetBits(CE_Port,CE_Pin);

    for(j=0;j<8;j++) {
      sendByte(packet_LC75824[i].data[j]);
    }
  }
  GPIO_ResetBits(CE_Port,CE_Pin);
}

void DRX7375R_receive_LC75854(void) {

  GPIO_ResetBits(CE_Port,CE_Pin);
  sendByte(CMD_OUTPUT_LC75854);
  GPIO_SetBits(CE_Port,CE_Pin);

  key_data[0] = sendByte(0);
  key_data[1] = sendByte(0);
  key_data[2] = sendByte(0);
  key_data[3] = sendByte(0);

  GPIO_ResetBits(CE_Port,CE_Pin);
}


void DRX7375R_text(char *pString,int pos,int clear,uint32_t flush) {

  if(clear){
    DRX7375R_clear_text(FALSE);
  }
  while(*pString) {
    DRX7375R_char(*pString++,pos++);
    if(pos>MAX_POS)
      break;
  }
  if(flush){
    DRX7375R_send_LC75824();
  }
}

void DRX7375R_char(char chr,int pos) {

  DRX7375R_clear_char(pos);
  switch(pos){
    case 0:
      packet_LC75824[0].data[3]|=FONT[chr-0x20][0];
      packet_LC75824[0].data[4]|=FONT[chr-0x20][1];
    break;
    case 1:
      packet_LC75824[0].data[5]|=FONT[chr-0x20][0];
      packet_LC75824[0].data[6]|=(FONT[chr-0x20][1]&0xF0);
      packet_LC75824[1].data[0]|=((FONT[chr-0x20][1]&0x0F)<<4);
    break;
    case 2:
      packet_LC75824[1].data[0]|=((FONT[chr-0x20][0]&0xF0)>>4);
      packet_LC75824[1].data[1]|=((FONT[chr-0x20][0]&0x0F)<<4);
      packet_LC75824[1].data[1]|=((FONT[chr-0x20][1]&0xF0)>>4);
      packet_LC75824[1].data[2]|=((FONT[chr-0x20][1]&0x0F)<<4);
    break;
    case 3:
      packet_LC75824[1].data[4]|=((FONT[chr-0x20][0]&0xF0)>>4);
      packet_LC75824[1].data[5]|=((FONT[chr-0x20][0]&0x0F)<<4);
      packet_LC75824[1].data[5]|=((FONT[chr-0x20][1]&0xF0)>>4);
      packet_LC75824[1].data[6]|=((FONT[chr-0x20][1]&0x0F)<<4);
    break;
    case 4:
      packet_LC75824[2].data[0]|=FONT[chr-0x20][0];
      packet_LC75824[2].data[1]|=FONT[chr-0x20][1];
    break;
    case 5:
      packet_LC75824[2].data[2]|=((FONT[chr-0x20][0]&0xF0)>>4);
      packet_LC75824[2].data[3]|=((FONT[chr-0x20][0]&0x0F)<<4);
      packet_LC75824[2].data[3]|=((FONT[chr-0x20][1]&0xF0)>>4);
      packet_LC75824[2].data[4]|=((FONT[chr-0x20][1]&0x0F)<<4);
    break;
    case 6:
      packet_LC75824[2].data[4]|=((FONT[chr-0x20][0]&0xF0)>>4);
      packet_LC75824[2].data[5]|=((FONT[chr-0x20][0]&0x0F)<<4);
      packet_LC75824[2].data[5]|=((FONT[chr-0x20][1]&0xF0)>>4);
      packet_LC75824[2].data[6]|=((FONT[chr-0x20][1]&0x0F)<<4);
    break;
    case 7:
      packet_LC75824[3].data[0]|=FONT[chr-0x20][0];
      packet_LC75824[3].data[1]|=FONT[chr-0x20][1];
    break;
    case 8:
      packet_LC75824[3].data[2]|=FONT[chr-0x20][0];
      packet_LC75824[3].data[3]|=FONT[chr-0x20][1];
    break;
    case 9:
      packet_LC75824[3].data[4]|=FONT[chr-0x20][0];
      packet_LC75824[3].data[5]|=FONT[chr-0x20][1];
    break;
  }
}

void DRX7375R_clear_char(int pos) {

  switch(pos){
    case 0:
      packet_LC75824[0].data[3]&=DIGIT_CLEAR1;
      packet_LC75824[0].data[4]&=DIGIT_CLEAR2;
    break;
    case 1:
      packet_LC75824[0].data[5]&=DIGIT_CLEAR1;
      packet_LC75824[0].data[6]&=DIGIT_CLEAR2_HIGH;
      packet_LC75824[1].data[0]&=((DIGIT_CLEAR2_LOW<<4)|0x0F);
    break;
    case 2:
      packet_LC75824[1].data[0]&=((DIGIT_CLEAR1_HIGH>>4)|0xF0);
      packet_LC75824[1].data[1]&=((DIGIT_CLEAR1_LOW<<4)|0x0F);
      packet_LC75824[1].data[1]&=((DIGIT_CLEAR2_HIGH>>4)|0xF0);
      packet_LC75824[1].data[2]&=((DIGIT_CLEAR2_LOW<<4)|0x0F);
    break;
    case 3:
      packet_LC75824[1].data[4]&=((DIGIT_CLEAR1_HIGH>>4)|0xF0);
      packet_LC75824[1].data[5]&=((DIGIT_CLEAR1_LOW<<4)|0x0F);
      packet_LC75824[1].data[5]&=((DIGIT_CLEAR2_HIGH>>4)|0xF0);
      packet_LC75824[1].data[6]&=((DIGIT_CLEAR2_LOW<<4)|0x0F);
    break;
    case 4:
      packet_LC75824[2].data[0]&=DIGIT_CLEAR1;
      packet_LC75824[2].data[1]&=DIGIT_CLEAR2;
    break;
    case 5:
      packet_LC75824[2].data[2]&=((DIGIT_CLEAR1_HIGH>>4)|0xF0);
      packet_LC75824[2].data[3]&=((DIGIT_CLEAR1_LOW<<4)|0x0F);
      packet_LC75824[2].data[3]&=((DIGIT_CLEAR2_HIGH>>4)|0xF0);
      packet_LC75824[2].data[4]&=((DIGIT_CLEAR2_LOW<<4)|0x0F);
    break;
    case 6:
      packet_LC75824[2].data[4]&=((DIGIT_CLEAR1_HIGH>>4)|0xF0);
      packet_LC75824[2].data[5]&=((DIGIT_CLEAR1_LOW<<4)|0x0F);
      packet_LC75824[2].data[5]&=((DIGIT_CLEAR2_HIGH>>4)|0xF0);
      packet_LC75824[2].data[6]&=((DIGIT_CLEAR2_LOW<<4)|0x0F);
    break;
    case 7:
      packet_LC75824[3].data[0]&=DIGIT_CLEAR1;
      packet_LC75824[3].data[1]&=DIGIT_CLEAR2;
    break;
    case 8:
      packet_LC75824[3].data[2]&=DIGIT_CLEAR1;
      packet_LC75824[3].data[3]&=DIGIT_CLEAR2;
    break;
    case 9:
      packet_LC75824[3].data[4]&=DIGIT_CLEAR1;
      packet_LC75824[3].data[5]&=DIGIT_CLEAR2;
    break;
  }
}

void DRX7375R_clear_text(uint32_t flush) {

  uint32_t i;

  for(i=0;i<10;i++){
    DRX7375R_clear_char(i);
  }
  if(flush){
    DRX7375R_send_LC75824();
  }
}

void DRX7375R_fill_symbols(uint32_t flush) {

  DRX7375R_fill_symbols_LC75854(flush);
  DRX7375R_fill_symbols_LC75824(flush);
}

void DRX7375R_fill_symbols_LC75854(uint32_t flush) {

  uint32_t i;

  for(i=0;i<32;i++) {
    packet_LC75854[(symbols_a[i]&0xF000)>>12].data[(symbols_a[i]&0x0F00)>>8]|=(symbols_a[i]&0x00FF);
  }

  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_fill_all_dots(uint8_t mask,uint32_t flush) {

  uint32_t i;
  for(i=0;i<18;i++) {
    DRX7375R_fill_dots(i,mask,FALSE);
  }
  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_fill_dots(uint32_t column,uint8_t mask,uint32_t flush) {

  switch(column){
    case 0:
      packet_LC75854[0].data[2]|=(mask&0xF0);
      if(mask&0x08) packet_LC75854[0].data[2]|=0x01;
      if(mask&0x04) packet_LC75854[0].data[2]|=0x02;
      if(mask&0x02) packet_LC75854[0].data[2]|=0x04;
    break;
    case 1:
      packet_LC75854[0].data[3]|=(mask&0xF0);
      if(mask&0x08) packet_LC75854[0].data[3]|=0x01;
      if(mask&0x04) packet_LC75854[0].data[3]|=0x02;
      if(mask&0x02) packet_LC75854[0].data[3]|=0x04;
    break;
    case 2:
      packet_LC75854[0].data[4]|=(mask&0xF0);
      if(mask&0x08) packet_LC75854[0].data[4]|=0x01;
      if(mask&0x04) packet_LC75854[0].data[4]|=0x02;
      if(mask&0x02) packet_LC75854[0].data[4]|=0x04;
    break;
    case 3:
      packet_LC75854[3].data[3]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[4]|=0x10;
      if(mask&0x04) packet_LC75854[3].data[4]|=0x20;
      if(mask&0x02) packet_LC75854[3].data[4]|=0x40;
    break;
    case 4:
      packet_LC75854[3].data[2]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[3]|=0x10;
      if(mask&0x04) packet_LC75854[3].data[3]|=0x20;
      if(mask&0x02) packet_LC75854[3].data[3]|=0x40;
    break;
    case 5:
      packet_LC75854[3].data[1]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[2]|=0x10;
      if(mask&0x04) packet_LC75854[3].data[2]|=0x20;
      if(mask&0x02) packet_LC75854[3].data[2]|=0x40;
    break;
    case 6:
      packet_LC75854[3].data[0]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[1]|=0x10;
      if(mask&0x04) packet_LC75854[3].data[1]|=0x20;
      if(mask&0x02) packet_LC75854[3].data[1]|=0x40;
    break;
    case 7:
      packet_LC75854[2].data[4]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[0]|=0x10;
      if(mask&0x04) packet_LC75854[3].data[0]|=0x20;
      if(mask&0x02) packet_LC75854[3].data[0]|=0x40;
    break;
    case 8:
      packet_LC75854[2].data[3]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[4]|=0x10;
      if(mask&0x04) packet_LC75854[2].data[4]|=0x20;
      if(mask&0x02) packet_LC75854[2].data[4]|=0x40;
    break;
    case 9:
      packet_LC75854[2].data[2]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[3]|=0x10;
      if(mask&0x04) packet_LC75854[2].data[3]|=0x20;
      if(mask&0x02) packet_LC75854[2].data[3]|=0x40;
    break;
    case 10:
      packet_LC75854[2].data[1]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[2]|=0x10;
      if(mask&0x04) packet_LC75854[2].data[2]|=0x20;
      if(mask&0x02) packet_LC75854[2].data[2]|=0x40;
    break;
    case 11:
      packet_LC75854[0].data[5]|=(mask&0xF0);
      if(mask&0x08) packet_LC75854[1].data[0]|=0x10;
      if(mask&0x04) packet_LC75854[1].data[0]|=0x20;
      if(mask&0x02) packet_LC75854[1].data[0]|=0x40;
    break;
    case 12:
      packet_LC75854[1].data[0]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[1].data[1]|=0x10;
      if(mask&0x04) packet_LC75854[1].data[1]|=0x20;
      if(mask&0x02) packet_LC75854[1].data[1]|=0x40;
    break;
    case 13:
      packet_LC75854[1].data[1]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[1].data[2]|=0x10;
      if(mask&0x04) packet_LC75854[1].data[2]|=0x20;
      if(mask&0x02) packet_LC75854[1].data[2]|=0x40;
    break;
    case 14:
      packet_LC75854[1].data[2]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[1].data[3]|=0x10;
      if(mask&0x04) packet_LC75854[1].data[3]|=0x20;
      if(mask&0x02) packet_LC75854[1].data[3]|=0x40;
    break;
    case 15:
      packet_LC75854[1].data[3]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[1].data[4]|=0x10;
      if(mask&0x04) packet_LC75854[1].data[4]|=0x20;
      if(mask&0x02) packet_LC75854[1].data[4]|=0x40;
    break;
    case 16:
      packet_LC75854[1].data[4]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[0]|=0x10;
      if(mask&0x04) packet_LC75854[2].data[0]|=0x20;
      if(mask&0x02) packet_LC75854[2].data[0]|=0x40;
    break;
    case 17:
      packet_LC75854[2].data[0]|=((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[1]|=0x10;
      if(mask&0x04) packet_LC75854[2].data[1]|=0x20;
      if(mask&0x02) packet_LC75854[2].data[1]|=0x40;
    break;
  }
  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_clear_all_dots(uint8_t mask,uint32_t flush) {

  uint32_t i;

  for(i=0;i<18;i++) {
    DRX7375R_clear_dots(i,mask,FALSE);
  }
  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_clear_dots(uint32_t column,uint8_t mask,uint32_t flush) {

  switch(column){
    case 0:
      packet_LC75854[0].data[2]&=~(mask&0xF0);
      if(mask&0x08) packet_LC75854[0].data[2]&=~0x01;
      if(mask&0x04) packet_LC75854[0].data[2]&=~0x02;
      if(mask&0x02) packet_LC75854[0].data[2]&=~0x04;
    break;
    case 1:
      packet_LC75854[0].data[3]&=~(mask&0xF0);
      if(mask&0x08) packet_LC75854[0].data[3]&=~0x01;
      if(mask&0x04) packet_LC75854[0].data[3]&=~0x02;
      if(mask&0x02) packet_LC75854[0].data[3]&=~0x04;
    break;
    case 2:
      packet_LC75854[0].data[4]&=~(mask&0xF0);
      if(mask&0x08) packet_LC75854[0].data[4]&=~0x01;
      if(mask&0x04) packet_LC75854[0].data[4]&=~0x02;
      if(mask&0x02) packet_LC75854[0].data[4]&=~0x04;
    break;
    case 3:
      packet_LC75854[3].data[3]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[4]&=~0x10;
      if(mask&0x04) packet_LC75854[3].data[4]&=~0x20;
      if(mask&0x02) packet_LC75854[3].data[4]&=~0x40;
    break;
    case 4:
      packet_LC75854[3].data[2]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[3]&=~0x10;
      if(mask&0x04) packet_LC75854[3].data[3]&=~0x20;
      if(mask&0x02) packet_LC75854[3].data[3]&=~0x40;
    break;
    case 5:
      packet_LC75854[3].data[1]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[2]&=~0x10;
      if(mask&0x04) packet_LC75854[3].data[2]&=~0x20;
      if(mask&0x02) packet_LC75854[3].data[2]&=~0x40;
    break;
    case 6:
      packet_LC75854[3].data[0]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[1]&=~0x10;
      if(mask&0x04) packet_LC75854[3].data[1]&=~0x20;
      if(mask&0x02) packet_LC75854[3].data[1]&=~0x40;
    break;
    case 7:
      packet_LC75854[2].data[4]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[3].data[0]&=~0x10;
      if(mask&0x04) packet_LC75854[3].data[0]&=~0x20;
      if(mask&0x02) packet_LC75854[3].data[0]&=~0x40;
    break;
    case 8:
      packet_LC75854[2].data[3]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[4]&=~0x10;
      if(mask&0x04) packet_LC75854[2].data[4]&=~0x20;
      if(mask&0x02) packet_LC75854[2].data[4]&=~0x40;
    break;
    case 9:
      packet_LC75854[2].data[2]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[3]&=~0x10;
      if(mask&0x04) packet_LC75854[2].data[3]&=~0x20;
      if(mask&0x02) packet_LC75854[2].data[3]&=~0x40;
    break;
    case 10:
      packet_LC75854[2].data[1]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[2]&=~0x10;
      if(mask&0x04) packet_LC75854[2].data[2]&=~0x20;
      if(mask&0x02) packet_LC75854[2].data[2]&=~0x40;
    break;
    case 11:
      packet_LC75854[0].data[5]&=~(mask&0xF0);
      if(mask&0x08) packet_LC75854[1].data[0]&=~0x10;
      if(mask&0x04) packet_LC75854[1].data[0]&=~0x20;
      if(mask&0x02) packet_LC75854[1].data[0]&=~0x40;
    break;
    case 12:
      packet_LC75854[1].data[0]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[1].data[1]&=~0x10;
      if(mask&0x04) packet_LC75854[1].data[1]&=~0x20;
      if(mask&0x02) packet_LC75854[1].data[1]&=~0x40;
    break;
    case 13:
      packet_LC75854[1].data[1]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[1].data[2]&=~0x10;
      if(mask&0x04) packet_LC75854[1].data[2]&=~0x20;
      if(mask&0x02) packet_LC75854[1].data[2]&=~0x40;
    break;
    case 14:
      packet_LC75854[1].data[2]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[1].data[3]&=~0x10;
      if(mask&0x04) packet_LC75854[1].data[3]&=~0x20;
      if(mask&0x02) packet_LC75854[1].data[3]&=~0x40;
    break;
    case 15:
      packet_LC75854[1].data[3]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[1].data[4]&=~0x10;
      if(mask&0x04) packet_LC75854[1].data[4]&=~0x20;
      if(mask&0x02) packet_LC75854[1].data[4]&=~0x40;
    break;
    case 16:
      packet_LC75854[1].data[4]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[0]&=~0x10;
      if(mask&0x04) packet_LC75854[2].data[0]&=~0x20;
      if(mask&0x02) packet_LC75854[2].data[0]&=~0x40;
    break;
    case 17:
      packet_LC75854[2].data[0]&=~((mask>>4)&0x0F);
      if(mask&0x08) packet_LC75854[2].data[1]&=~0x10;
      if(mask&0x04) packet_LC75854[2].data[1]&=~0x20;
      if(mask&0x02) packet_LC75854[2].data[1]&=~0x40;
    break;
  }
  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_fill_symbols_LC75824(uint32_t flush) {

  uint32_t i;

  for(i=0;i<66;i++) {
    packet_LC75824[(symbols_b[i]&0xF000)>>12].data[(symbols_b[i]&0x0F00)>>8]|=(symbols_b[i]&0x00FF);
  }

  if(flush){
    DRX7375R_send_LC75824();
  }
}

void DRX7375R_fill_symbol_nr_LC75854(uint32_t nr,uint32_t on,uint32_t flush) {

  if(on) {
    packet_LC75854[(symbols_a[nr]&0xF000)>>12].data[(symbols_a[nr]&0x0F00)>>8]|=(symbols_a[nr]&0x00FF);
  }
  else {
    packet_LC75854[(symbols_a[nr]&0xF000)>>12].data[(symbols_a[nr]&0x0F00)>>8]&=~(symbols_a[nr]&0x00FF);
  }

  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_fill_symbol_nr_LC75824(uint32_t nr,uint32_t on,uint32_t flush) {

  if(on) {
    packet_LC75824[(symbols_b[nr]&0xF000)>>12].data[(symbols_b[nr]&0x0F00)>>8]|=(symbols_b[nr]&0x00FF);
  }
  else {
    packet_LC75824[(symbols_b[nr]&0xF000)>>12].data[(symbols_b[nr]&0x0F00)>>8]&=~(symbols_b[nr]&0x00FF);
  }

  if(flush){
    DRX7375R_send_LC75824();
  }
}

void DRX7375R_fill_symbol_val_LC75854(uint32_t val,uint32_t on,uint32_t flush) {

  if(on) {
    packet_LC75854[(val&0xF000)>>12].data[(val&0x0F00)>>8]|=(val&0x00FF);
  }
  else {
    packet_LC75854[(val&0xF000)>>12].data[(val&0x0F00)>>8]&=~(val&0x00FF);
  }

  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_fill_symbol_val_LC75824(uint32_t val,uint32_t on,uint32_t flush) {

  if(on) {
    packet_LC75824[(val&0xF000)>>12].data[(val&0x0F00)>>8]|=(val&0x00FF);
  }
  else {
    packet_LC75824[(val&0xF000)>>12].data[(val&0x0F00)>>8]&=~(val&0x00FF);
  }

  if(flush){
    DRX7375R_send_LC75824();
  }
}

void DRX7375R_clear_symbols(uint32_t flush) {

  DRX7375R_clear_symbols_LC75854(flush);
  DRX7375R_clear_symbols_LC75824(flush);
}

void DRX7375R_clear_symbols_LC75854(uint32_t flush) {

  uint32_t i;

  for(i=0;i<32;i++) {
    packet_LC75854[(symbols_a[i]&0xF000)>>12].data[(symbols_a[i]&0x0F00)>>8]&=~(symbols_a[i]&0x00FF);
  }

  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_clear_symbols_LC75824(uint32_t flush) {

  uint32_t i;

  for(i=0;i<66;i++) {
    packet_LC75824[(symbols_b[i]&0xF000)>>12].data[(symbols_b[i]&0x0F00)>>8]&=~(symbols_b[i]&0x00FF);
  }

  if(flush){
    DRX7375R_send_LC75824();
  }
}

void DRX7375R_ring(uint16_t on,uint32_t flush) {

  if(on) {
    packet_LC75854[0].data[0]=on>>8;
    packet_LC75854[0].data[1]=on;
  }
  else {
    packet_LC75854[0].data[0]=0x00;
    packet_LC75854[0].data[1]=0x00;
  }
  if(flush){
    DRX7375R_send_LC75854();
  }
}

void DRX7375R_segments(uint32_t on) {

  if(on) {
    packet_LC75854[0].data[6]&=~SEGMENTS_ONOFF_CONTROL_DATA_LC75854;
    packet_LC75824[0].data[7]&=~SEGMENTS_ONOFF_CONTROL_DATA_LC75824;
  }
  else {
    packet_LC75854[0].data[6]|=SEGMENTS_ONOFF_CONTROL_DATA_LC75854;
    packet_LC75824[0].data[7]|=SEGMENTS_ONOFF_CONTROL_DATA_LC75824;
  }
}

void DRX7375R_sleep(uint32_t on) {

  if(on) {
    packet_LC75854[0].data[5]|=SLEEP_CONTROL_DATA_LC75854;
    packet_LC75824[0].data[7]|=SLEEP_CONTROL_DATA_LC75824;
  }
  else {
    packet_LC75854[0].data[5]&=~SLEEP_CONTROL_DATA_LC75854;
    packet_LC75824[0].data[7]&=~SLEEP_CONTROL_DATA_LC75824;
  }
}
