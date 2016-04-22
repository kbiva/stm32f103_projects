/*
 *  main.c
 *
 *  Sony Ericsson K800i LCD demo
 *
 *  Author: Kestutis Bivainis
 *
 *  for rotating 3D cube, adapted code from:
 *  https://github.com/majestik666/Beagle_GPIO/blob/master/Beagle_GPIO_Nokia6100_test_3d.cc
 */

#include "stm32f10x_conf.h"
#include "R63400.h"
#include "colors.h"
#include "delay.h"
#include "hy_stm32.h"
#include "3d.h"
#include <math.h>
#include "jpeg.h"
#include <stdio.h>
#include "xorshift.h"
#include "adc.h"
#include "bouncingballs.h"

extern uint8_t p_cherry[];
extern uint8_t p_city[];
extern uint8_t p_feather[];
extern uint8_t p_icelight[];
extern uint8_t p_jellicious[];
extern uint8_t p_light[];
extern uint8_t p_mystic[];
extern uint8_t p_shimmering[];
extern uint8_t p_sparkly[];

extern uint8_t* dataptr;
COLOR_MODE current_color_mode;

extern int nbPts;
extern float cube[];

extern uint32_t frame[];

void show_3d_cube(void);

void setColorMode(COLOR_MODE color_mode) {

  R63400_ColorMode(color_mode);
  current_color_mode=color_mode;
}

void displayOrientation(char* text) {

  uint32_t i,
           max_x=R63400_GetWidth(),
           max_y=R63400_GetHeight();

  R63400_ClearScreen(BLACK);
  R63400_PutStrCentered("TOP",0);
  R63400_PutStrCentered("BOTTOM",max_y-14);
  R63400_PutStr("LEFT",0,max_y/2);
  R63400_PutStr("RIGHT",max_x-8*5,max_y/2);
  R63400_PutStrCentered("Orientation:",max_y/2-16/2);
  R63400_PutStrCentered(text,max_y/2+16/2);
  DWT_Delay(3000000);
  R63400_ClearScreen(BLACK);
  R63400_PutStrCentered("10K colored text lines",max_y/2);
  DWT_Delay(3000000);

  R63400_SetFont(FONT_6x8);
  for(i=0;i<10000;i++) {
    R63400_SetTextColors(xor128()%0xFFFFFF,xor128()%0xFFFFFF);
    R63400_PutStr("This is some text",xor128()%max_x,xor128()%max_y);
  }
  R63400_SetTextColors(WHITE,BLACK);
  R63400_SetFont(FONT_8x14);
  DWT_Delay(3000000);
}

void GradientFill(uint16_t y1,uint16_t y2,uint32_t color_inc) {

  uint32_t i,j,c;

  c=0;
  i=0;

  for(j=0;j<256;j++) {

    R63400_Fill(i,y1,i,y2,c);
    i+=1;
    c+=color_inc;
  }
}

void GradientBars(void) {

  GradientFill(0,                     R63400_GetHeight()/7-1,  0x010000);
  GradientFill(R63400_GetHeight()/7,  R63400_GetHeight()/7*2-1,0x000100);
  GradientFill(R63400_GetHeight()/7*2,R63400_GetHeight()/7*3-1,0x000001);
  GradientFill(R63400_GetHeight()/7*3,R63400_GetHeight()/7*4-1,0x010001);
  GradientFill(R63400_GetHeight()/7*4,R63400_GetHeight()/7*5-1,0x000101);
  GradientFill(R63400_GetHeight()/7*5,R63400_GetHeight()/7*6-1,0x010100);
  GradientFill(R63400_GetHeight()/7*6,R63400_GetHeight()-1,    0x010101);
}

void ColorDepthTest(void) {

  switch(current_color_mode) {
    case COLOR_16BIT:R63400_PutStr("16",270,R63400_GetHeight()/2-16/2);break;
    case COLOR_18BIT:R63400_PutStr("18",270,R63400_GetHeight()/2-16/2);break;
    default:R63400_PutStrCentered("error",0);break;
  }
  R63400_PutStr("bit",270,R63400_GetHeight()/2+16/2);
  GradientBars();
  DWT_Delay(3000000);
}

