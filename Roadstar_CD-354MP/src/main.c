/*
 *  main.c
 *
 *  Roadstar CD-354MP face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "adc.h"
#include "cd354mp.h"
#include <stdio.h>

extern uint8_t display_data[];

extern volatile uint16_t ADCConvertedValue[];

const uint16_t V25 = 1750;// when V25=1.41V at ref 3.3V
const uint16_t Avg_Slope = 5; //when avg_slope=4.3mV/C at ref 3.3V

void demo1(int delay,int on,int flush) {

  CD354MP_symbol(SYMBOL_CD1_POS,SYMBOL_CD1_BIT,on,flush);
  CD354MP_symbol(SYMBOL_CD2_POS,SYMBOL_CD2_BIT,on,flush);
  CD354MP_symbol(SYMBOL_CD3_POS,SYMBOL_CD3_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_TP_POS,SYMBOL_TP_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_TA_POS,SYMBOL_TA_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_AF_POS,SYMBOL_AF_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_LEFT1_POS,SYMBOL_RING_LEFT1_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_LEFT2_POS,SYMBOL_RING_LEFT2_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_LEFT3_POS,SYMBOL_RING_LEFT3_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_LEFT4_POS,SYMBOL_RING_LEFT4_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_LEFT5_POS,SYMBOL_RING_LEFT5_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_LEFT6_POS,SYMBOL_RING_LEFT6_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_MP3_POS,SYMBOL_MP3_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_PTY_POS,SYMBOL_PTY_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_STEREO_POS,SYMBOL_STEREO_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_ANTENNA_POS,SYMBOL_ANTENNA_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_BAR4_POS,SYMBOL_BAR4_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_BAR3_POS,SYMBOL_BAR3_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_BAR2_POS,SYMBOL_BAR2_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_BAR1_POS,SYMBOL_BAR1_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_RIGHT6_POS,SYMBOL_RING_RIGHT6_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_RIGHT5_POS,SYMBOL_RING_RIGHT5_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_RIGHT4_POS,SYMBOL_RING_RIGHT4_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_RIGHT3_POS,SYMBOL_RING_RIGHT3_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_RIGHT2_POS,SYMBOL_RING_RIGHT2_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_RING_RIGHT1_POS,SYMBOL_RING_RIGHT1_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_CDIN_POS,SYMBOL_CDIN_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_DSP_POS,SYMBOL_DSP_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_DIR_POS,SYMBOL_DIR_BIT,on,flush);
  DWT_Delay(delay);
  CD354MP_symbol(SYMBOL_POINT_POS,SYMBOL_POINT_BIT,on,flush);

}


int main(void) {

  uint32_t i;
  uint16_t ADC_Val1,ADC_Val2;

  DWT_Init();

  //DWT_Delay(500);
  GPIO_Configuration();
  CD354MP_init();

  ADC_DMA_Configuration();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);


  for(i=0;i<20;i++) {
     display_data[i]=0xFF;
  }

  CD354MP_send();


  DWT_Delay(5000000);

  for(i=0;i<20;i++) {
     display_data[i]=0x00;
  }

  CD354MP_send();
  DWT_Delay(500000);

  for(i=0;i<20;i++) {
     display_data[i]=0xFF;
  }

  CD354MP_send();
  DWT_Delay(500000);

  for(i=0;i<20;i++) {
     display_data[i]=0x00;
  }

  CD354MP_send();
  DWT_Delay(500000);

  for(i=0;i<20;i++) {
     display_data[i]=0xFF;
  }

  CD354MP_send();
  DWT_Delay(500000);

  for(i=0;i<20;i++) {
     display_data[i]=0x00;
  }

  CD354MP_send();
  DWT_Delay(500000);

  for(i=0;i<10;i++) {
    CD354MP_digit(i,TRUE);
    DWT_Delay(500000);
  }
  CD354MP_digit_off(TRUE);

  CD354MP_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(1000000);

  demo1(100000,TRUE,TRUE);
  demo1(100000,FALSE,TRUE);
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  CD354MP_send();

  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  CD354MP_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  CD354MP_send();
  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  CD354MP_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  CD354MP_send();
  DWT_Delay(500000);
  demo1(0,FALSE,FALSE);
  CD354MP_send();
  DWT_Delay(500000);
  demo1(0,TRUE,FALSE);
  CD354MP_send();
  DWT_Delay(500000);
  CD354MP_clear_text(TRUE);
  DWT_Delay(500000);
  CD354MP_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);
  CD354MP_clear_text(TRUE);
  DWT_Delay(500000);
  CD354MP_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);
  CD354MP_clear_text(TRUE);
  DWT_Delay(500000);
  CD354MP_text("HI WORLD",0,TRUE,TRUE);
  DWT_Delay(500000);

  CD354MP_text("I WORLD",0,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text(" WORLD",0,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("WORLD",0,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("ORLD",0,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("RLD",0,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("LD",0,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("D",0,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_clear_text(TRUE);
  DWT_Delay(200000);
  CD354MP_text("H",7,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("HI",6,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("HI ",5,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("HI W",4,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("HI WO",3,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("HI WOR",2,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("HI WORL",1,TRUE,TRUE);
  DWT_Delay(200000);
  CD354MP_text("HI WORLD",0,TRUE,TRUE);

  DWT_Delay(2000000);

  while(1){

    ADC_Val1 = ADCConvertedValue[0];
    ADC_Val2 = ADCConvertedValue[1];

    if((ADC_Val1>3893)&&ADC_Val2>3888){
      CD354MP_clear_text(TRUE);
    }
    else {

      if(ADC_Val1<235){CD354MP_text("DISP",0,TRUE,TRUE);}
      else if(ADC_Val1<435){CD354MP_text("PWR",0,TRUE,TRUE);}
      else if(ADC_Val1<695){CD354MP_text("AF",0,TRUE,TRUE);}
      else if(ADC_Val1<1005){CD354MP_text("TA",0,TRUE,TRUE);}
      else if(ADC_Val1<1370){CD354MP_text("PTY",0,TRUE,TRUE);}
      else if(ADC_Val1<1770){CD354MP_text("MODE",0,TRUE,TRUE);}
      else if(ADC_Val1<2540){CD354MP_text("MUTE",0,TRUE,TRUE);}
      else if(ADC_Val1<3385){CD354MP_text("DSP",0,TRUE,TRUE);}
      else if(ADC_Val1<3893){CD354MP_text("CD EJ",0,TRUE,TRUE);}

      if(ADC_Val2<75){CD354MP_text("ONE",0,TRUE,TRUE);}
      else if(ADC_Val2<230){CD354MP_text("TWO",0,TRUE,TRUE);}
      else if(ADC_Val2<430){CD354MP_text("THREE",0,TRUE,TRUE);}
      else if(ADC_Val2<685){CD354MP_text("FOUR",0,TRUE,TRUE);}
      else if(ADC_Val2<995){CD354MP_text("FIVE",0,TRUE,TRUE);}
      else if(ADC_Val2<1360){CD354MP_text("SIX",0,TRUE,TRUE);}
      else if(ADC_Val2<1765){CD354MP_text("BND",0,TRUE,TRUE);}
      else if(ADC_Val2<2190){CD354MP_text(">>",0,TRUE,TRUE);}
      else if(ADC_Val2<2580){CD354MP_text("<<",0,TRUE,TRUE);}
      else if(ADC_Val2<3220){CD354MP_text("JOYSTICK",0,TRUE,TRUE);}
      else if(ADC_Val2<3888){CD354MP_text("AMS",0,TRUE,TRUE);}
    }

    DWT_Delay(100000);
  };
}
