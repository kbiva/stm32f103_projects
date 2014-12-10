/*
 *  mdx7900r.c
 *
 *  Sony MDX-C7900R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "mdx7900r.h"

static uint8_t data[32];
static uint8_t symbols_set[8];

static PIN pins[]={
  {{CLK_Pin,CLK_Speed,CLK_Mode},CLK_Port,CLK_Bus},
  {{DO_Pin,DO_Speed,DO_Mode},DO_Port,DO_Bus},
  {{CE_Pin, CE_Speed, CE_Mode}, CE_Port, CE_Bus},
  {{KEYIN0_Pin, KEYIN0_Speed, KEYIN0_Mode}, KEYIN0_Port, KEYIN0_Bus},
  {{KEYIN1_Pin, KEYIN1_Speed, KEYIN1_Mode}, KEYIN1_Port, KEYIN1_Bus},
  {{DBASS_Pin, DBASS_Speed, DBASS_Mode}, DBASS_Port, DBASS_Bus},
  {{VOLMINUS_Pin, VOLMINUS_Speed, VOLMINUS_Mode}, VOLMINUS_Port, VOLMINUS_Bus},
  {{VOLPLUS_Pin, VOLPLUS_Speed, VOLPLUS_Mode}, VOLPLUS_Port, VOLPLUS_Bus},
};

void GPIO_Configuration(void) {

  uint32_t i;
  SPI_InitTypeDef  SPI_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

  // Pins GPIO configuration
  for(i=0;i<sizeof(pins)/sizeof(PIN);i++) {
    RCC_APB2PeriphClockCmd(pins[i].GPIO_Bus,ENABLE);
    GPIO_Init(pins[i].GPIOx,&pins[i].GPIO_InitStructure);
  }

  // SPI2 configuration
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);

  // Encoder configuration
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

  TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period=1;
  TIM_TimeBaseInitStruct.TIM_Prescaler=0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

  TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);

  NVIC_EnableIRQ(TIM4_IRQn);
}

void MDX7900R_init(void) {

  GPIO_ResetBits(CE_Port,CE_Pin);

  data[0]=CMD_DISPLAY_SETTING|
          CMD_DISPLAY_SETTING_VOLTAGE_INTERNAL|
          CMD_DISPLAY_SETTING_MASTER|
          CMD_DISPLAY_SETTING_1_8_DUTY;
  MDX7900R_send(1);

  data[0]=CMD_STATUS|
          CMD_STATUS_TEST_OFF|
          CMD_STATUS_STANDBY_OFF|
          CMD_STATUS_KEY_SCAN_CONTROL_STOP|
          CMD_STATUS_LED_OFF|
          CMD_STATUS_LCD_OFF;
  MDX7900R_send(1);

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_FIXED|
          CMD_DATA_SETTING_WRITE_LED_OUTPUT_LATCH;
  data[1]=LEDS_OFF;
  MDX7900R_send(2);

  data[0]=CMD_STATUS|
          CMD_STATUS_TEST_OFF|
          CMD_STATUS_STANDBY_OFF|
          CMD_STATUS_KEY_SCAN_CONTROL_STOP|
          CMD_STATUS_LED_ON|
          CMD_STATUS_LCD_ON;
  MDX7900R_send(1);
}

void MDX7900R_backlight(BACKLIGHT val) {

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_FIXED|
          CMD_DATA_SETTING_WRITE_LED_OUTPUT_LATCH;
  data[1]=val;
  MDX7900R_send(2);
}

static void sendByte(uint8_t byte) {

  SPI_I2S_SendData(SPI2,byte);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET){};
}

void MDX7900R_send(uint8_t length) {

  uint8_t i;

  GPIO_SetBits(CE_Port,CE_Pin);

  for(i=0;i<length;i++) {
    sendByte(data[i]);
  }
  GPIO_ResetBits(CE_Port,CE_Pin);
}

void MDX7900R_text(uint8_t pos,char* text,uint8_t clear) {

  uint8_t i=1;

  if(clear) {
    MDX7900R_clear_text(pos);
  }

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_INCREMENT|
          CMD_DATA_SETTING_WRITE_DISPLAY_DATA_RAM;
  MDX7900R_send(1);
  data[0]=CMD_ADDRESS_SETTING+pos;
  while(*text) {
    data[i++]=*text++;
  }
  MDX7900R_send(i);
}

void MDX7900R_clear_text(uint8_t start) {

  uint8_t i;

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_INCREMENT|
          CMD_DATA_SETTING_WRITE_DISPLAY_DATA_RAM;
  MDX7900R_send(1);

  data[0]=CMD_ADDRESS_SETTING+start;
  for(i=1;i<14-start;i++) {
    data[i]=' ';
  }
  MDX7900R_send(14-start);
}

void MDX7900R_clear_symbols(void) {

  uint8_t i;

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_INCREMENT|
          CMD_DATA_SETTING_WRITE_CHARACTER_DISPLAY;
  MDX7900R_send(1);
  data[0]=CMD_ADDRESS_SETTING;
  for(i=1;i<9;i++) {
    data[i]=0x00;
    symbols_set[i-1]=0;
  }
  MDX7900R_send(9);
}

void MDX7900R_create_char(uint8_t pos,uint8_t* character) {

  uint8_t i;

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_FIXED|
          CMD_DATA_SETTING_WRITE_CGRAM;
  MDX7900R_send(1);
  data[0]=CMD_ADDRESS_SETTING+pos;
  for(i=1;i<8;i++) {
    data[i]=*character++;
  }
  MDX7900R_send(8);
}

void MDX7900R_symbol(uint16_t symbol,uint8_t on) {

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_INCREMENT|
          CMD_DATA_SETTING_WRITE_CHARACTER_DISPLAY;
  MDX7900R_send(1);
  data[0]=CMD_ADDRESS_SETTING+(symbol>>8);
  if(on) {
    data[1]=symbols_set[symbol>>8]|symbol;
    symbols_set[symbol>>8]|=symbol;
  }
  else {
    data[1]=symbols_set[symbol>>8]&~symbol;
    symbols_set[symbol>>8]&=~symbol;
  }
  MDX7900R_send(2);
}

void MDX7900R_fill_symbols(void) {

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_INCREMENT|
          CMD_DATA_SETTING_WRITE_CHARACTER_DISPLAY;
  MDX7900R_send(1);
  data[0]=CMD_ADDRESS_SETTING;

  data[1]=(uint8_t)(SYMBOL_DOUBLE_COLON|SYMBOL_1|SYMBOL_2);
  symbols_set[0]=(uint8_t)(SYMBOL_DOUBLE_COLON|SYMBOL_1|SYMBOL_2);
  data[2]=(uint8_t)(SYMBOL_BIG1|SYMBOL_REG|SYMBOL_ST|SYMBOL_EON|SYMBOL_TP|SYMBOL_TA|SYMBOL_LCL|SYMBOL_AF);
  symbols_set[1]=(uint8_t)(SYMBOL_BIG1|SYMBOL_REG|SYMBOL_ST|SYMBOL_EON|SYMBOL_TP|SYMBOL_TA|SYMBOL_LCL|SYMBOL_AF);
  data[3]=(uint8_t)(SYMBOL_ARROW_LEFT|SYMBOL_BANK|SYMBOL_DISC);
  symbols_set[2]=(uint8_t)(SYMBOL_ARROW_LEFT|SYMBOL_BANK|SYMBOL_DISC);
  data[4]=(uint8_t)(SYMBOL_PLAYMODE|SYMBOL_SETUP);
  symbols_set[3]=(uint8_t)(SYMBOL_PLAYMODE|SYMBOL_SETUP);
  data[5]=(uint8_t)(SYMBOL_2REP|SYMBOL_1REP|SYMBOL_REP_TOP|SYMBOL_MONO|SYMBOL_TRACK|SYMBOL_ARROW_RIGHT);
  symbols_set[4]=(uint8_t)(SYMBOL_2REP|SYMBOL_1REP|SYMBOL_REP_TOP|SYMBOL_MONO|SYMBOL_TRACK|SYMBOL_ARROW_RIGHT);
  data[6]=(uint8_t)(SYMBOL_ALL|SYMBOL_1SHUF|SYMBOL_SHUF_TOP);
  symbols_set[5]=(uint8_t)(SYMBOL_ALL|SYMBOL_1SHUF|SYMBOL_SHUF_TOP);
  data[7]=(uint8_t)(SYMBOL_REP_BOTTOM|SYMBOL_ENTER);
  symbols_set[6]=(uint8_t)(SYMBOL_REP_BOTTOM|SYMBOL_ENTER);
  data[8]=(uint8_t)(SYMBOL_DBASS|SYMBOL_DIGITAL|SYMBOL_SHUF_BOTTOM);
  symbols_set[7]=(uint8_t)(SYMBOL_DBASS|SYMBOL_DIGITAL|SYMBOL_SHUF_BOTTOM);

  MDX7900R_send(9);
}

void MDX7900R_symbol_1(void) {

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_FIXED|
          CMD_DATA_SETTING_WRITE_CHARACTER_DISPLAY;
  MDX7900R_send(1);
  data[0]=CMD_ADDRESS_SETTING;
  data[1]=SYMBOL_1;
  MDX7900R_send(2);
}

void MDX7900R_symbol_2(void) {

  data[0]=CMD_DATA_SETTING|
          CMD_DATA_SETTING_ADDRESS_FIXED|
          CMD_DATA_SETTING_WRITE_CHARACTER_DISPLAY;
  MDX7900R_send(1);
  data[0]=CMD_ADDRESS_SETTING;
  data[1]=SYMBOL_2;
  MDX7900R_send(2);
}