void JPEGTest(uint8_t* picture) {

  setColorMode(COLOR_18BIT);
  dataptr=picture;
  load_jpg();
  switch(current_color_mode) {
    case COLOR_16BIT:R63400_PutStrCentered("16 bit",0);break;
    case COLOR_18BIT:R63400_PutStrCentered("18 bit",0);break;
    default:R63400_PutStrCentered("error",0);break;
  }
  DWT_Delay(3000000);

  setColorMode(COLOR_16BIT);
  dataptr=picture;
  load_jpg();
  switch(current_color_mode) {
    case COLOR_16BIT:R63400_PutStrCentered("16 bit",0);break;
    case COLOR_18BIT:R63400_PutStrCentered("18 bit",0);break;
    default:R63400_PutStrCentered("error",0);break;
  }
  DWT_Delay(3000000);
}

void MemoryReadTest(void) {

  uint16_t buf1[16];
  char buf[32];

  R63400_ClearScreen(BLACK);
  R63400_SetFont(FONT_8x14);
  R63400_PutStrCentered("Reading from memory",0);
  switch(current_color_mode) {
    case COLOR_16BIT:R63400_PutStrCentered("16bit color depth",14);break;
    case COLOR_18BIT:R63400_PutStrCentered("18bit color depth",14);break;
    default:R63400_PutStrCentered("error",0);break;
  }

  R63400_SetFont(FONT_6x8);

  R63400_PutStr("Reading pixel at 5,5 ... (should be BLACK)",0,40);
  R63400_ReadMemory(5,5,5,5,buf1);
  R63400_PutStr("Read back: ",0,54);
  R63400_SetFont(FONT_8x14);
  sprintf(buf,"%04X",buf1[0]);
  R63400_PutStr(buf,70,50);
  R63400_SetFont(FONT_6x8);

  R63400_PutStr("Setting pixel at 5,5 to RED",0,68);
  R63400_SetPixel(5,5,RED);
  R63400_PutStr("Reading pixel at 5,5 ...",0,82);
  R63400_ReadMemory(5,5,5,5,buf1);
  sprintf(buf,"%04X",buf1[0]);
  R63400_PutStr("Read back: ",0,96);
  R63400_SetFont(FONT_8x14);
  R63400_PutStr(buf,70,92);
  R63400_SetFont(FONT_6x8);

  R63400_PutStr("Setting pixel at 5,5 to GREEN",0,110);
  R63400_SetPixel(5,5,GREEN);
  R63400_PutStr("Reading pixel at 5,5 ...",0,124);
  R63400_ReadMemory(5,5,5,5,buf1);
  sprintf(buf,"%04X",buf1[0]);
  R63400_PutStr("Read back: ",0,138);
  R63400_SetFont(FONT_8x14);
  R63400_PutStr(buf,70,134);
  R63400_SetFont(FONT_6x8);

  R63400_PutStr("Setting pixel at 5,5 to BLUE",0,152);
  R63400_SetPixel(5,5,BLUE);
  R63400_PutStr("Reading pixel at 5,5 ...",0,166);
  R63400_ReadMemory(5,5,5,5,buf1);
  sprintf(buf,"%04X",buf1[0]);
  R63400_PutStr("Read back: ",0,180);
  R63400_SetFont(FONT_8x14);
  R63400_PutStr(buf,70,176);
  R63400_SetFont(FONT_6x8);

  R63400_PutStr("Setting pixel at 5,5 to WHITE",0,194);
  R63400_SetPixel(5,5,WHITE);
  R63400_PutStr("Reading pixel at 5,5 ...",0,208);
  R63400_ReadMemory(5,5,5,5,buf1);
  sprintf(buf,"%04X",buf1[0]);
  R63400_PutStr("Read back: ",0,222);
  R63400_SetFont(FONT_8x14);
  R63400_PutStr(buf,70,218);
  R63400_SetFont(FONT_6x8);

  DWT_Delay(3000000);
}

