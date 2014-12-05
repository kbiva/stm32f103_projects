/*
 *  main.c
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "m66004.h"
#include <string.h>

volatile uint32_t millis;

extern uint16_t inputVal;
extern uint8_t inputValues;

uint8_t openHeart[8] = {0x00,0x0A,0x15,0x11,0x0A,0x04,0x00,0x00};
uint8_t filledHeart[8] = {0x00,0x0A,0x1F,0x1F,0x0E,0x04,0x00,0x00};

uint8_t pat0[9]={0x17,0x15,0x15,0x15,0x17,0x00,0x1f,0x00};
uint8_t pat1[9]={0xa,0xa,0xa,0xa,0xa,0x0,0x1f};
uint8_t pat2[9]={0x17,0x11,0x17,0x14,0x17,0x0,0x1f};
uint8_t pat3[9]={0x17,0x11,0x13,0x11,0x17,0x0,0x1f};
uint8_t pat4[9]={0x15,0x15,0x17,0x11,0x11,0x0,0x1f};
uint8_t pat5[9]={0x17,0x14,0x17,0x11,0x17,0x0,0x1f};
uint8_t pat6[9]={0x17,0x14,0x17,0x15,0x17,0x0,0x1f};
uint8_t pat7[9]={0x17,0x11,0x12,0x12,0x12,0x0,0x1f};
uint8_t pat8[9]={0x17,0x15,0x17,0x15,0x17,0x0,0x1f};
uint8_t pat9[9]={0x17,0x15,0x17,0x11,0x17,0x0,0x1f};
uint8_t pat10[9]={0x0,0xf,0x3,0x5,0x9,0x10,0x0};
uint8_t pat11[9]={0x0,0x1e,0x18,0x14,0x12,0x1,0x0};
uint8_t pat12[9]={0x0,0x1,0x12,0x14,0x18,0x1e,0x0};
uint8_t pat13[9]={0x0,0x10,0x9,0x5,0x3,0xf,0x0};
uint8_t pat14[9]={0x0,0x0,0xa,0x0,0x11,0xe,0x0};
uint8_t pat15[9]={0x0,0x0,0xa,0x0,0xe,0x11,0x0};


char buf[16];
/*
void printBits(uint16_t value, uint8_t bitCount) {

  uint16_t mask;
  for (mask = (1<<(uint16_t)(bitCount-1)); mask; mask >>= 1) {
    write((value & mask) ? 1 : 0);
  }
}
*/

