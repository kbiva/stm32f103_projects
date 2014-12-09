/*
 *  main.c
 *
 *  Clarion DRX7375R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include <stdio.h>
#include <string.h>
#include "stm32f10x_conf.h"
#include "delay.h"
#include "drx7375r.h"
#include "ir_decode.h"
#include "queue.h"
#include "hy_stm32.h"
#include "adc.h"

extern volatile uint8_t recState;
IR_PACKET *irp;

extern uint8_t key_data[];
extern DATA_LC75854 packet_LC75854[];
extern DATA_LC75824 packet_LC75824[];

uint8_t ring[16]={
  RING_1,
  RING_2,
  RING_3,
  RING_4,
  RING_5,
  RING_6,
  RING_7,
  RING_8,
  RING_9,
  RING_10,
  RING_11,
  RING_12,
  RING_13,
  RING_14,
  RING_15,
  RING_16
};

char strMenu[32][16]={
  "KEY TEST",
  "RAW KEY VALUES",
  "SEGMENTS TEST",
  "SLEEP TEST",
  "SEGMENTS ON/OFF",
  "ANIMATION",
  "IR RECEIVER",
  "DIMMER TEST",
  "ABOUT",
};

void menu_text(void);
void menu_key_test(void);
void menu_raw_key_values(void);
void menu_segments_test(void);
void menu_sleep_test(void);
void menu_segments_onoff(void);
void menu_animation(void);
void menu_ir_receiver(void);
void menu_dimmer_test(void);
void menu_about(void);

extern volatile uint16_t ADCConvertedValue;

char bufa[10];

int main(void) {

  int8_t inmenu=0,menuval=0,menulen,menuscroll,menuclip,menudir=1,clipdir=1;
  char *menustring;

  DWT_Init();
  BSP_LEDs_Init();

  GPIO_Configuration();

  DRX7375R_init();

  IR_Init();

  ADC_DMA_Configuration();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  DRX7375R_clear(TRUE);

  DRX7375R_text("HI WORLD",0,TRUE,TRUE);

  while(1){

    DRX7375R_receive_LC75854();

    if(inmenu) {

      if(menudir) {
        if(clipdir) {
          DRX7375R_text(menustring+menuclip++,menuscroll++,TRUE,TRUE);
        }
        else {
          DRX7375R_text(menustring+menuclip--,menuscroll++,TRUE,TRUE);
        }
      }
      else {
        if(clipdir) {
          DRX7375R_text(menustring+menuclip++,menuscroll--,TRUE,TRUE);
        }
        else {
          DRX7375R_text(menustring+menuclip--,menuscroll--,TRUE,TRUE);
        }
      }
      if(menuscroll>10-menulen) {
        menudir=0;
        menuscroll--;
      }
      else if(menuscroll<0) {
        menudir=1;
        menuscroll++;
      }
      if(menulen>=10) {
        menuscroll=0;
      }
      if(menulen<=10) {
        menuclip=0;
      }
      else if(menuclip>menulen-10) {
        clipdir=0;
        menuclip--;
      }
      else if(menuclip<0) {
        clipdir=1;
        menuclip++;
      }

      if(key_data[1]&KEY_RIGHT) {
        menuval++;
        if(menuval>8) {
          menuval=0;
        }
        menuscroll=0;
        menuclip=0;
        menulen=strlen(strMenu[menuval]);
        menustring=strMenu[menuval];
      }
      if(key_data[1]&KEY_LEFT) {
        menuval--;
        if(menuval<0) {
          menuval=8;
        }
        menuscroll=0;
        menuclip=0;
        menulen=strlen(strMenu[menuval]);
        menustring=strMenu[menuval];
      }

      if(key_data[1]&KEY_ENTER) {

        switch(menuval) {
          case 0:
            menu_key_test();
            inmenu=1;
          break;
          case 1:
            menu_raw_key_values();
            inmenu=1;
          break;
          case 2:
            menu_segments_test();
            inmenu=1;
          break;
          case 3:
            menu_sleep_test();
            inmenu=1;
          break;
          case 4:
            menu_segments_onoff();
            inmenu=1;
          break;
          case 5:
            menu_animation();
            inmenu=1;
          break;
          case 6:
            menu_ir_receiver();
            inmenu=1;
          break;
          case 7:
            menu_dimmer_test();
            inmenu=1;
          break;
          case 8:
            menu_about();
            inmenu=1;
          break;
        }
        DRX7375R_clear(TRUE);
        menu_text();
      }
    }
    else {
      if(key_data[1]&KEY_ENTER) {
        menu_text();
        inmenu=1;
        menuclip=0;
        menuscroll=0;
        menulen=strlen(strMenu[menuval]);
        menustring=strMenu[menuval];
      }
    }
    DWT_Delay(500000);
  }
}

void menu_text(void) {

  DRX7375R_fill_dots(0,0xFE,FALSE);
  DRX7375R_fill_dots(1,0xC0,FALSE);
  DRX7375R_fill_dots(2,0xFE,FALSE);
  DRX7375R_fill_dots(3,0xC0,FALSE);
  DRX7375R_fill_dots(4,0xFE,FALSE);

  DRX7375R_fill_dots(6,0xFE,FALSE);
  DRX7375R_fill_dots(7,0x92,FALSE);
  DRX7375R_fill_dots(8,0x92,FALSE);

  DRX7375R_fill_dots(10,0xFE,FALSE);
  DRX7375R_fill_dots(11,0x70,FALSE);
  DRX7375R_fill_dots(12,0x0C,FALSE);
  DRX7375R_fill_dots(13,0xFE,FALSE);

  DRX7375R_fill_dots(15,0xFE,FALSE);
  DRX7375R_fill_dots(16,0x06,FALSE);
  DRX7375R_fill_dots(17,0xFE,TRUE);

  DRX7375R_fill_symbol_val_LC75824(SYMBOL_LC75824_LESS_MORE_FILLED,TRUE,TRUE);
}

void menu_key_test(void) {

  DRX7375R_clear(TRUE);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    DRX7375R_receive_LC75854();

    if(key_data[0]&KEY_DSP)DRX7375R_text("DSP",0,TRUE,TRUE);
    if(key_data[0]&KEY_EQ)DRX7375R_text("EQ",0,TRUE,TRUE);

    if(key_data[1]&KEY_RIGHT)DRX7375R_text("RIGHT",0,TRUE,TRUE);
    if(key_data[1]&KEY_LEFT)DRX7375R_text("LEFT",0,TRUE,TRUE);
    if(key_data[1]&KEY_DOWN)DRX7375R_text("DOWN",0,TRUE,TRUE);
    if(key_data[1]&KEY_UP)DRX7375R_text("UP",0,TRUE,TRUE);
    if(key_data[1]&KEY_ENTER)DRX7375R_text("ENTER",0,TRUE,TRUE);
    if(key_data[1]&KEY_ADJ)DRX7375R_text("ADJ",0,TRUE,TRUE);

    if(key_data[2]&KEY_4)DRX7375R_text("4",0,TRUE,TRUE);
    if(key_data[2]&KEY_RPT)DRX7375R_text("RPT",0,TRUE,TRUE);
    if(key_data[2]&KEY_RDM)DRX7375R_text("RDM",0,TRUE,TRUE);
    if(key_data[2]&KEY_DISP)DRX7375R_text("DISP",0,TRUE,TRUE);
    if(key_data[2]&KEY_TA)DRX7375R_text("TA",0,TRUE,TRUE);
    if(key_data[2]&KEY_BAND)DRX7375R_text("BAND",0,TRUE,TRUE);
    if(key_data[2]&KEY_AM)DRX7375R_text("AM",0,TRUE,TRUE);
    if(key_data[2]&KEY_TITLE)DRX7375R_text("TITLE",0,TRUE,TRUE);

    if(key_data[3]&KEY_3)DRX7375R_text("3",0,TRUE,TRUE);
    if(key_data[3]&KEY_2)DRX7375R_text("2",0,TRUE,TRUE);
    if(key_data[3]&KEY_1)DRX7375R_text("1",0,TRUE,TRUE);
    if(key_data[3]&KEY_SCN)DRX7375R_text("SCN",0,TRUE,TRUE);
    if(key_data[3]&KEY_6)DRX7375R_text("6",0,TRUE,TRUE);
    if(key_data[3]&KEY_5)DRX7375R_text("5",0,TRUE,TRUE);

    DWT_Delay(100000);
  }
}

void menu_raw_key_values(void) {

  DRX7375R_clear(TRUE);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    DRX7375R_receive_LC75854();

    sprintf(bufa,"%02X",key_data[0]);
    DRX7375R_text(bufa,0,TRUE,FALSE);
    sprintf(bufa,"%02X",key_data[1]);
    DRX7375R_text(bufa,2,FALSE,FALSE);
    sprintf(bufa,"%02X",key_data[2]);
    DRX7375R_text(bufa,4,FALSE,FALSE);
    sprintf(bufa,"%02X",key_data[3]);
    DRX7375R_text(bufa,6,FALSE,TRUE);

    DWT_Delay(100000);
  }
}

void menu_segments_test(void) {

  uint32_t i,j,k;

  DRX7375R_clear(TRUE);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    DRX7375R_receive_LC75854();

    if(key_data[3]&KEY_1) {
      DRX7375R_fill_LC75854(TRUE);
    }
    if(key_data[3]&KEY_2) {
      DRX7375R_fill_LC75824(TRUE);
    }
    if(key_data[3]&KEY_3) {
      DRX7375R_fill(TRUE);
    }
    if(key_data[2]&KEY_4) {
      DRX7375R_clear_LC75854(TRUE);
    }
    if(key_data[3]&KEY_5) {
      DRX7375R_clear_LC75824(TRUE);
    }
    if(key_data[3]&KEY_6) {
      DRX7375R_clear(TRUE);
    }

    if(key_data[1]&KEY_RIGHT)  {

      DRX7375R_fill_LC75854(TRUE);
      for(i=0;i<1;i++){
        for(j=0;j<5;j++){
          for(k=1;k<(1<<8);k<<=1){
            packet_LC75854[i].data[j]&=~k;
            DRX7375R_send_LC75854();
            DWT_Delay(100000);
          }
        }
      }
      packet_LC75854[0].data[5]&=~0x80;
      DRX7375R_send_LC75854();
      DWT_Delay(100000);
      packet_LC75854[0].data[5]&=~0x40;
      DRX7375R_send_LC75854();
      DWT_Delay(100000);
      packet_LC75854[0].data[5]&=~0x20;
      DRX7375R_send_LC75854();
      DWT_Delay(100000);
      packet_LC75854[0].data[5]&=~0x10;
      DRX7375R_send_LC75854();
      DWT_Delay(100000);
      for(i=1;i<4;i++){
        for(j=0;j<5;j++){
          for(k=1;k<(1<<8);k<<=1){
            packet_LC75854[i].data[j]&=~k;
            DRX7375R_send_LC75854();
            DWT_Delay(100000);
          }
        }
      }
    }

     if(key_data[1]&KEY_LEFT) {

      DRX7375R_clear_LC75854(TRUE);
      for(i=0;i<1;i++){
        for(j=0;j<5;j++){
          for(k=1;k<(1<<8);k<<=1){
            packet_LC75854[i].data[j]|=k;
            DRX7375R_send_LC75854();
            DWT_Delay(100000);
          }
        }
      }
      packet_LC75854[0].data[5]|=0x80;
      DRX7375R_send_LC75854();
      DWT_Delay(100000);
      packet_LC75854[0].data[5]|=0x40;
      DRX7375R_send_LC75854();
      DWT_Delay(100000);
      packet_LC75854[0].data[5]|=0x20;
      DRX7375R_send_LC75854();
      DWT_Delay(100000);
      packet_LC75854[0].data[5]|=0x10;
      DRX7375R_send_LC75854();
      DWT_Delay(100000);
      for(i=1;i<4;i++){
        for(j=0;j<5;j++){
          for(k=1;k<(1<<8);k<<=1){
            packet_LC75854[i].data[j]|=k;
            DRX7375R_send_LC75854();
            DWT_Delay(100000);
          }
        }
      }
     }

    if(key_data[1]&KEY_UP) {

      DRX7375R_fill_LC75824(TRUE);
      for(j=0;j<6;j++){
        for(k=1;k<(1<<8);k<<=1){
          packet_LC75824[0].data[j]&=~k;
          DRX7375R_send_LC75824();
          DWT_Delay(100000);
        }
      }
      packet_LC75824[0].data[6]&=~0x80;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[0].data[6]&=~0x40;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[0].data[6]&=~0x20;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[0].data[6]&=~0x10;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);

      for(j=0;j<6;j++){
        for(k=1;k<(1<<8);k<<=1){
          packet_LC75824[1].data[j]&=~k;
          DRX7375R_send_LC75824();
          DWT_Delay(100000);
        }
      }
      packet_LC75824[1].data[6]&=~0x80;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[1].data[6]&=~0x40;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[1].data[6]&=~0x20;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[1].data[6]&=~0x10;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);

      for(j=0;j<6;j++){
        for(k=1;k<(1<<8);k<<=1){
          packet_LC75824[2].data[j]&=~k;
          DRX7375R_send_LC75824();
          DWT_Delay(100000);
        }
      }
      packet_LC75824[2].data[6]&=~0x80;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[2].data[6]&=~0x40;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[2].data[6]&=~0x20;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[2].data[6]&=~0x10;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);

      for(j=0;j<6;j++){
        for(k=1;k<(1<<8);k<<=1){
          packet_LC75824[3].data[j]&=~k;
          DRX7375R_send_LC75824();
          DWT_Delay(100000);
        }
      }
    }

    if(key_data[1]&KEY_DOWN) {

      DRX7375R_clear_LC75824(TRUE);
      for(j=0;j<6;j++){
        for(k=1;k<(1<<8);k<<=1){
          packet_LC75824[0].data[j]|=k;
          DRX7375R_send_LC75824();
          DWT_Delay(100000);
        }
      }
      packet_LC75824[0].data[6]|=0x80;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[0].data[6]|=0x40;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[0].data[6]|=0x20;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[0].data[6]|=0x10;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);

      for(j=0;j<6;j++){
        for(k=1;k<(1<<8);k<<=1){
          packet_LC75824[1].data[j]|=k;
          DRX7375R_send_LC75824();
          DWT_Delay(100000);
        }
      }
      packet_LC75824[1].data[6]|=0x80;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[1].data[6]|=0x40;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[1].data[6]|=0x20;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[1].data[6]|=0x10;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);

      for(j=0;j<6;j++){
        for(k=1;k<(1<<8);k<<=1){
          packet_LC75824[2].data[j]|=k;
          DRX7375R_send_LC75824();
          DWT_Delay(100000);
        }
      }
      packet_LC75824[2].data[6]|=0x80;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[2].data[6]|=0x40;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[2].data[6]|=0x20;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);
      packet_LC75824[2].data[6]|=0x10;
      DRX7375R_send_LC75824();
      DWT_Delay(100000);

      for(j=0;j<6;j++){
        for(k=1;k<(1<<8);k<<=1){
          packet_LC75824[3].data[j]|=k;
          DRX7375R_send_LC75824();
          DWT_Delay(100000);
        }
      }
    }

    DWT_Delay(500000);
  }
}

void menu_sleep_test(void) {

  DRX7375R_fill(TRUE);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    DRX7375R_receive_LC75854();

    if(key_data[3]&KEY_1) {
      DRX7375R_sleep(TRUE);
      DRX7375R_send();
    }
    if(key_data[3]&KEY_2) {
      DRX7375R_sleep(FALSE);
      DRX7375R_send();
    }

    DWT_Delay(100000);
  }
  DRX7375R_sleep(FALSE);
  DRX7375R_send();
}

void menu_segments_onoff(void) {

  DRX7375R_fill(TRUE);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    DRX7375R_receive_LC75854();

    if(key_data[3]&KEY_1) {
      DRX7375R_segments(FALSE);
      DRX7375R_send();
    }
    if(key_data[3]&KEY_2) {
      DRX7375R_segments(TRUE);
      DRX7375R_send();
    }

    DWT_Delay(100000);
  }
  DRX7375R_segments(TRUE);
  DRX7375R_send();
}

void menu_animation(void) {

  uint8_t ring_forward=0,ring_full=0,mode=3,inv=1;
  int8_t i=0,j=2,s=1,col=0,wobble=0,wobbleon=0,wobblepos=0,wobbledir=1;
  uint8_t thick=0xFE;

  DRX7375R_clear(TRUE);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    DRX7375R_receive_LC75854();

    if(key_data[1]&KEY_RIGHT) {
      DRX7375R_text("RIGHT",0,TRUE,TRUE);
      ring_forward=1;
      mode=2;
    }
    if(key_data[1]&KEY_LEFT) {
      DRX7375R_text("LEFT",0,TRUE,TRUE);
      ring_forward=0;
      mode=3;
    }
    if(key_data[1]&KEY_DOWN) {
      DRX7375R_text("DOWN",0,TRUE,TRUE);
      ring_full=1;
      mode=0;
    }
    if(key_data[1]&KEY_UP) {
      DRX7375R_text("UP",0,TRUE,TRUE);
      ring_full=0;
      mode=1;
    }

    if(key_data[3]&KEY_1) {
      thick=0xFE;
      wobble=0;
    }
    if(key_data[3]&KEY_2) {
      thick=0x7C;
      wobble=1;
    }
    if(key_data[3]&KEY_3) {
      thick=0x38;
      wobble=2;
    }
    if(key_data[2]&KEY_4) {
      thick=0x10;
      wobble=3;
    }
    if(key_data[3]&KEY_5) {
      wobbleon=1;
    }
    if(key_data[3]&KEY_6) {
      wobbleon=0;
    }

    if(key_data[2]&KEY_DISP) {
      inv=0;
    }
    if(key_data[2]&KEY_TA) {
      inv=1;
    }

    if(wobbleon) {
      switch(wobble){
        case 0:
          thick=0xFE;
          wobblepos=0;
        break;
        case 1:// 3E 7C F8
          switch(wobblepos) {
            case 0:thick=0x3E;break;
            case 1:thick=0x7C;break;
            case 2:thick=0xF8;break;
          }
          if((wobblepos>=2)&&wobbledir) {
            wobbledir=0;
          }
          else if((wobblepos<=0)&&(!wobbledir)) {
            wobbledir=1;
          }
          else {
            if(wobbledir)
              wobblepos++;
            else
              wobblepos--;
          }
        break;
        case 2:// E0 70 38 1C 0E
          switch(wobblepos) {
            case 0:thick=0xE0;break;
            case 1:thick=0x70;break;
            case 2:thick=0x38;break;
            case 3:thick=0x1C;break;
            case 4:thick=0x0E;break;
          }
          if((wobblepos>=4)&&wobbledir) {
            wobbledir=0;
          }
          else if((wobblepos<=0)&&(!wobbledir)) {
            wobbledir=1;
          }
          else {
            if(wobbledir)
              wobblepos++;
            else
              wobblepos--;
          }
        break;
        case 3:// 80 40 20 10 08 04 02
          switch(wobblepos) {
            case 0:thick=0x80;break;
            case 1:thick=0x40;break;
            case 2:thick=0x20;break;
            case 3:thick=0x10;break;
            case 4:thick=0x08;break;
            case 5:thick=0x04;break;
            case 6:thick=0x02;break;
          }
          if((wobblepos>=6)&&wobbledir) {
            wobbledir=0;
          }
          else if((wobblepos<=0)&&(!wobbledir)) {
            wobbledir=1;
          }
          else {
            if(wobbledir)
              wobblepos++;
            else
              wobblepos--;
          }
        break;
      }
    }

    if(ring_full) {
      DRX7375R_ring(~(1<<(16-ring[i])),FALSE);
    }
    else {
      DRX7375R_ring((1<<(16-ring[i])),FALSE);
    }

    if(ring_forward) {
      i++;
      if(i>15)i=0;
      col++;
      if(col>17)col=0;
    }
    else {
      i--;
      if(i<0)i=15;
      col--;
      if(col<0)col=17;
    }

    if(inv) {
      DRX7375R_fill_all_dots(0xFE,FALSE);
    }
    else {
      DRX7375R_clear_all_dots(0xFE,FALSE);
    }

    switch(mode) {
      case 0:
        if(inv) {
          DRX7375R_clear_all_dots(j,TRUE);
        }
        else {
          DRX7375R_fill_all_dots(j,TRUE);
        }
        s--;
        j=(1<<s);
        if(s<1) {
          s=7;
          j=0x80;
        }
      break;
      case 1:
        if(inv) {
          DRX7375R_clear_all_dots(j,TRUE);
        }
        else {
          DRX7375R_fill_all_dots(j,TRUE);
        }
        s++;
        j=(1<<s);
        if(s>7) {
          s=1;
          j=0x02;
        }
      break;
      case 2:
        if(inv) {
          DRX7375R_clear_dots(col,thick,TRUE);
        }
        else {
          DRX7375R_fill_dots(col,thick,TRUE);
        }
      break;
      case 3:
        if(inv) {
          DRX7375R_clear_dots(col,thick,TRUE);
        }
        else {
          DRX7375R_fill_dots(col,thick,TRUE);
        }
      break;
    }

    DWT_Delay(100000);
  }
}

void menu_ir_receiver(void){

  uint8_t cnt=0;

  DRX7375R_clear(TRUE);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    if((irp=qretrieve())!=0) {
      IR_Decode(irp);
    }

    if(recState==REC_Captured)
      recState=REC_Idle;

    if(!cnt) {

      DRX7375R_receive_LC75854();
    }

    cnt++;
    if(cnt==100)
      cnt=0;

    DWT_Delay(1000);
  }
}

void menu_dimmer_test(void) {

  DRX7375R_clear(TRUE);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    sprintf(bufa,"%d.%d V",ADCConvertedValue/1000,ADCConvertedValue%1000);
    DRX7375R_text(bufa,0,TRUE,TRUE);

    DRX7375R_receive_LC75854();

    DWT_Delay(100000);
  }
}

void menu_about(void) {

  char *text="CLARION DRX7375R FACE PLATE... MORE: KBIVA.WORDPRESS.COM";
  int8_t pos=0,clipdir=1,stringlen,fill=0;
  int8_t i=0,col=0,wobblepos=0,wobbledir=1;
  uint8_t thick=0xFE;

  DRX7375R_clear(TRUE);

  stringlen=strlen(text);

  while(!((key_data[3]&KEY_6)&&(key_data[3]&KEY_5))) {

    DRX7375R_receive_LC75854();

    if(!fill) {
      if(clipdir)
        DRX7375R_text(text+pos++,0,TRUE,TRUE);
      else
        DRX7375R_text(text+pos--,0,TRUE,TRUE);

      if(pos>stringlen-10) {
        clipdir=0;
        pos--;
      }
      else if(pos<0) {
        clipdir=1;
        pos++;
      }
    }

    fill++;
    if(fill>4)
      fill=0;

    switch(wobblepos) {
      case 0:thick=0xE0;break;
      case 1:thick=0x70;break;
      case 2:thick=0x38;break;
      case 3:thick=0x1C;break;
      case 4:thick=0x0E;break;
    }
    if((wobblepos>=4)&&wobbledir) {
      wobbledir=0;
    }
    else if((wobblepos<=0)&&(!wobbledir)) {
      wobbledir=1;
    }
    else {
      if(wobbledir)
        wobblepos++;
      else
        wobblepos--;
    }

    DRX7375R_ring(~(1<<(16-ring[i])),FALSE);

    i++;
    if(i>15)i=0;
    col++;
    if(col>17)col=0;

    DRX7375R_fill_all_dots(0xFE,FALSE);

    DRX7375R_clear_dots(col,thick,TRUE);

    DWT_Delay(100000);
  }
}
