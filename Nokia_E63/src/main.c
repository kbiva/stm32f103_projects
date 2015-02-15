/*
 *  main.c
 *
 *  Nokia E63 LCD demo
 *
 *  Author: Kestutis Bivainis
 *
 *  for rotating 3D cube, adapted code from:
 *  https://github.com/majestik666/Beagle_GPIO/blob/master/Beagle_GPIO_Nokia6100_test_3d.cc
 */

#include "stm32f10x_conf.h"
#include "MC2PA8201.h"
#include "colors.h"
#include "delay.h"
#include "hy_stm32.h"
#include "3d.h"
#include "math.h"
#include <stdio.h>

extern int nbPts;
extern float cube[];

extern uint32_t frame[];

void show_3d_cube(void);

static uint32_t x;
static uint32_t y;
static uint32_t z;
static uint32_t w;
// Xorshift pseudorandom number generator
uint32_t xor128(void) {

  uint32_t t;

  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

void displayOrientation(char* text) {

  uint32_t i,
           max_x=MC2PA8201_GetWidth(),
           max_y=MC2PA8201_GetHeight();

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("TOP",0);
  MC2PA8201_PutStrCentered("BOTTOM",max_y-14);
  MC2PA8201_PutStr("LEFT",0,max_y/2);
  MC2PA8201_PutStr("RIGTH",max_x-8*5,max_y/2);
  MC2PA8201_PutStrCentered("Orientation:",max_y/2-16/2);
  MC2PA8201_PutStrCentered(text,max_y/2+16/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("10K colored text lines",max_y/2);
  DWT_Delay(3000000);

  MC2PA8201_SetFont(FONT_6x8);
  for(i=0;i<10000;i++) {
    MC2PA8201_SetTextColors(xor128()%0xFFFFFF,xor128()%0xFFFFFF);
    MC2PA8201_PutStr("This is some text",xor128()%max_x,xor128()%max_y);
  }
  MC2PA8201_SetTextColors(WHITE,BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  DWT_Delay(2000000);
}

void GradientFill(uint16_t y1,uint16_t y2,uint32_t color_inc) {

  uint32_t i,j,c;

  c=0;
  i=0;

  for(j=0;j<256;j++) {

    MC2PA8201_Fill(i,y1,i,y2,c);
    i+=1;
    c+=color_inc;
  }
}

int main (void) {

  uint32_t i,max_x,max_y,t_start,t_end;
  uint16_t x0,y0,x1,y1,tmp;
  char buf[16];

  // reset pseudo-random number generator
  x = 123456789;
  y = 362436069;
  z = 521288629;
  w = 88675123;

  DWT_Init();

  DWT_Delay(2000000);

  BSP_Buttons_Init(BUTTON_MODE_EXTI);

  MC2PA8201_Init();
  MC2PA8201_ColorMode(COLOR_24BIT);
  MC2PA8201_OrientationMode(ORIENTATION_LANDSCAPE);
  MC2PA8201_ClearScreen(BLACK);

  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Nokia E63 LCD",0);
  MC2PA8201_SetFont(FONT_6x8);
  MC2PA8201_PutStr("Markings on the back:",0,25);
  MC2PA8201_PutStr("4850118 MP1.3 LS024Q8UX05C 0111R08353DE",0,35);
  MC2PA8201_PutStr("LCD manufacturer: Sharp",0,55);
  MC2PA8201_PutStr("LCD type: TFT",0,75);
  MC2PA8201_PutStr("Screen size: 2.36 inches",0,95);
  MC2PA8201_PutStr("Resolution: 320x240 px",0,115);
  MC2PA8201_PutStr("Colors: 16M (24 bit/pixel)",0,135);
  MC2PA8201_PutStr("Controller: MagnaChip MC2PA8201 compatible",0,155);
  MC2PA8201_PutStr("Connector: 24pin 24R-JANK-GSAN-TF with 0.4mm pitch",0,175);
  MC2PA8201_PutStr("Backlight: 4 LEDs",0,195);
  MC2PA8201_PutStr("MPU Interface: 8bit 8080 parallel",0,215);
  DWT_Delay(5000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Color mode used in this demo",MC2PA8201_GetHeight()/2);
  MC2PA8201_PutStrCentered("is 16M (24 bit/pixel)",MC2PA8201_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Orientation test",0);
  DWT_Delay(3000000);

  MC2PA8201_OrientationMode(ORIENTATION_LANDSCAPE);
  displayOrientation("Lansdcape");
  MC2PA8201_OrientationMode(ORIENTATION_PORTRAIT);
  displayOrientation("Portrait");
  MC2PA8201_OrientationMode(ORIENTATION_PORTRAIT_REV);
  displayOrientation("Portrait reversed");
  MC2PA8201_OrientationMode(ORIENTATION_LANDSCAPE_REV);
  displayOrientation("Lansdcape reversed");

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_OrientationMode(ORIENTATION_LANDSCAPE);
  MC2PA8201_PutStrCentered("Sleep mode test",0);
  MC2PA8201_PutStrCentered("Sleeping for 3 seconds",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_Sleep();
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Woken up",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Wakeup();
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Display on/off test",0);
  MC2PA8201_PutStrCentered("Display off for 3 seconds",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_DisplayOff();
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Display on",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_DisplayOn();
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Gamma curves test",0);
  MC2PA8201_PutStrCentered("There are 4 predefined gamma curves",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("Default is gamma curve 1",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x010000);
  MC2PA8201_PutStrCentered("Gamma curve 1 (2.2)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE1);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 2 (1.8)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE2);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 3 (2.5)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE3);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 4 (1.0)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE4);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_Gamma(GAMMA_CURVE1);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x0100);
  MC2PA8201_PutStrCentered("Gamma curve 1 (2.2)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE1);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 2 (1.8)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE2);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 3 (2.5)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE3);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 4 (1.0)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE4);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_Gamma(GAMMA_CURVE1);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x01);
  MC2PA8201_PutStrCentered("Gamma curve 1 (2.2)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE1);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 2 (1.8)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE2);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 3 (2.5)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE3);
  DWT_Delay(3000000);
  MC2PA8201_PutStrCentered("Gamma curve 4 (1.0)",MC2PA8201_GetHeight()/2-8/2);
  MC2PA8201_Gamma(GAMMA_CURVE4);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_Gamma(GAMMA_CURVE1);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Color test",0);
  MC2PA8201_PutStrCentered("Basic colors",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(RED);
  MC2PA8201_PutStrCentered("Red",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(2000000);
  MC2PA8201_ClearScreen(GREEN);
  MC2PA8201_PutStrCentered("Green",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(2000000);
  MC2PA8201_ClearScreen(BLUE);
  MC2PA8201_PutStrCentered("Blue",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(2000000);
  MC2PA8201_ClearScreen(CYAN);
  MC2PA8201_PutStrCentered("Cyan",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(2000000);
  MC2PA8201_ClearScreen(MAGENTA);
  MC2PA8201_PutStrCentered("Magenta",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(2000000);
  MC2PA8201_ClearScreen(YELLOW);
  MC2PA8201_PutStrCentered("Yellow",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(2000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Color test",0);
  MC2PA8201_PutStrCentered("Gradient with basic colors",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(5000000);

  MC2PA8201_ClearScreen(BLACK);
  GradientFill(0,                        MC2PA8201_GetHeight()/7-1,  0x010000);
  GradientFill(MC2PA8201_GetHeight()/7,  MC2PA8201_GetHeight()/7*2-1,0x000100);
  GradientFill(MC2PA8201_GetHeight()/7*2,MC2PA8201_GetHeight()/7*3-1,0x000001);
  GradientFill(MC2PA8201_GetHeight()/7*3,MC2PA8201_GetHeight()/7*4-1,0x010001);
  GradientFill(MC2PA8201_GetHeight()/7*4,MC2PA8201_GetHeight()/7*5-1,0x000101);
  GradientFill(MC2PA8201_GetHeight()/7*5,MC2PA8201_GetHeight()/7*6-1,0x010100);
  GradientFill(MC2PA8201_GetHeight()/7*6,MC2PA8201_GetHeight()-1,    0x010101);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Idle mode test",0);
  MC2PA8201_PutStrCentered("In idle mode color space",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("is reduced to 8 colors",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);

  GradientFill(0,                        MC2PA8201_GetHeight()/7-1,  0x010000);
  GradientFill(MC2PA8201_GetHeight()/7,  MC2PA8201_GetHeight()/7*2-1,0x000100);
  GradientFill(MC2PA8201_GetHeight()/7*2,MC2PA8201_GetHeight()/7*3-1,0x000001);
  GradientFill(MC2PA8201_GetHeight()/7*3,MC2PA8201_GetHeight()/7*4-1,0x010001);
  GradientFill(MC2PA8201_GetHeight()/7*4,MC2PA8201_GetHeight()/7*5-1,0x000101);
  GradientFill(MC2PA8201_GetHeight()/7*5,MC2PA8201_GetHeight()/7*6-1,0x010100);
  GradientFill(MC2PA8201_GetHeight()/7*6,MC2PA8201_GetHeight()-1,    0x010101);
  MC2PA8201_PutStrCentered("Idle mode off",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_IdleModeOn();
  MC2PA8201_PutStrCentered("Idle mode  on",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_IdleModeOff();
  MC2PA8201_PutStrCentered("Idle mode off",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  max_x=MC2PA8201_GetWidth();
  max_y=MC2PA8201_GetHeight();

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Rectangles",0);
  MC2PA8201_PutStrCentered("1000 colored rectangles",MC2PA8201_GetHeight()/2);
  DWT_Delay(3000000);

  t_start=DWT_Get();
  for(i=0;i<1000;i++) {
    x0=xor128()%max_x;
    x1=xor128()%max_x;
    y0=xor128()%max_y;
    y1=xor128()%max_y;
    if(x0>x1) {
      tmp=x0;
      x0=x1;
      x1=tmp;
    }
    if(y0>y1) {
      tmp=y0;
      y0=y1;
      y1=tmp;
    }

    MC2PA8201_Fill(x0,y0,x1,y1,xor128()%0xFFFFFF);
  }
  t_end=DWT_Get();
  sprintf(buf,"in %d ms",(t_end-t_start)/(SystemCoreClock/1000));
  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Rectangles",0);
  MC2PA8201_PutStrCentered("1000 colored rectangles",MC2PA8201_GetHeight()/2);
  MC2PA8201_PutStrCentered(buf,MC2PA8201_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  show_3d_cube();
  while(1) {};
}

void show_3d_cube(void){
  //////////////////////////////////////////////////////////////////////////
  // 3D CUBE
  //////////////////////////////////////////////////////////////////////////
  // adapted code from:
  // https://github.com/majestik666/Beagle_GPIO/blob/master/Beagle_GPIO_Nokia6100_test_3d.cc

  int16_t ys=16,xs=0;
  uint8_t ysv=0,xsv=0;
  float tp[24];

  int pp[16];

  float camz = 2.0;

  int j,i=0;

  uint32_t max_x=MC2PA8201_GetWidth(),
           max_y=MC2PA8201_GetHeight();

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

  float rotation[3] = { 0.0, 0.0, 0.0 };
  float rot_speed[3] = { 0.04, -0.08, 0.033 };

  MC2PA8201_PutStrCentered("Nokia E63 LCD - Rotating 3D cube",0);
  MC2PA8201_PutStrCentered("kbiva.wordpress.com",max_y-16);

  while(1) {

    i++;

    for(j=0;j<MAX_X_3D*MAX_Y_3D;j++)
       frame[j]=0;

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

    MC2PA8201_FillPixel(xs,ys,xs+MAX_X_3D-1,ys+MAX_Y_3D-1,frame);

    ysv?ys--:ys++;

    if(ys>(int)(max_y-MAX_Y_3D-16)) {
      ysv=1;
      ys--;
    }
    if(ys<16) {
      ysv=0;
      ys++;
    }

    xsv?xs--:xs++;

    if(xs>(int)(max_x-MAX_X_3D)) {
      xsv=1;
      xs--;
    }
    if(xs<0) {
      xsv=0;
      xs++;
    }

    for (j=0;j<3;++j)
      rotation[j] += rot_speed[j];
  }
}
