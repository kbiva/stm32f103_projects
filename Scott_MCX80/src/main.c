/*
 *  main.c
 *
 *  Scott MCX80 face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "adc.h"
#include "mcx80.h"
#include <stdio.h>

extern volatile uint8_t capture_is_ready;
extern volatile Volume_Direction captured_direction;

extern uint8_t display_data[];

extern volatile uint16_t ADCConvertedValue[];

uint8_t symbols[]={
  SYMBOL_DISC1,SYMBOL_DISC2,SYMBOL_DISC3,SYMBOL_DISC4,SYMBOL_2RINGS,SYMBOL_MUTE,SYMBOL_MUSIC,SYMBOL_FOLDER,
  SYMBOL_NOTE,SYMBOL_PEOPLE,SYMBOL_BOOK,SYMBOL_EQ,SYMBOL_EQ1,SYMBOL_ROCK,SYMBOL_POP,SYMBOL_FLAT,SYMBOL_CLAS,
  SYMBOL_BLUETOOTH,SYMBOL_TP,SYMBOL_TA,SYMBOL_AF,SYMBOL_PTY,SYMBOL_EON,SYMBOL_RDM,SYMBOL_INT,SYMBOL_RPT,SYMBOL_LOUD,
  SYMBOL_MP3,SYMBOL_DISC,SYMBOL_TRACK,SYMBOL_DOTS1,SYMBOL_DOTS2,SYMBOL_DOTS3,SYMBOL_DOTS4,SYMBOL_DOTS5,SYMBOL_DOTS6,SYMBOL_DOTS7,
};

void demo1(int delay,int on,int flush) {

  uint8_t i;

  for(i=0;i<sizeof(symbols)/sizeof(uint8_t);i++) {
    MCX80_symbol(symbols[i],on,flush);
    DWT_Delay(delay);
  }
}

int main(void) {

  int32_t i;
  uint16_t ADC_Val1,ADC_Val2;
  uint16_t val=1000;
  uint8_t cleared=0;

  char buf[16];

  DWT_Init();

  MCX80_init();

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  DWT_Delay(1000000);

  for(i=0;i<20;i++) {
    display_data[i]=0xFF;
  }
  MCX80_send();
  DWT_Delay(1000000);

  for(i=0;i<20;i++) {
    display_data[i]=0x00;
  }
  MCX80_send();
  DWT_Delay(1000000);

  for(i=0;i<20;i++) {
    display_data[i]=0xFF;
  }
  MCX80_send();
  DWT_Delay(1000000);

  for(i=0;i<20;i++) {
    display_data[i]=0x00;
  }
  MCX80_send();
  DWT_Delay(1000000);

  for(i=0;i<20;i++) {
    MCX80_digit(i,TRUE);
    DWT_Delay(200000);
  }
  MCX80_digit_off(TRUE);

  MCX80_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(1000000);

  MCX80_symbol(SYMBOL_DOTS1,TRUE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS2,TRUE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS3,TRUE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS4,TRUE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS5,TRUE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS6,TRUE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS7,TRUE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS7,FALSE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS6,FALSE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS5,FALSE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS4,FALSE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS3,FALSE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS2,FALSE,TRUE);
  DWT_Delay(100000);
  MCX80_symbol(SYMBOL_DOTS1,FALSE,TRUE);
  DWT_Delay(100000);

  demo1(100000,TRUE,TRUE);
  demo1(100000,FALSE,TRUE);

  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  MCX80_send();

  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  MCX80_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  MCX80_send();
  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  MCX80_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  MCX80_send();
  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  MCX80_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  MCX80_send();
  DWT_Delay(500000);
  MCX80_clear_text(TRUE);
  DWT_Delay(500000);
  MCX80_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);
  MCX80_clear_text(TRUE);
  DWT_Delay(500000);
  MCX80_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);
  MCX80_clear_text(TRUE);
  DWT_Delay(500000);
  MCX80_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);

  MCX80_text("kbiva.wordpress.com",0,TRUE,TRUE);
  DWT_Delay(2000000);

  for(i=0;i>=-11;i--) {
    MCX80_text("kbiva.wordpress.com",i,TRUE,TRUE);
    DWT_Delay(200000);
  }
  DWT_Delay(1000000);
  for(i=-11;i<=0;i++) {
    MCX80_text("kbiva.wordpress.com",i,TRUE,TRUE);
    DWT_Delay(200000);
  }
  DWT_Delay(2000000);

  demo1(0,FALSE,FALSE);
  MCX80_send();

  while(1){

    ADC_Val1 = ADCConvertedValue[0];
    ADC_Val2 = ADCConvertedValue[1];

    if((ADC_Val1<3000)||(ADC_Val2<3000)) {

      if(ADC_Val1<50){MCX80_text("Button 1",0,TRUE,TRUE);}
      else if(ADC_Val1<100){MCX80_text("Button 2",0,TRUE,TRUE);}
      else if(ADC_Val1<175){MCX80_text("Button 3",0,TRUE,TRUE);}
      else if(ADC_Val1<297){MCX80_text("Button 4",0,TRUE,TRUE);}
      else if(ADC_Val1<473){MCX80_text("Button 5",0,TRUE,TRUE);}
      else if(ADC_Val1<704){MCX80_text("Button 6",0,TRUE,TRUE);}
      else if(ADC_Val1<1012){MCX80_text(">>",0,TRUE,TRUE);}
      else if(ADC_Val1<1364){MCX80_text("<<",0,TRUE,TRUE);}
      else if(ADC_Val1<1782){MCX80_text("BND",0,TRUE,TRUE);}
      else if(ADC_Val1<2233){MCX80_text("AP",0,TRUE,TRUE);}

      if(ADC_Val2<50){MCX80_text("PWR",0,TRUE,TRUE);}
      else if(ADC_Val2<190){MCX80_text("AF",0,TRUE,TRUE);}
      else if(ADC_Val2<330){MCX80_text("TA",0,TRUE,TRUE);}
      else if(ADC_Val2<484){MCX80_text("PTY",0,TRUE,TRUE);}
      else if(ADC_Val2<715){MCX80_text("MOD",0,TRUE,TRUE);}
      else if(ADC_Val2<1012){MCX80_text("MU",0,TRUE,TRUE);}
      else if(ADC_Val2<1375){MCX80_text("SEL",0,TRUE,TRUE);}
      else if(ADC_Val2<2662){MCX80_text("CLK",0,TRUE,TRUE);}

      cleared=0;
    }
    else {
      if(!cleared) {
        MCX80_clear_text(TRUE);
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
        sprintf(buf,"--> %d",val);
      }
      else {
        val--;
        sprintf(buf,"<-- %d",val);
      }
      MCX80_text(buf,0,TRUE,TRUE);
    }

    DWT_Delay(50000);
  };
}
