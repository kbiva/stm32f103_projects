/*
 *  main.c
 *
 *  Kenwood KDC-KDC_5060R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "KDC_5060R.h"

uint8_t symbols[]={
 SYMBOL_DAB,SYMBOL_AUTO,SYMBOL_2,SYMBOL_CLOCK,SYMBOL_RDS,SYMBOL_PTY,SYMBOL_B_NR,SYMBOL_TP,SYMBOL_REG,SYMBOL_CH,
 SYMBOL_ATT,SYMBOL_TI,SYMBOL_LOUD,SYMBOL_ST,SYMBOL_1,SYMBOL_DOUBLE_COLON_2,SYMBOL_TA,SYMBOL_1_LEFT,SYMBOL_1_V,SYMBOL_1_BOTTOM,
 SYMBOL_1_RIGHT,SYMBOL_1_TOP_MIDDLE,SYMBOL_2_V_BOTTOM,SYMBOL_2_LEFT_RIGHT,SYMBOL_2_V_TOP,SYMBOL_TRIANGLE1,SYMBOL_II,SYMBOL_I,
 SYMBOL_IN,SYMBOL_TRIANGLE2,SYMBOL_MTL,SYMBOL_NEWS,SYMBOL_DUAL,SYMBOL_DOUBLE_COLON_1,SYMBOL_MINUS,SYMBOL_CHANGER
};

extern volatile uint16_t ADCConvertedValue[];
extern uint8_t key_data[];

void demo1(int delay,int on,int flush) {

  uint8_t i;

  for(i=0;i<sizeof(symbols)/sizeof(uint8_t);i++) {
    KDC_5060R_symbol(symbols[i],on,flush);
    DWT_Delay(delay);
  }
  if(on) {
    KDC_5060R_digit(8,flush);
  }
  else {
    KDC_5060R_digit_off(flush);
  }
}

int main(void) {

  int i;

  DWT_Init();

  KDC_5060R_init();

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  DWT_Delay(1000000);
  demo1(0,TRUE,FALSE);
  KDC_5060R_text_all(TRUE,TRUE);
  DWT_Delay(5000000);
  demo1(0,FALSE,FALSE);
  KDC_5060R_text_all(FALSE,TRUE);
  DWT_Delay(1000000);
  demo1(0,TRUE,FALSE);
  KDC_5060R_text_all(TRUE,TRUE);
  DWT_Delay(1000000);
  demo1(0,FALSE,FALSE);
  KDC_5060R_text_all(FALSE,TRUE);
  DWT_Delay(1000000);
  demo1(0,TRUE,FALSE);
  KDC_5060R_text_all(TRUE,TRUE);
  DWT_Delay(1000000);
  demo1(0,FALSE,FALSE);
  KDC_5060R_text_all(FALSE,TRUE);
  DWT_Delay(1000000);

  for(i=0;i<10;i++) {
    KDC_5060R_digit(i,TRUE);
    DWT_Delay(200000);
  }
  KDC_5060R_digit_off(TRUE);

  KDC_5060R_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(1000000);

  demo1(100000,TRUE,TRUE);
  DWT_Delay(1000000);
  demo1(100000,FALSE,TRUE);

  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  KDC_5060R_send();

  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  KDC_5060R_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  KDC_5060R_send();
  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  KDC_5060R_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  KDC_5060R_send();
  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  KDC_5060R_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  KDC_5060R_send();
  DWT_Delay(500000);
  KDC_5060R_clear_text(TRUE);
  DWT_Delay(500000);

  KDC_5060R_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);
  KDC_5060R_clear_text(TRUE);
  DWT_Delay(500000);
  KDC_5060R_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);
  KDC_5060R_clear_text(TRUE);
  DWT_Delay(500000);
  KDC_5060R_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);

  KDC_5060R_symbol(SYMBOL_DOUBLE_COLON_1,FALSE,FALSE);
  KDC_5060R_symbol(SYMBOL_DOUBLE_COLON_2,FALSE,FALSE);
  KDC_5060R_symbol(SYMBOL_MINUS,FALSE,TRUE);

  KDC_5060R_text("kbiva.wordpress.com",0,TRUE,TRUE);
  DWT_Delay(2000000);

  for(i=0;i>=-11;i--) {
    KDC_5060R_text("kbiva.wordpress.com",i,TRUE,TRUE);
    DWT_Delay(200000);
  }
  DWT_Delay(1000000);
  for(i=-11;i<=0;i++) {
    KDC_5060R_text("kbiva.wordpress.com",i,TRUE,TRUE);
    DWT_Delay(200000);
  }
  DWT_Delay(2000000);

  demo1(0,FALSE,FALSE);
  KDC_5060R_clear_text(TRUE);

  while(1){

    KDC_5060R_receive();

    if(key_data[0]&KEY_3) {KDC_5060R_text("red",0,TRUE,TRUE);KDC_5060R_leds_red();}
    if(key_data[0]&KEY_2) KDC_5060R_text("Button 2",0,TRUE,TRUE);

    if(key_data[1]&KEY_1) KDC_5060R_text("Button 1",0,TRUE,TRUE);
    if(key_data[1]&KEY_4) {KDC_5060R_text("green",0,TRUE,TRUE);KDC_5060R_leds_green();}
    if(key_data[1]&KEY_VOL_UP) KDC_5060R_text("Vol Up",0,TRUE,TRUE);
    if(key_data[1]&KEY_VOL_DOWN) KDC_5060R_text("Vol Down",0,TRUE,TRUE);
    if(key_data[1]&KEY_ATT) KDC_5060R_text("Att",0,TRUE,TRUE);
    if(key_data[1]&KEY_5) {KDC_5060R_text("yellow",0,TRUE,TRUE);KDC_5060R_leds_yellow();}
    if(key_data[1]&KEY_6) {KDC_5060R_text("leds off",0,TRUE,TRUE);KDC_5060R_leds_off();}

    if(key_data[2]&KEY_PLAY) KDC_5060R_text("Play",0,TRUE,TRUE);
    if(key_data[2]&KEY_SRC) KDC_5060R_text("SRC",0,TRUE,TRUE);
    if(key_data[2]&KEY_SEEK_RIGHT) KDC_5060R_text("Seek >>",0,TRUE,TRUE);
    if(key_data[2]&KEY_DISP) KDC_5060R_text("DISP",0,TRUE,TRUE);
    if(key_data[2]&KEY_SEEK_LEFT) KDC_5060R_text("Seek <<",0,TRUE,TRUE);
    if(key_data[2]&KEY_FM) KDC_5060R_text("FM",0,TRUE,TRUE);
    if(key_data[2]&KEY_AM) KDC_5060R_text("AM",0,TRUE,TRUE);

    if(key_data[3]&KEY_TI) KDC_5060R_text("TI",0,TRUE,TRUE);
    if(key_data[3]&KEY_AUDIO) KDC_5060R_text("AUDIO",0,TRUE,TRUE);

    if(ADCConvertedValue[0]<2000) KDC_5060R_text("RESET",0,TRUE,TRUE);

    DWT_Delay(100000);
    KDC_5060R_clear_text(TRUE);
  }
}