void Int2Str(char* str, int32_t intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

int main(void) {

  uint16_t i=0,j;
  uint8_t dim=DIMMER_14_16,under=0,first=5,disp=1;

  DWT_Init();
  DWT_Delay(100000);

  SysTick_Config(SystemCoreClock/1000);

  GPIO_Configuration();

  if(!HP_5N_attached()) {
    while(1){};
  }

  HP_5N_init(16);

  M66004_createChar(0, openHeart);
  M66004_createChar(1, filledHeart);
/*
  createChar(0, pat0);
  createChar(1, pat1);
  createChar(2, pat2);
  createChar(3, pat3);
  createChar(4, pat4);
  createChar(5, pat5);
  createChar(6, pat6);
  createChar(7, pat7);
  createChar(8, pat8);
  createChar(9, pat9);
  createChar(10, pat10);
  createChar(11, pat11);
  createChar(12, pat12);
  createChar(13, pat13);
  createChar(14, pat14);
  createChar(15, pat15);
*/
  M66004_clear(1);
/*
  writeAt(0,0);
  writeAt(1,1);
  writeAt(2,2);
  writeAt(3,3);
  writeAt(4,4);
  writeAt(5,5);
  writeAt(6,6);
  writeAt(7,7);
  writeAt(8,8);
  writeAt(9,9);
  writeAt(10,10);
  writeAt(11,11);
  writeAt(12,12);
  writeAt(13,13);
  writeAt(14,14);
  writeAt(15,15);
  while(1){};
*/
  //for(j=0;j<DISPLAY_COLUMNS;j++)
    //noUnderscore(j);


  M66004_printAt(0,0,"kbiva.wordpress.");
  DWT_Delay(2000000);
  M66004_printAt(0,0,"biva.wordpress.c");
  DWT_Delay(500000);
  M66004_printAt(0,0,"iva.wordpress.co");
  DWT_Delay(500000);
  M66004_printAt(0,0,"va.wordpress.com");
  DWT_Delay(2000000);
  M66004_printAt(0,0,"iva.wordpress.co");
  DWT_Delay(500000);
  M66004_printAt(0,0,"biva.wordpress.c");
  DWT_Delay(500000);
  M66004_printAt(0,0,"kbiva.wordpress.");
  DWT_Delay(2000000);

  M66004_clear(1);
//  for(j=0;j<DISPLAY_COLUMNS;j++)
//    noUnderscore(j);

/*
  while(1){
     if((oldinput!=inputValues) && (inputValues!=0) && (inputValues!=0xFF) ) {
       oldinput=inputValues;
        printAt(0,0,"                ");
       //clear();
       //sendByte(NOP);
       //printBits(inputValues,8);
       //DWT_Delay(1000);
       memset(buf,0,16);
       Int2Str(buf,oldinput);
       printAt(0,0,buf);
       DWT_Delay(10000);

     }
     sendByte(0xF1);
   }
*/


  M66004_printAt(0,1,"Button test");
  DWT_Delay(2000000);

  while(first){

    M66004_displayCharAt(15,((millis % 1000) < 250) ? 1 : 0);

    switch(inputVal){
      case HP5N_BUTTON_ITEM_RIGHT:
        if(i!=HP5N_BUTTON_ITEM_RIGHT) {
          M66004_printAt(0,1,"Middle >");
          M66004_underscore(under);
          under++;
          if(under==16)under=15;
          i=HP5N_BUTTON_ITEM_RIGHT;
        }
        break;
      case HP5N_BUTTON_SELECT:
        if(i!=HP5N_BUTTON_SELECT) {
          if(disp==0) M66004_display();
          else if(disp==1) M66004_allDisplay();
          else if(disp==2) M66004_noDisplay();
          disp++;
          if(disp==3) disp=0;
          i=HP5N_BUTTON_SELECT;
        }
        break;
      case HP5N_BUTTON_VALUE_MINUS:
        if(i!=HP5N_BUTTON_VALUE_MINUS) {
          if(dim>0) {
            dim--;
          }
          M66004_dimmer(dim);
          M66004_printAt(0,1,"Dimmer");
          for(j=0;j<sizeof(buf);j++)
            buf[j]=0;
          Int2Str(buf,dim);
          M66004_printAt(7,1,buf);
          i=HP5N_BUTTON_VALUE_MINUS;
        }
        break;
      case HP5N_BUTTON_ITEM_LEFT:
        if(i!=HP5N_BUTTON_ITEM_LEFT) {
          M66004_printAt(0,1,"Middle <");
          M66004_noUnderscore(under);
          under--;
          if(under==255)under=0;
          i=HP5N_BUTTON_ITEM_LEFT;
        }
        break;
      case HP5N_BUTTON_GO:
        if(i!=HP5N_BUTTON_GO) {
          M66004_printAt(0,1,"Big green");
          i=HP5N_BUTTON_GO;
          first--;
        }
        break;
      case HP5N_BUTTON_MENU_LEFT:
        if(i!=HP5N_BUTTON_MENU_LEFT) {
          M66004_printAt(0,1,"Top <");
          i=HP5N_BUTTON_MENU_LEFT;
        }
        break;
      case HP5N_BUTTON_VALUE_PLUS:
        if(i!=HP5N_BUTTON_VALUE_PLUS) {
          if(dim<7) {
            dim++;
          }
          M66004_dimmer(dim);
          M66004_printAt(0,1,"Dimmer");
          for(j=0;j<sizeof(buf);j++)
            buf[j]=0;
          Int2Str(buf,dim);
          M66004_printAt(7,1,buf);
          i=HP5N_BUTTON_VALUE_PLUS;
        }
        break;
      case HP5N_BUTTON_MENU_RIGHT:
        if(i!=HP5N_BUTTON_MENU_RIGHT) {
          M66004_printAt(0,1,"Top >");
          i=HP5N_BUTTON_MENU_RIGHT;
        }
        break;
      case HP5N_BUTTON_JOB_CANCEL:
        if(i!=HP5N_BUTTON_JOB_CANCEL) {
          M66004_printAt(0,1,"Small red");
          i=HP5N_BUTTON_JOB_CANCEL;
        }
        break;
      default:
        if(i) {
          DWT_Delay(100000);
          M66004_clear(0);
          i=0;
        }
        break;
    }
    DWT_Delay(100000);

  }

  M66004_clear(1);

  M66004_printAt(0,0,"Hi world!");
  millis=0;
  while(1){

    M66004_displayCharAt(15,((millis % 1000) < 250) ? 1 : 0);

    Int2Str(buf,millis);
    M66004_printAt(9,0,buf);
    DWT_Delay(10000);
  }
}

