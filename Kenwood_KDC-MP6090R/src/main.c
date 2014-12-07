/*
 *  main.c
 *
 *  Kenwood KDC-MP6090R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "mp6090r.h"
#include "ir_decode.h"
#include "queue.h"
#include "hy_stm32.h"

extern uint8_t key_data[];

extern volatile uint8_t recState;
IR_PACKET *irp;

char buf[3];

void print_hex(unsigned char c) {

const unsigned char hexdigits[] = "0123456789ABCDEF";

  buf[0]=(hexdigits[c >> 4]);
  buf[1]=(hexdigits[c & 0xf]);
  buf[2]=0;
}

int main(void) {

  int i=0,d=0,dim=0,s=0,sp=0,cl=0,cp=5;
  CONTRAST c[11]={CT10,CT9,CT8,CT7,CT6,CT5,CT4,CT3,CT2,CT1,CT0};

  DWT_Init();

  BSP_LEDs_Init();

  GPIO_Configuration();
  MP6090R_init(CT5);

  IR_Init();


/*
  for(i=0;i<256;i++) {
    b1=i;
    for(j=0;j<256;j++) {
      b2=j;
      send_dd_MP6090R();
      delay_ms(100);
    }
  }
*/
//  send_dd_MP6090R();
  MP6090R_dimmer(FALSE);

//  delay_ms(1000);
/*
  for(i=0;i<60;i++) {
    for (j = (1<<7); j; j>>=1) {
      display_data[i]=j;
      send_dd_MP6090R();
      delay_ms(1000);
    }
    display_data[i]=0;;
  }
*/

  MP6090R_contrast(CT5);
  //b2=CT11|CTC_ON;
/*  send_dd_MP6090R();
  delay_ms(1000);

  b1=BUTTONS_RED;
  send_dd_MP6090R();
  delay_ms(2000);

  b1=0;
  b2=CT11|CTC_ON|BUTTONS_GREEN;
  send_dd_MP6090R();
  delay_ms(2000);
*/

/*
  display_data[0]=0x48;
  display_data[1]=0x54;
  display_data[2]=0xfe;
  display_data[3]=0x54;
  display_data[4]=0x24;
*/