int main (void) {

  uint32_t i,max_x,max_y,t_start,t_end;
  uint16_t x0,y0,x1,y1,tmp;
  char buf[16];

  DWT_Init();

  //DWT_Delay(1000000);

  BSP_Buttons_Init(BUTTON_MODE_EXTI);
  BSP_LEDs_Init();

  if(R63400_Init(1,3)==R63400_ERROR) {
    BSP_LED_On(LED1);
  }

  DWT_Delay(100000);
  R63400_OrientationMode(ORIENTATION_LANDSCAPE);
  R63400_ClearScreen(BLACK);

  setColorMode(COLOR_18BIT);

  R63400_SetFont(FONT_8x14);
  R63400_PutStrCentered("Sony Ericsson K800i LCD",0);
  R63400_SetFont(FONT_6x8);
  R63400_PutStr("Compatible models: K790i, K810i, W850i",0,25);
  R63400_PutStr("Markings on the back: RHN942268 R1B N6A90382062A3",0,40);
  R63400_PutStr("LCD type: TFT",0,55);
  R63400_PutStr("Screen size: 2.0 inches",0,70);
  R63400_PutStr("Resolution: 320x240 px",0,85);
  R63400_PutStr("Colors: 262k (18 bit/pixel)",0,100);
  R63400_PutStr("Controller: Renesas R63400",0,115);
  R63400_PutStr("Connector: 26pin with 0.4mm pitch",0,130);
  R63400_PutStr("Connector type: Panasonic AXK7L26227G",0,145);
  R63400_PutStr("MPU Interface: 8bit 8080 parallel",0,160);
  R63400_Fill(0,175,R63400_GetWidth()-1,175,WHITE);
  R63400_PutStr("There is visible water damage under metal plate",0,190);
  R63400_PutStr("that is used for backlight. It doesn't affect",0,205);
  R63400_PutStr("LCD operation.",0,220);
  DWT_Delay(7000000);
  R63400_ClearScreen(BLACK);
  R63400_SetFont(FONT_8x14);
  R63400_PutStrCentered("Color mode used in this demo",R63400_GetHeight()/2-16/2);
  R63400_PutStrCentered("is 262k (18 bit/pixel)",R63400_GetHeight()/2+16/2);
  DWT_Delay(3000000);

  R63400_ClearScreen(BLACK);
  R63400_PutStrCentered("Color test",0);
  R63400_PutStrCentered("Basic colors",R63400_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  R63400_ClearScreen(RED);
  R63400_PutStrCentered("Red",R63400_GetHeight()/2-8/2);
  DWT_Delay(1000000);
  R63400_ClearScreen(GREEN);
  R63400_PutStrCentered("Green",R63400_GetHeight()/2-8/2);
  DWT_Delay(1000000);
  R63400_ClearScreen(BLUE);
  R63400_PutStrCentered("Blue",R63400_GetHeight()/2-8/2);
  DWT_Delay(1000000);
  R63400_ClearScreen(CYAN);
  R63400_PutStrCentered("Cyan",R63400_GetHeight()/2-8/2);
  DWT_Delay(1000000);
  R63400_ClearScreen(MAGENTA);
  R63400_PutStrCentered("Magenta",R63400_GetHeight()/2-8/2);
  DWT_Delay(1000000);
  R63400_ClearScreen(YELLOW);
  R63400_PutStrCentered("Yellow",R63400_GetHeight()/2-8/2);
  DWT_Delay(1000000);

  R63400_ClearScreen(BLACK);
  R63400_PutStrCentered("Color test",0);
  R63400_PutStrCentered("Gradient with basic colors",R63400_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  R63400_ClearScreen(BLACK);
  GradientBars();
  DWT_Delay(3000000);

  R63400_ClearScreen(BLACK);
  R63400_SetFont(FONT_8x14);
  R63400_PutStrCentered("Color depth test",0);
  R63400_PutStrCentered("Gradient color bars are displayed",R63400_GetHeight()/2-16/2);
  R63400_PutStrCentered("at different LCD color depth",R63400_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  R63400_ClearScreen(BLACK);
  ColorDepthTest();
  R63400_ClearScreen(BLACK);
  setColorMode(COLOR_16BIT);
  ColorDepthTest();
  R63400_ClearScreen(BLACK);
  setColorMode(COLOR_18BIT);
  ColorDepthTest();

  R63400_ClearScreen(BLACK);
  R63400_SetFont(FONT_8x14);
  R63400_PutStrCentered("JPEG pictures test",0);
  R63400_PutStrCentered("The same jpeg picture is displayed",R63400_GetHeight()/2-16/2);
  R63400_PutStrCentered("at different LCD color depth",R63400_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  JPEGTest(p_cherry);
  JPEGTest(p_city);
  JPEGTest(p_feather);
  JPEGTest(p_icelight);
  JPEGTest(p_jellicious);
  JPEGTest(p_light);
  JPEGTest(p_mystic);
  JPEGTest(p_shimmering);
  JPEGTest(p_sparkly);

  setColorMode(COLOR_18BIT);
  R63400_ClearScreen(BLACK);
  R63400_SetFont(FONT_8x14);
  R63400_PutStrCentered("Reading from display memory",0);
  R63400_PutStrCentered("(read is always 16bit/px)",R63400_GetHeight()/2);
  DWT_Delay(3000000);

  MemoryReadTest();
  setColorMode(COLOR_16BIT);
  MemoryReadTest();
  setColorMode(COLOR_18BIT);

  R63400_ClearScreen(BLACK);
  R63400_SetFont(FONT_8x14);
  R63400_PutStrCentered("Orientation test",0);
  DWT_Delay(3000000);

  R63400_OrientationMode(ORIENTATION_LANDSCAPE);
  displayOrientation("Lansdcape");
  R63400_OrientationMode(ORIENTATION_PORTRAIT);
  displayOrientation("Portrait");
  R63400_OrientationMode(ORIENTATION_PORTRAIT_REV);
  displayOrientation("Portrait reversed");
  R63400_OrientationMode(ORIENTATION_LANDSCAPE_REV);
  displayOrientation("Lansdcape reversed");

  R63400_ClearScreen(BLACK);
  R63400_OrientationMode(ORIENTATION_LANDSCAPE);

  max_x=R63400_GetWidth();
  max_y=R63400_GetHeight();

  R63400_ClearScreen(BLACK);
  R63400_PutStrCentered("Rectangles",0);
  R63400_PutStrCentered("1000 colored rectangles",R63400_GetHeight()/2);
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

    R63400_Fill(x0,y0,x1,y1,xor128()%0xFFFFFF);
  }
  t_end=DWT_Get();
  sprintf(buf,"in %d ms",(t_end-t_start)/(SystemCoreClock/1000));
  R63400_ClearScreen(BLACK);
  R63400_PutStrCentered("Rectangles",0);
  R63400_PutStrCentered("1000 colored rectangles",R63400_GetHeight()/2);
  R63400_PutStrCentered(buf,R63400_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  setColorMode(COLOR_16BIT);
  R63400_ClearScreen(BLACK);
  R63400_PutStrCentered("Bouncing balls",0);
  R63400_PutStrCentered("Ball count is controlled with",R63400_GetHeight()/2-16/2);
  R63400_PutStrCentered("an onboard potentiometer",R63400_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  R63400_ClearScreen(BLACK);

  ADC_DMA_Configuration();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  bouncing_balls();

  R63400_ClearScreen(BLACK);
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

  uint32_t max_x=R63400_GetWidth(),
           max_y=R63400_GetHeight();

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

  R63400_PutStrCentered("Sony Ericsson K800i LCD-Rotating 3D cube",0);
  R63400_PutStrCentered("kbiva.wordpress.com",max_y-16);

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

    R63400_FillPixel(xs,ys,xs+MAX_X_3D-1,ys+MAX_Y_3D-1,frame);

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
