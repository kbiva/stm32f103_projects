/*
 *  main.c
 *
 *  Samsung E700 LCD demo
 *
 *  Author: Kestutis Bivainis
 *
 *  adapted code from:
 *  https://github.com/majestik666/Beagle_GPIO/blob/master/Beagle_GPIO_Nokia6100_test_3d.cc
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "HD66773R.h"
#include "colors.h"
#include "hy_stm32.h"
#include <math.h>
#include "3d.h"

extern int nbPts;
extern float cube[];

extern uint32_t frame[];

void show_3d_cube(void);

void Gradient(uint32_t color_inc,uint32_t cnt,uint32_t px_inc,uint32_t delay) {

  uint32_t i,j,c;

  c=0;
  i=32;
  for(j=0;j<cnt;j++){
    HD66773R_Fill(0,i,HD66773R_GetWidth()-1,i+px_inc-1,c);
    i+=px_inc;
    c+=color_inc;
  }
  DWT_Delay(delay);
  c=0;
  i=160;
  for(j=0;j<cnt;j++){
    HD66773R_Fill(0,i-px_inc,HD66773R_GetWidth()-1,i-1,c);
    i-=px_inc;
    c+=color_inc;
  }
  DWT_Delay(delay);
  c=0;
  i=0;
  for(j=0;j<cnt;j++){
    HD66773R_Fill(i,32,i+px_inc-1,HD66773R_GetHeight()-1,c);
    i+=px_inc;
    c+=color_inc;
  }
  DWT_Delay(delay);
  c=0;
  i=128;
  for(j=0;j<cnt;j++){
    HD66773R_Fill(i-px_inc,32,i-1,HD66773R_GetHeight()-1,c);
    i-=px_inc;
    c+=color_inc;
  }
  DWT_Delay(delay);
}

int main(void) {

  DWT_Init();

  //DWT_Delay(120000);

  BSP_Buttons_Init(BUTTON_MODE_EXTI);
  BSP_LEDs_Init();

  if(HD66773R_Init()==HD66773R_ERROR){
    BSP_LED_On(LED4);
    while(1);
  }

  //HD66773R_OrientationMode(ORIENTATION_LANDSCAPE);
  //HD66773R_OrientationMode(ORIENTATION_PORTRAIT);
  //HD66773R_OrientationMode(ORIENTATION_LANDSCAPE_REV);
  HD66773R_OrientationMode(ORIENTATION_PORTRAIT_REV);

  HD66773R_ClearScreen(BLACK);

  HD66773R_SetFont(FONT_8x16);
  HD66773R_PutStr("Mobile phone LCD",0,2);
  HD66773R_PutStr("Samsung E700",12,14);
  HD66773R_SetFont(FONT_6x8);
  HD66773R_PutStr("Type: 1.8\" a-Si TFT",0,30);
  HD66773R_PutStr("active matrix",0,40);
  HD66773R_PutStr("Resolution:128x160 px",0,50);
  HD66773R_PutStr("Colors: 65K (16bit)",0,60);
  HD66773R_PutStr("Controller:",0,70);
  HD66773R_PutStr("Renesas HD66773R",0,80);
  HD66773R_PutStr("LCD:",0,90);
  HD66773R_PutStr("Samsung LTS180S3-HF1",0,100);
  HD66773R_PutStr("Connector: 27pin FPC",0,110);
  HD66773R_PutStr("with 0.6mm pitch",0,120);
  HD66773R_PutStr("Backlight: 3 LEDs",0,130);
  HD66773R_PutStr("MPU Interface:",0,140);
  HD66773R_PutStr("16bit 8080 parallel",0,150);

  DWT_Delay(10000000);
  HD66773R_ClearScreen(BLACK);

  HD66773R_PutStr("Accidentally spilled",0,0);
  HD66773R_PutStr("liquid soldering flux",0,10);
  HD66773R_PutStr("on the back of LCD :(",0,20);
  HD66773R_PutStr("The metal plate that",0,30);
  HD66773R_PutStr("reflects light from",0,40);
  HD66773R_PutStr("the LEDs isn't clean",0,50);
  HD66773R_PutStr("anymore. LCD works",0,60);
  HD66773R_PutStr("but backlight is not",0,70);
  HD66773R_PutStr("nice and not uniform",0,80);
  HD66773R_SetFont(FONT_8x16);
  HD66773R_PutStr("Demonstation of",0,88);
  HD66773R_PutStr("soldering flux",0,102);
  HD66773R_PutStr("beneath the LCD:",0,116);
  HD66773R_PutStr("best visible on",0,130);
  HD66773R_PutStr("white background",0,144);
  HD66773R_SetFont(FONT_6x8);

  DWT_Delay(10000000);
  HD66773R_ClearScreen(WHITE);
  DWT_Delay(5000000);
  HD66773R_ClearScreen(BLACK);

  HD66773R_Fill(0,0,HD66773R_GetWidth()-1,9,BLACK);
  HD66773R_Fill(0,10,HD66773R_GetWidth()-1,19,0xc0c0c0);
  HD66773R_Fill(0,20,HD66773R_GetWidth()-1,29,0x808080);
  HD66773R_Fill(0,30,HD66773R_GetWidth()-1,39,WHITE);
  HD66773R_Fill(0,40,HD66773R_GetWidth()-1,49,MAROON);
  HD66773R_Fill(0,50,HD66773R_GetWidth()-1,59,RED);
  HD66773R_Fill(0,60,HD66773R_GetWidth()-1,69,PURPLE);
  HD66773R_Fill(0,70,HD66773R_GetWidth()-1,79,MAGENTA);
  HD66773R_Fill(0,80,HD66773R_GetWidth()-1,89,0x008000);
  HD66773R_Fill(0,90,HD66773R_GetWidth()-1,99,GREEN);
  HD66773R_Fill(0,100,HD66773R_GetWidth()-1,109,0x808000);
  HD66773R_Fill(0,110,HD66773R_GetWidth()-1,119,YELLOW);
  HD66773R_Fill(0,120,HD66773R_GetWidth()-1,129,NAVY);
  HD66773R_Fill(0,130,HD66773R_GetWidth()-1,139,BLUE);
  HD66773R_Fill(0,140,HD66773R_GetWidth()-1,149,0x008080);
  HD66773R_Fill(0,150,HD66773R_GetWidth()-1,159,CYAN);

  HD66773R_PutStr("Color Bars",30,0);

  DWT_Delay(5000000);
  HD66773R_ClearScreen(BLACK);

  HD66773R_SetFont(FONT_8x16);
  HD66773R_PutStr("Blue gradient",10,8);
  HD66773R_Fill(0,31,HD66773R_GetWidth()-1,31,WHITE);
  Gradient(0x08,32,4,2000000);

  HD66773R_ClearScreen(BLACK);

  HD66773R_PutStr("Green gradient",8,8);
  HD66773R_Fill(0,31,HD66773R_GetWidth()-1,31,WHITE);
  Gradient(0x0400,64,2,2000000);

  HD66773R_ClearScreen(BLACK);

  HD66773R_PutStr("Red gradient",16,8);
  HD66773R_Fill(0,31,HD66773R_GetWidth()-1,31,WHITE);
  Gradient(0x080000,32,4,2000000);

  HD66773R_ClearScreen(BLACK);

  HD66773R_SetFont(FONT_6x8);
  HD66773R_PutStr("3D rotating cube and",4,0);
  HD66773R_PutStr("moving checkerboard",8,10);
  HD66773R_PutStr("kbiva.wordpress.com",8,150);

  show_3d_cube();

  while(1){};

}

void show_3d_cube(void){
  //////////////////////////////////////////////////////////////////////////
  // 3D CUBE
  //////////////////////////////////////////////////////////////////////////
  // adapted code from:
  // https://github.com/majestik666/Beagle_GPIO/blob/master/Beagle_GPIO_Nokia6100_test_3d.cc

  int16_t ys=24,xs=8;

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

    HD66773R_FillPixel(xs,ys,xs+MAX_X_3D-1,ys+MAX_Y_3D-1,frame);

    for (j=0;j<3;++j)
      rotation[j] += rot_speed[j];

  }
}
