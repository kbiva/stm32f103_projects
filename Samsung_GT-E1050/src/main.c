/*
 *  main.c
 *
 *  Samsung GT-E1050 LCD demo
 *
 *  Author: Kestutis Bivainis
 *
 *  adapted code from:
 *  https://github.com/majestik666/Beagle_GPIO/blob/master/Beagle_GPIO_Nokia6100_test_3d.cc
 */
 
#include "stm32f10x_conf.h"
#include "delay.h"
#include "hy_stm32.h"
#include "colors.h"
#include "S6B33BG.h"
#include "3d.h"
#include "math.h"
#include "adc.h"

extern int nbPts;
extern float cube[];

extern uint32_t frame[];

void show_3d_cube(void);

extern volatile uint16_t ADCConvertedValue[];

int main (void) {

  uint32_t TempColor[9] = { RED,GREEN,BLUE,CYAN,MAGENTA,YELLOW,BROWN,ORANGE,PINK };
  uint32_t i;

  DWT_Init();

  DWT_Delay(10000);

  BSP_Buttons_Init(BUTTON_MODE_EXTI);
  BSP_LEDs_Init();

  ADC_DMA_Configuration();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  if(S6B33BG_Init(0,1)==S6B33BG_ERROR) {
    BSP_LED_On(LED1);
    while(1);
  }

  S6B33BG_ColorMode(COLOR_16BIT);
  S6B33BG_OrientationMode(ORIENTATION_PORTRAIT);
  S6B33BG_ClearScreen(BLACK);

  S6B33BG_SetFont(FONT_8x14);
  S6B33BG_PutStr("Mobile phone LCD",2,2);
  S6B33BG_PutStr("Samsung GT-E1050",2,14);
  S6B33BG_SetFont(FONT_6x8);
  S6B33BG_PutStr("Resolution:128x128 px",2,30);
  S6B33BG_PutStr("Type:16bit color CSTN",2,40);
  S6B33BG_PutStr("Controller: S6B33BG",2,50);
  S6B33BG_PutStr("Marking on the LCD:",2,60);
  S6B33BG_PutStr(" 2P0427-E(Rev 1.0)",2,70);
  S6B33BG_PutStr(" Y120618-B1F",2,80);
  S6B33BG_PutStr("Marking on the cable:",2,90);
  S6B33BG_PutStr(" TSF8H0784FPC-A1-E G",2,100);
  S6B33BG_PutStr("MPU Interface:",2,110);
  S6B33BG_PutStr(" 8bit 8080 parallel",2,120);

  DWT_Delay(5000000);
  S6B33BG_ClearScreen(BLACK);

  S6B33BG_Fill(0,0,S6B33BG_GetWidth()-1,16,BLACK);
  S6B33BG_Fill(0,17,S6B33BG_GetWidth()-1,32,RED);
  S6B33BG_Fill(0,33,S6B33BG_GetWidth()-1,48,GREEN);
  S6B33BG_Fill(0,49,S6B33BG_GetWidth()-1,64,BLUE);
  S6B33BG_Fill(0,65,S6B33BG_GetWidth()-1,80,CYAN);
  S6B33BG_Fill(0,81,S6B33BG_GetWidth()-1,96,MAGENTA);
  S6B33BG_Fill(0,97,S6B33BG_GetWidth()-1,112,YELLOW);
  S6B33BG_Fill(0,113,S6B33BG_GetWidth()-1,128,WHITE);

  S6B33BG_PutStr("Color Bars",30,6);

  DWT_Delay(5000000);
  S6B33BG_ClearScreen(BLACK);

  S6B33BG_SetFont(FONT_8x8);
  S6B33BG_PutStr("3D rotating",20,12);
  S6B33BG_PutStr("cube and moving",4,24);
  S6B33BG_PutStr("checkerboard",18,36);
  S6B33BG_PutStr("demo",50,48);
  S6B33BG_SetFont(FONT_6x8);
  S6B33BG_PutStr("Delay (0 - 80 ms)",4,62);
  S6B33BG_PutStr("between frames is",4,72);
  S6B33BG_PutStr("controlled by onboard",4,82);
  S6B33BG_PutStr("potentiometer",4,92);

  S6B33BG_PutStr("more at:",8,108);
  S6B33BG_PutStr("kbiva.wordpress.com",8,118);

  for(i=0;i<100;i++) {
    S6B33BG_SetTextColours(TempColor[i%9],BLACK);
    S6B33BG_PutStr("kbiva.wordpress.com",8,118);
    DWT_Delay(100000);
  }


  S6B33BG_ClearScreen(BLACK);
  show_3d_cube();

  while(1){};
}

void show_3d_cube(void){

  int16_t ys=0,xs=0;

  float tp[24];

  int pp[16];

  float camz = 2.0;

  int x,y,xx,yy,j,i=0;

  float cosT;
  float sinT;
  float cosP;
  float sinP;
  float cosTcosP;
  float cosTsinP;
  float sinTcosP;
  float sinTsinP;

  int scaleFactor = MAX_Y_3D/4;
  float near = 3;
  float nearToObj;
  float x0,y0,z0;
  float fac;
  int xo;
  int yo;
  float rotation[3] = { 0.0, 0.0, 0.0 };
  float rot_speed[3] = { 0.04, -0.08, 0.033 };

  while(1) {

    i++;

    for(j=0;j<MAX_X_3D*MAX_Y_3D;j++)
       frame[j]=0;

    // Checkerboard background

    xo = 25.0*sin(10*i/100.0)*cos(6*i/100.0);
    yo = 25.0*sin(5*i/100.0)*cos(7.5*i/100.0);

    for (x=0;x<MAX_Y_3D;x++)
    {
      xx = (50+x+xo)% 32;
      for (y=0;y<MAX_X_3D;++y)
      {
        yy = (50+y+yo)% 32;
        if ((xx<16 && yy<16) || (xx>16 && yy>16))
        {
            frame[x*MAX_Y_3D+y]=WHITE;
        }
      }
    }

    cosT = cos(rotation[0]);
    sinT = sin(rotation[0]);
    cosP = cos(rotation[1]);
    sinP = sin(rotation[1]);
    cosTcosP = cosT*cosP;
    cosTsinP = cosT*sinP;
    sinTcosP = sinT*cosP;
    sinTsinP = sinT*sinP;

    nearToObj = 1.5 + 3.0*sin(5*i/100.0);

    for (j=0;j<nbPts;++j) {

      x0 = cube[3*j+0];
      y0 = cube[3*j+1];
      z0 = cube[3*j+2];

      tp[3*j+0] = cosT*x0 + sinT*z0;
      tp[3*j+1] = -sinTsinP*x0 + cosP*y0 + cosTsinP*z0;
      tp[3*j+2] = camz + cosTcosP*z0 - sinTcosP*x0 - sinP*y0;

      fac = scaleFactor * near * 1.0f / (tp[3*j+2]+near+nearToObj);

      pp[2*j+0] = (int)(MAX_Y_3D/2.0f + fac*tp[3*j+0] + 0.5 );
      pp[2*j+1] = (int)(MAX_Y_3D/2.0f + fac*tp[3*j+1] + 0.5 );
    }

    drawPolygons( tp, pp );

    S6B33BG_FillPixel(xs,ys,xs+MAX_X_3D-1,ys+MAX_Y_3D-1,frame);

    for (j=0;j<3;++j)
      rotation[j] += rot_speed[j];

    DWT_Delay(ADCConvertedValue[1]*20);
  }
}