/*
 text_MP6090R("Hello world!",0,0,TRUE,TRUE);
 DWT_Delay(2000000);
 for(i=0;i<MAX_HEIGHT;i++){
     scroll_down_MP6090R(1,TRUE);
     DWT_Delay(2000000);
 }
*/
/*
  for(i=0;i<10;i++){
    digit_MP6090R(i,TRUE);
    delay_ms(2000);
  }
*/
/*
  for(i=0;i<60;i++){
    symbol_MP6090R(i,TRUE,TRUE);
    delay_ms(1000);
    symbol_MP6090R(i,FALSE,TRUE);
    delay_ms(1000);
  }
*/
//  send_dd_MP6090R();
  MP6090R_text("Hello world!",0,0,TRUE,TRUE);

  while(1){

    if((irp=qretrieve())!=0) {
      IR_Decode(irp);
    }

    if(recState==REC_Captured)
      recState=REC_Idle;


    MP6090R_receive();
    /*
    print_hex(key_data[0]);
    text_MP6090R(buf,0,0,TRUE,FALSE);
    print_hex(key_data[1]);
    text_MP6090R(buf,3,0,FALSE,FALSE);
    print_hex(key_data[2]);
    text_MP6090R(buf,6,0,FALSE,FALSE);
    print_hex(key_data[3]);
    text_MP6090R(buf,9,0,FALSE,TRUE);
    */


/*
    if(key_data[0]&KEY_VOL_UP)text_MP6090R("Volume Up",0,0,TRUE,TRUE);
    if(key_data[0]&KEY_VOL_DOWN)text_MP6090R("Volume Down",0,0,TRUE,TRUE);
    if(key_data[0]&KEY_ATT)text_MP6090R("Attention",0,0,TRUE,TRUE);
    if(key_data[0]&KEY_1)text_MP6090R("Button 1",0,0,TRUE,TRUE);
    if(key_data[0]&KEY_2)text_MP6090R("Button 2",0,0,TRUE,TRUE);
    if(key_data[0]&KEY_4)text_MP6090R("Button 4",0,0,TRUE,TRUE);
    if(key_data[0]&KEY_6)text_MP6090R("Button 6",0,0,TRUE,TRUE);

    if(key_data[1]&KEY_3)text_MP6090R("Button 3",0,0,TRUE,TRUE);
    if(key_data[1]&KEY_5)text_MP6090R("Button 5",0,0,TRUE,TRUE);
    if(key_data[1]&KEY_TI)text_MP6090R("TI Vol Adj",0,0,TRUE,TRUE);
    if(key_data[1]&KEY_SRC)text_MP6090R("SRC",0,0,TRUE,TRUE);
    if(key_data[1]&KEY_SEEK_RIGHT)text_MP6090R("Seek Right",0,0,TRUE,TRUE);
    if(key_data[1]&KEY_AM)text_MP6090R("AM",0,0,TRUE,TRUE);
    if(key_data[1]&KEY_FM)text_MP6090R("FM",0,0,TRUE,TRUE);
    if(key_data[1]&KEY_EJECT)text_MP6090R("Eject",0,0,TRUE,TRUE);

    if(key_data[2]&KEY_DISP)text_MP6090R("DISP NAME.S",0,0,TRUE,TRUE);
    if(key_data[2]&KEY_SEEK_LEFT)text_MP6090R("Seek Left",0,0,TRUE,TRUE);
    if(key_data[2]&KEY_Q)text_MP6090R("Q Aud",0,0,TRUE,TRUE);
    if(key_data[2]&KEY_PLAY)text_MP6090R("Play/Pause",0,0,TRUE,TRUE);
    if(key_data[2]&KEY_MINUS)text_MP6090R("Minus",0,0,TRUE,TRUE);
*/

    if(key_data[1]&KEY_SEEK_RIGHT)MP6090R_scroll_right(1,TRUE);
    if(key_data[2]&KEY_SEEK_LEFT)MP6090R_scroll_left(1,TRUE);
    if(key_data[1]&KEY_FM)MP6090R_scroll_up(1,TRUE);
    if(key_data[1]&KEY_AM)MP6090R_scroll_down(1,TRUE);

    if(key_data[0]&KEY_VOL_UP)MP6090R_scroll_right(5,TRUE);
    if(key_data[0]&KEY_VOL_DOWN)MP6090R_scroll_left(5,TRUE);

    if(key_data[2]&KEY_Q)MP6090R_text("Hello world!",0,0,TRUE,TRUE);
    if(key_data[0]&KEY_ATT)MP6090R_text("Attention",0,0,TRUE,TRUE);


    if(key_data[1]&KEY_3)MP6090R_leds_red();
    if(key_data[0]&KEY_4)MP6090R_leds_green();
    if(key_data[1]&KEY_5)MP6090R_leds_off();

    if(key_data[1]&KEY_SRC) {
      if(d) {
        MP6090R_digit_off(TRUE);
        d=0;
      }
      else {
        MP6090R_digit(i,TRUE);
        d=1;
      }
    }

    if(key_data[2]&KEY_MINUS){
      i++;
      if(i<=9) {
        MP6090R_digit(i,TRUE);
      }
      else {
        i=9;
        MP6090R_digit(i,TRUE);
      }
    }

    if(key_data[1]&KEY_EJECT){
      i--;
      if(i>=0) {
        MP6090R_digit(i,TRUE);
      }
      else {
        i=0;
        MP6090R_digit(i,TRUE);
      }
    }

    if(key_data[0]&KEY_1) {
      cp++;
      if(cp>10) cp=10;
      MP6090R_contrast(c[cp]);
      if(cp==10) {
        MP6090R_text("Contrast max",0,0,TRUE,TRUE);
      }
      else {
        MP6090R_text("Contrast",0,0,TRUE,FALSE);
        MP6090R_char(cp+0x30,9,0);
        MP6090R_send();
      }
    }

    if(key_data[0]&KEY_2) {
      cp--;
      if(cp<0) cp=0;
      MP6090R_contrast(c[cp]);
      if(cp==0) {
        MP6090R_text("Contrast min",0,0,TRUE,TRUE);
      }
      else {
        MP6090R_text("Contrast",0,0,TRUE,FALSE);
        MP6090R_char(cp+0x30,9,0);
        MP6090R_send();
      }
    }

    if(key_data[0]&KEY_6){
      if(dim) {
        MP6090R_dimmer(TRUE);
        dim=0;
      }
      else {
        MP6090R_dimmer(FALSE);
        dim=1;
      }
    }

    if(key_data[2]&KEY_PLAY) {
       if(cl) {
        MP6090R_clear_buf(TRUE);
        cl=0;
      }
      else {
        MP6090R_fill_buf(TRUE);
        cl=1;
      }
    }
    if(key_data[1]&KEY_TI) {

      MP6090R_symbol(sp,TRUE,TRUE);
      sp++;
      if(sp>60){
        MP6090R_clear_symbols(TRUE);
        sp=0;
      }
    }
    if(key_data[2]&KEY_DISP) {
      for(i=0;i<60;i++){
        if(s) {
          MP6090R_symbol(i,FALSE,FALSE);
        }
        else {
          MP6090R_symbol(i,TRUE,FALSE);
        }
      }
      s=!s;
      MP6090R_send();
    }

    DWT_Delay(200000);
  }
}
