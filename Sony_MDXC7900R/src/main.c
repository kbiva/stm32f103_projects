/*
 *  main.c
 *
 *  Sony MDX-C7900R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "mdx7900r.h"
#include "adc.h"
#include <stdio.h>

extern volatile uint8_t capture_is_ready;
extern volatile Volume_Direction captured_direction;

uint8_t flip,cleared,dbass;
volatile uint16_t val0,val1,val2;

extern volatile uint16_t ADCConvertedValue[];

char bufa[10];

uint8_t char1[7]={0x1F,0x31,0x51,0x71,0x91,0xB1,0xDF}; // rectangle
uint8_t char2[7]={0x00,0x2E,0x4E,0x6E,0x8E,0xAE,0xC0}; // filled rectangle
uint8_t char3[7]={0x15,0x2A,0x55,0x6A,0x95,0xAA,0xD5}; // checkerboard1
uint8_t char4[7]={0x0A,0x35,0x4A,0x75,0x8A,0xB5,0xCA}; // checkerboard2
uint8_t char5[7]={0x00,0x2A,0x55,0x71,0x8A,0xA4,0xC0}; // empty heart
uint8_t char6[7]={0x00,0x2A,0x5F,0x7F,0x8E,0xA4,0xC0}; // filled heart
uint8_t char7[7]={0x1F,0x3F,0x5F,0x7F,0x9F,0xBF,0xDF}; // all pixels 5x7

uint16_t symbols[29]={
  SYMBOL_DOUBLE_COLON,SYMBOL_1|SYMBOL_2,
  SYMBOL_BIG1,SYMBOL_REG,SYMBOL_ST,SYMBOL_EON,SYMBOL_TP,SYMBOL_TA,SYMBOL_LCL,SYMBOL_AF,
  SYMBOL_ARROW_LEFT,SYMBOL_BANK,SYMBOL_DISC,
  SYMBOL_PLAYMODE,SYMBOL_SETUP,
  SYMBOL_2REP,SYMBOL_1REP,SYMBOL_REP_TOP,SYMBOL_MONO,SYMBOL_TRACK,SYMBOL_ARROW_RIGHT,
  SYMBOL_ALL,SYMBOL_1SHUF,SYMBOL_SHUF_TOP,SYMBOL_REP_BOTTOM,SYMBOL_ENTER,
  SYMBOL_DBASS,SYMBOL_DIGITAL,SYMBOL_SHUF_BOTTOM
};

uint16_t symbols1[29]={
  SYMBOL_1|SYMBOL_2,
  SYMBOL_DOUBLE_COLON,SYMBOL_DISC,SYMBOL_BANK,SYMBOL_TRACK,SYMBOL_MONO,
  SYMBOL_REP_TOP,SYMBOL_1REP,SYMBOL_2REP,SYMBOL_SHUF_TOP,SYMBOL_1SHUF,SYMBOL_ALL,SYMBOL_DIGITAL,SYMBOL_DBASS,
  SYMBOL_SHUF_BOTTOM,SYMBOL_REP_BOTTOM,SYMBOL_ENTER,SYMBOL_ARROW_RIGHT,SYMBOL_PLAYMODE,SYMBOL_SETUP,SYMBOL_ARROW_LEFT,
  SYMBOL_LCL,SYMBOL_EON,SYMBOL_REG,SYMBOL_BIG1,SYMBOL_ST,SYMBOL_TP,SYMBOL_TA,SYMBOL_AF
};

int main(void) {

  int8_t i;
  uint8_t val;

  DWT_Init();

  GPIO_Configuration();

  ADC_DMA_Configuration();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  MDX7900R_init();
  MDX7900R_backlight(LEDS_AMBER);

  MDX7900R_clear_text(0);
  MDX7900R_clear_symbols();

  MDX7900R_create_char(0,char1);
  MDX7900R_create_char(1,char2);
  MDX7900R_create_char(2,char3);
  MDX7900R_create_char(3,char4);
  MDX7900R_create_char(4,char5);
  MDX7900R_create_char(5,char6);
  MDX7900R_create_char(6,char7);

  //MDX7900R_fill_symbols();
  //MDX7900R_text(0,"\x06\x06\x06\x06\x06\x06\x06\x06\x06\x06\x06\x06\x06",1);
  //while(1){};

  MDX7900R_text(0,"Hello World!!",1);
  DWT_Delay(2000000);

  for(i=0;i<13;i++) {
    MDX7900R_text(i,"Hello World!!",1);
    DWT_Delay(300000);
    MDX7900R_clear_text(0);
  }
  for(i=12;i>=0;i--) {
    MDX7900R_text(i,"Hello World!!",0);
    DWT_Delay(300000);
  }

  MDX7900R_text(0,"Hello World!!",1);
  DWT_Delay(2000000);

  MDX7900R_clear_text(0);

  for(i=12;i>=0;i--) {
    MDX7900R_text(i,"Sony MDX-C790",0);
    DWT_Delay(300000);
  }
  MDX7900R_text(0,"ony MDX-C7900R",0);
  DWT_Delay(300000);
  MDX7900R_text(0,"ny MDX-C7900R",0);
  DWT_Delay(3000000);

  MDX7900R_text(0,"   kbiva.word",0);DWT_Delay(1000000);
  MDX7900R_text(0,"  kbiva.wordp",0);DWT_Delay(300000);
  MDX7900R_text(0," kbiva.wordpr",0);DWT_Delay(300000);
  MDX7900R_text(0,"kbiva.wordpre",0);DWT_Delay(300000);
  MDX7900R_text(0,"biva.wordpres",0);DWT_Delay(300000);
  MDX7900R_text(0,"iva.wordpress",0);DWT_Delay(300000);
  MDX7900R_text(0,"va.wordpress.",0);DWT_Delay(300000);
  MDX7900R_text(0,"a.wordpress.c",0);DWT_Delay(300000);
  MDX7900R_text(0,".wordpress.co",0);DWT_Delay(300000);
  MDX7900R_text(0,"wordpress.com",0);DWT_Delay(1000000);
  MDX7900R_text(0,".wordpress.co",0);DWT_Delay(300000);
  MDX7900R_text(0,"a.wordpress.c",0);DWT_Delay(300000);
  MDX7900R_text(0,"va.wordpress.",0);DWT_Delay(300000);
  MDX7900R_text(0,"iva.wordpress",0);DWT_Delay(300000);
  MDX7900R_text(0,"biva.wordpres",0);DWT_Delay(300000);
  MDX7900R_text(0,"kbiva.wordpre",0);DWT_Delay(300000);
  MDX7900R_text(0," kbiva.wordpr",0);DWT_Delay(300000);
  MDX7900R_text(0,"  kbiva.wordp",0);DWT_Delay(300000);
  MDX7900R_text(0,"   kbiva.word",0);

  DWT_Delay(2000000);

  MDX7900R_clear_text(0);
  MDX7900R_text(0,"\x04\x05\x04\x05Hi World\x04",0);
  DWT_Delay(2000000);

  for(i=0;i<29;i++) {
    MDX7900R_symbol(symbols1[i],1);
    if(flip) {
      MDX7900R_text(0,"\x04\x05\x04\x05Hi World\x04",0);
    }
    else {
      MDX7900R_text(0,"\x05\x04\x05\x04Hi World\x05",0);
    }
    flip=!flip;
    DWT_Delay(200000);
  }
  DWT_Delay(2000000);
  for(i=28;i>=0;i--) {
    MDX7900R_symbol(symbols1[i],0);
    if(flip) {
      MDX7900R_text(0,"\x04\x05\x04\x05Hi World\x04",0);
    }
    else {
      MDX7900R_text(0,"\x05\x04\x05\x04Hi World\x05",0);
    }
    flip=!flip;
    DWT_Delay(200000);
  }
  DWT_Delay(2000000);
  MDX7900R_fill_symbols();
  DWT_Delay(1000000);
  MDX7900R_clear_symbols();
  DWT_Delay(1000000);
  MDX7900R_fill_symbols();
  DWT_Delay(1000000);
  MDX7900R_clear_symbols();
  DWT_Delay(1000000);
  MDX7900R_fill_symbols();
  DWT_Delay(1000000);
  MDX7900R_clear_text(0);
  DWT_Delay(1000000);
  MDX7900R_text(0,"\x04\x05\x04\x05Hi World\x04",0);
  DWT_Delay(1000000);
  MDX7900R_clear_text(0);
  DWT_Delay(1000000);
  MDX7900R_text(0,"\x04\x05\x04\x05Hi World\x04",0);
  DWT_Delay(1000000);
  MDX7900R_clear_symbols();
  DWT_Delay(2000000);

  i=0;
  dbass=4;
  val=128;

  while(1) {

    i++;
    if(flip) {
      if(i>20) {
        MDX7900R_symbol_1();
        i=0;
        flip=!flip;
      }
    }
    else {
      if(i>20) {
        MDX7900R_symbol_2();
        i=0;
        flip=!flip;
      }
    }
    val0=ADCConvertedValue[0];
    if(val0<702) {
      if(dbass!=0) {
        MDX7900R_text(0,"DB0LED off",1);
        MDX7900R_backlight(LEDS_OFF);
        dbass=0;
      }
    }
    else if(val0<1643) {
      if(dbass!=1) {
        MDX7900R_text(0,"DB1LED green",1);
        MDX7900R_backlight(LEDS_GREEN);
        dbass=1;
      }
    }
    else if(val0<2638) {
      if(dbass!=2) {
        MDX7900R_text(0,"DB2LED amber",1);
        MDX7900R_backlight(LEDS_AMBER);
        dbass=2;
      }
    }
    else {
      if(dbass!=3) {
        MDX7900R_text(0,"DB3LED yellow",1);
        MDX7900R_backlight(LEDS_YELLOW);
        dbass=3;
      }
    }

    val1=ADCConvertedValue[1];
    val2=ADCConvertedValue[2];
    if((val1<3485)||(val2<3325)) {

      if(val1<180)MDX7900R_text(3,"OFF",1);
      else if(val1<423)MDX7900R_text(3,"SOURCE",1);
      else if(val1<636)MDX7900R_text(3,"-",1);
      else if(val1<866)MDX7900R_text(3,"MODE",1);
      else if(val1<1155)MDX7900R_text(3,"+",1);
      else if(val1<1445)MDX7900R_text(3,"SOUND",1);
      else if(val1<1728)MDX7900R_text(3,"SHIFT",1);
      else if(val1<2050)MDX7900R_text(3,"1",1);
      else if(val1<2388)MDX7900R_text(3,"2",1);
      else if(val1<2715)MDX7900R_text(3,"3",1);
      else if(val1<3485)MDX7900R_text(3,"4",1);

      if(val2<180)MDX7900R_text(3,"EJECT",1);
      else if(val2<423)MDX7900R_text(3,"DSPL",1);
      else if(val2<636)MDX7900R_text(3,"PTY",1);
      else if(val2<866)MDX7900R_text(3,"AF/TA",1);
      else if(val2<1155)MDX7900R_text(3,"10",1);
      else if(val2<1445)MDX7900R_text(3,"9",1);
      else if(val2<1728)MDX7900R_text(3,"8",1);
      else if(val2<2050)MDX7900R_text(3,"7",1);
      else if(val2<2388)MDX7900R_text(3,"6",1);
      else if(val2<3325)MDX7900R_text(3,"5",1);
      cleared=0;
    }
    else {
      if(!cleared) {
        MDX7900R_clear_text(3);
        cleared=1;
      }
    }

    if (capture_is_ready) {

      Volume_Direction direction;
      NVIC_DisableIRQ(TIM4_IRQn);
      capture_is_ready = 0;
      direction = captured_direction;
      NVIC_EnableIRQ(TIM4_IRQn);

      if(direction==FORWARD) {
        val++;
        sprintf(bufa,"----> %d",val);
      }
      else {
        val--;
        sprintf(bufa,"<---- %d",val);
      }
      MDX7900R_text(3,bufa,1);
    }

    DWT_Delay(50000);
  }
}
