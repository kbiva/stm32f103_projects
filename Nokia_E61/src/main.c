/*
 *  main.c
 *
 *  Nokia E61 LCD demo
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
#include <math.h>
#include "jpeg.h"
#include <stdio.h>
#include "xorshift.h"

extern uint8_t data[];
extern uint8_t data1[];
extern uint8_t data2[];
extern uint8_t data3[];
extern uint8_t data4[];
extern uint8_t data5[];
extern uint8_t data6[];
extern uint8_t data7[];
extern uint8_t data8[];
extern uint8_t data9[];
extern uint8_t data10[];
extern uint8_t data11[];
extern uint8_t data12[];
extern uint8_t data13[];
extern uint8_t data14[];
extern uint8_t data15[];
extern uint8_t data16[];
extern uint8_t data17[];
extern uint8_t data18[];
extern uint8_t data28[];

extern uint8_t* dataptr;
COLOR_MODE current_color_mode;

extern int nbPts;
extern float cube[];

extern uint32_t frame[];

uint8_t LUT12bit[192]={
  0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

uint8_t LUT16bit[192]={
  0x00,0x08,0x10,0x18,0x20,0x28,0x30,0x38,0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78,
  0x80,0x88,0x90,0x98,0xA0,0xA8,0xB0,0xB8,0xC0,0xC8,0xD0,0xD8,0xE0,0xE8,0xF0,0xFF,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3C,
  0x40,0x44,0x48,0x4C,0x50,0x54,0x58,0x5C,0x60,0x64,0x68,0x6C,0x70,0x74,0x78,0x7C,
  0x80,0x84,0x88,0x8C,0x90,0x94,0x98,0x9C,0xA0,0xA4,0xA8,0xAC,0xB0,0xB4,0xB8,0xBC,
  0xC0,0xC4,0xC8,0xCC,0xD0,0xD4,0xD8,0xDC,0xE0,0xE4,0xE8,0xEC,0xF0,0xF4,0xF8,0xFF,

  0x00,0x08,0x10,0x18,0x20,0x28,0x30,0x38,0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78,
  0x80,0x88,0x90,0x98,0xA0,0xA8,0xB0,0xB8,0xC0,0xC8,0xD0,0xD8,0xE0,0xE8,0xF0,0xFF,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

uint8_t LUT18bit[192]={
  0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3C,
  0x40,0x44,0x48,0x4C,0x50,0x54,0x58,0x5C,0x60,0x64,0x68,0x6C,0x70,0x74,0x78,0x7C,
  0x80,0x84,0x88,0x8C,0x90,0x94,0x98,0x9C,0xA0,0xA4,0xA8,0xAC,0xB0,0xB4,0xB8,0xBC,
  0xC0,0xC4,0xC8,0xCC,0xD0,0xD4,0xD8,0xDC,0xE0,0xE4,0xE8,0xEC,0xF0,0xF4,0xF8,0xFF,

  0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3C,
  0x40,0x44,0x48,0x4C,0x50,0x54,0x58,0x5C,0x60,0x64,0x68,0x6C,0x70,0x74,0x78,0x7C,
  0x80,0x84,0x88,0x8C,0x90,0x94,0x98,0x9C,0xA0,0xA4,0xA8,0xAC,0xB0,0xB4,0xB8,0xBC,
  0xC0,0xC4,0xC8,0xCC,0xD0,0xD4,0xD8,0xDC,0xE0,0xE4,0xE8,0xEC,0xF0,0xF4,0xF8,0xFF,

  0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3C,
  0x40,0x44,0x48,0x4C,0x50,0x54,0x58,0x5C,0x60,0x64,0x68,0x6C,0x70,0x74,0x78,0x7C,
  0x80,0x84,0x88,0x8C,0x90,0x94,0x98,0x9C,0xA0,0xA4,0xA8,0xAC,0xB0,0xB4,0xB8,0xBC,
  0xC0,0xC4,0xC8,0xCC,0xD0,0xD4,0xD8,0xDC,0xE0,0xE4,0xE8,0xEC,0xF0,0xF4,0xF8,0xFF
};

void show_3d_cube(void);

void displayOrientation(char* text) {

  uint32_t i,
           max_x=MC2PA8201_GetWidth(),
           max_y=MC2PA8201_GetHeight();

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("TOP",0);
  MC2PA8201_PutStrCentered("BOTTOM",max_y-14);
  MC2PA8201_PutStr("LEFT",0,max_y/2);
  MC2PA8201_PutStr("RIGHT",max_x-8*5,max_y/2);
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
  DWT_Delay(3000000);
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

void GradientBars(void) {

  GradientFill(0,                        MC2PA8201_GetHeight()/7-1,  0x010000);
  GradientFill(MC2PA8201_GetHeight()/7,  MC2PA8201_GetHeight()/7*2-1,0x000100);
  GradientFill(MC2PA8201_GetHeight()/7*2,MC2PA8201_GetHeight()/7*3-1,0x000001);
  GradientFill(MC2PA8201_GetHeight()/7*3,MC2PA8201_GetHeight()/7*4-1,0x010001);
  GradientFill(MC2PA8201_GetHeight()/7*4,MC2PA8201_GetHeight()/7*5-1,0x000101);
  GradientFill(MC2PA8201_GetHeight()/7*5,MC2PA8201_GetHeight()/7*6-1,0x010100);
  GradientFill(MC2PA8201_GetHeight()/7*6,MC2PA8201_GetHeight()-1,    0x010101);
}

void ColorDepthTest(void) {

  uint8_t pixelFormat;

  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  GradientBars();
  switch(pixelFormat) {
    case COLOR_12BIT:MC2PA8201_PutStr("12",270,MC2PA8201_GetHeight()/2-16/2);break;
    case COLOR_16BIT:MC2PA8201_PutStr("16",270,MC2PA8201_GetHeight()/2-16/2);break;
    case COLOR_18BIT:MC2PA8201_PutStr("18",270,MC2PA8201_GetHeight()/2-16/2);break;
    case COLOR_24BIT:MC2PA8201_PutStr("24",270,MC2PA8201_GetHeight()/2-16/2);break;
    default:MC2PA8201_PutStr("--",270,MC2PA8201_GetHeight()/2-16/2);break;
  }
  MC2PA8201_PutStr("bit",270,MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);
}

void printColorMode(void) {

  switch(current_color_mode) {
    case COLOR_12BIT:MC2PA8201_PutStrCentered("12 bit",0);break;
    case COLOR_16BIT:MC2PA8201_PutStrCentered("16 bit",0);break;
    case COLOR_18BIT:MC2PA8201_PutStrCentered("18 bit",0);break;
    case COLOR_24BIT:MC2PA8201_PutStrCentered("24 bit",0);break;
    default:MC2PA8201_PutStrCentered("error",0);break;
  }
}

void JPEGTest(uint8_t* picture) {

  uint8_t pixelFormat;

  MC2PA8201_ColorMode(COLOR_24BIT);
  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);

  MC2PA8201_ColorMode(COLOR_18BIT);
  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);

  MC2PA8201_ColorMode(COLOR_16BIT);
  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);

  MC2PA8201_ColorMode(COLOR_12BIT);
  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);
}

void HardwareScrollingTest(void) {

  uint32_t i,j;

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Hardware",MC2PA8201_GetHeight()/2-32/2);
  MC2PA8201_PutStrCentered("scrolling",MC2PA8201_GetHeight()/2);
  MC2PA8201_PutStrCentered("test",MC2PA8201_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  MC2PA8201_ScrollArea(0,240);

  for(j=0;j<20;j++) {
    for(i=0;i<50;i++) {
      MC2PA8201_SetScrollPosition(i);
      DWT_Delay(10000);
    }

    for(i=0;i<50;i++) {
      MC2PA8201_SetScrollPosition(50-i);
      DWT_Delay(10000);
    }
  }
  MC2PA8201_SetScrollPosition(0);
}

void GammaCurvesTest(void) {

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
}

void sprintfReadBack(uint8_t pixelFormat,char* buf,uint8_t* buf1) {

  switch(pixelFormat) {
    case COLOR_12BIT:sprintf(buf,"%02X%01X",buf1[0],buf1[1]>>4);break;
    case COLOR_16BIT:sprintf(buf,"%02X%02X",buf1[0],buf1[1]);break;
    case COLOR_18BIT:
    case COLOR_24BIT:sprintf(buf,"%02X%02X%02X",buf1[0],buf1[1],buf1[2]);break;
    default:sprintf(buf,"error");break;
  }
}

void MemoryReadTest(void) {

  uint8_t buf1[16];
  char buf[32];
  uint8_t pixelFormat;

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Reading from memory",0);

  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  switch(pixelFormat) {
    case COLOR_12BIT:MC2PA8201_PutStrCentered("12bit color depth",16);break;
    case COLOR_16BIT:MC2PA8201_PutStrCentered("16bit color depth",16);break;
    case COLOR_18BIT:MC2PA8201_PutStrCentered("18bit color depth",16);break;
    case COLOR_24BIT:MC2PA8201_PutStrCentered("24bit color depth",16);break;
    default:MC2PA8201_PutStrCentered("error",16);break;
  }

  MC2PA8201_SetFont(FONT_6x8);

  MC2PA8201_PutStr("Reading pixel at 5,5 ... (should be BLACK)",0,40);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  MC2PA8201_PutStr("Read back: ",0,54);
  MC2PA8201_SetFont(FONT_8x14);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr(buf,70,50);
  MC2PA8201_SetFont(FONT_6x8);

  MC2PA8201_PutStr("Setting pixel at 5,5 to RED",0,68);
  MC2PA8201_SetPixel(5,5,RED);
  MC2PA8201_PutStr("Reading pixel at 5,5 ...",0,82);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr("Read back: ",0,96);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStr(buf,70,92);
  MC2PA8201_SetFont(FONT_6x8);

  MC2PA8201_PutStr("Setting pixel at 5,5 to GREEN",0,110);
  MC2PA8201_SetPixel(5,5,GREEN);
  MC2PA8201_PutStr("Reading pixel at 5,5 ...",0,124);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr("Read back: ",0,138);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStr(buf,70,134);
  MC2PA8201_SetFont(FONT_6x8);

  MC2PA8201_PutStr("Setting pixel at 5,5 to BLUE",0,152);
  MC2PA8201_SetPixel(5,5,BLUE);
  MC2PA8201_PutStr("Reading pixel at 5,5 ...",0,166);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr("Read back: ",0,180);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStr(buf,70,176);
  MC2PA8201_SetFont(FONT_6x8);

  MC2PA8201_PutStr("Setting pixel at 5,5 to WHITE",0,194);
  MC2PA8201_SetPixel(5,5,WHITE);
  MC2PA8201_PutStr("Reading pixel at 5,5 ...",0,208);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr("Read back: ",0,222);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStr(buf,70,218);
  MC2PA8201_SetFont(FONT_6x8);

  DWT_Delay(3000000);
}

int main (void) {

  uint32_t i,max_x,max_y,t_start,t_end;
  uint16_t x0,y0,x1,y1,tmp;
  char buf[16];
  uint8_t regs[5];

  DWT_Init();

  DWT_Delay(1000000);

  BSP_Buttons_Init(BUTTON_MODE_EXTI);

  MC2PA8201_Init(0,1);
  DWT_Delay(100000);

  MC2PA8201_SetLUT_params(COLOR_12BIT,sizeof(LUT12bit),LUT12bit);
  MC2PA8201_SetLUT_params(COLOR_16BIT,sizeof(LUT16bit),LUT16bit);
  MC2PA8201_SetLUT_params(COLOR_18BIT,sizeof(LUT18bit),LUT18bit);
  MC2PA8201_ColorMode(COLOR_24BIT);
  MC2PA8201_OrientationMode(ORIENTATION_LANDSCAPE);
  MC2PA8201_ClearScreen(BLACK);

  //MC2PA8201_TearingEffectLineOn(TE_VBLANK);
  //MC2PA8201_TearingEffectLineOff();

  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Nokia E61 LCD",0);
  MC2PA8201_SetFont(FONT_6x8);
  MC2PA8201_PutStr("Markings on the back:",0,25);
  MC2PA8201_PutStrCentered("30316P01 714FP 4851077 MP1.1 0441600005",45);
  MC2PA8201_PutStr("LCD type: TFT",0,65);
  MC2PA8201_PutStr("Screen size: 2.8 inches",0,85);
  MC2PA8201_PutStr("Resolution: 320x240 px",0,105);
  MC2PA8201_PutStr("Colors: 16M (24 bit/pixel)",0,125);
  MC2PA8201_PutStr("Controller: MagnaChip MC2PA8201 compatible",0,145);
  MC2PA8201_PutStr("Connector: 24pin with 0.4mm pitch",0,165);
  MC2PA8201_PutStr("Backlight: 6 LEDs",0,185);
  MC2PA8201_PutStr("MPU Interface: 8bit 8080 parallel",0,205);
  DWT_Delay(5000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Color mode used in this demo",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("is 16M (24 bit/pixel)",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Color test",0);
  MC2PA8201_PutStrCentered("Basic colors",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(RED);
  MC2PA8201_PutStrCentered("Red",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(GREEN);
  MC2PA8201_PutStrCentered("Green",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLUE);
  MC2PA8201_PutStrCentered("Blue",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(CYAN);
  MC2PA8201_PutStrCentered("Cyan",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(MAGENTA);
  MC2PA8201_PutStrCentered("Magenta",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(YELLOW);
  MC2PA8201_PutStrCentered("Yellow",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Color test",0);
  MC2PA8201_PutStrCentered("Gradient with basic colors",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLACK);
  GradientBars();
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Color depth test",0);
  MC2PA8201_PutStrCentered("Gradient color bars are displayed",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("at different LCD color depth",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLACK);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_18BIT);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_16BIT);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_12BIT);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_16BIT);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_18BIT);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_24BIT);
  ColorDepthTest();

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("JPEG pictures test",0);
  MC2PA8201_PutStrCentered("The same jpeg picture is displayed",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("at different LCD color depth",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  JPEGTest(data);
  JPEGTest(data1);
  JPEGTest(data2);
  JPEGTest(data3);
  JPEGTest(data4);
  JPEGTest(data5);
  JPEGTest(data6);
  JPEGTest(data7);
  JPEGTest(data8);
  JPEGTest(data9);
  JPEGTest(data10);
  JPEGTest(data11);
  JPEGTest(data12);
  JPEGTest(data13);
  JPEGTest(data14);
  JPEGTest(data15);
  JPEGTest(data16);
  JPEGTest(data17);
  JPEGTest(data18);
  JPEGTest(data28);
  MC2PA8201_ColorMode(COLOR_24BIT);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Gamma curves test",0);
  MC2PA8201_PutStrCentered("There are 4 predefined gamma curves",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("Default is gamma curve 1",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x010000);
  GammaCurvesTest();
  MC2PA8201_Gamma(GAMMA_CURVE1);

  MC2PA8201_ClearScreen(BLACK);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x0100);
  GammaCurvesTest();
  MC2PA8201_Gamma(GAMMA_CURVE1);

  MC2PA8201_ClearScreen(BLACK);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x01);
  GammaCurvesTest();
  MC2PA8201_Gamma(GAMMA_CURVE1);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Reading from registers",0);
  MC2PA8201_SetFont(FONT_6x8);
  MC2PA8201_ReadRegister(READ_DISPLAY_ID,3,regs);
  MC2PA8201_PutStr("(0x04) DISPLAY_ID:",0,35);
  sprintf(buf,"%02X %02X %02X",regs[0],regs[1],regs[2]);
  MC2PA8201_PutStr(buf,200,35);

  MC2PA8201_ReadRegister(READ_DISPLAY_STATUS,4,regs);
  MC2PA8201_PutStr("(0x09) DISPLAY_STATUS:",0,45);
  sprintf(buf,"%02X %02X %02X %02X",regs[0],regs[1],regs[2],regs[3]);
  MC2PA8201_PutStr(buf,200,45);

  MC2PA8201_ReadRegister(READ_DISPLAY_POWER_MODE,1,regs);
  MC2PA8201_PutStr("(0x0A) DISPLAY_POWER_MODE:",0,55);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,55);

  MC2PA8201_ReadRegister(READ_DISPLAY_MADCTL,1,regs);
  MC2PA8201_PutStr("(0x0B) DISPLAY_MADCTL:",0,65);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,65);

  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,regs);
  MC2PA8201_PutStr("(0x0C) DISPLAY_PIXEL_FORMAT:",0,75);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,75);

  MC2PA8201_ReadRegister(READ_DISPLAY_IMAGE_MODE,1,regs);
  MC2PA8201_PutStr("(0x0D) DISPLAY_IMAGE_MODE:",0,85);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,85);

  MC2PA8201_ReadRegister(READ_DISPLAY_SIGNAL_MODE,1,regs);
  MC2PA8201_PutStr("(0x0E) DISPLAY_SIGNAL_MODE:",0,95);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,95);

  MC2PA8201_ReadRegister(READ_DISPLAY_SELF_DIAGNOSTICS,1,regs);
  MC2PA8201_PutStr("(0x0F) DISPLAY_SELF_DIAGNOSTICS:",0,105);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,105);

  MC2PA8201_ReadRegister(READ_ID1,1,regs);
  MC2PA8201_PutStr("(0xDA) ID1:",0,115);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,115);

  MC2PA8201_ReadRegister(READ_ID2,1,regs);
  MC2PA8201_PutStr("(0xDB) ID2:",0,125);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,125);

  MC2PA8201_ReadRegister(READ_ID3,1,regs);
  MC2PA8201_PutStr("(0xDC) ID3:",0,135);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,200,135);
  DWT_Delay(3000000);

  MemoryReadTest();
  MC2PA8201_ColorMode(COLOR_18BIT);
  MemoryReadTest();
  MC2PA8201_ColorMode(COLOR_16BIT);
  MemoryReadTest();
  MC2PA8201_ColorMode(COLOR_12BIT);
  MemoryReadTest();
  MC2PA8201_ColorMode(COLOR_24BIT);

  HardwareScrollingTest();

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Partial mode test",0);
  MC2PA8201_PutStrCentered("Setting partial area to 40,200",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("and filling all screen with one color",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  MC2PA8201_PartialArea(40,200);
  MC2PA8201_PartialMode();
  MC2PA8201_ClearScreen(RED);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(GREEN);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLUE);
  DWT_Delay(3000000);

  MC2PA8201_NormalDisplayMode();
  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Partial mode test",0);
  MC2PA8201_PutStrCentered("Setting partial area to 200,40",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("and filling all screen with one color",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  MC2PA8201_PartialArea(200,40);
  MC2PA8201_PartialMode();
  MC2PA8201_ClearScreen(RED);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(GREEN);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLUE);
  DWT_Delay(3000000);
  MC2PA8201_NormalDisplayMode();

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
  MC2PA8201_PutStrCentered("Display inversion test",0);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLACK);
  GradientBars();
  MC2PA8201_PutStrCentered(" Normal ",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_DisplayInversionOn();
  MC2PA8201_PutStrCentered("Inverted",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  MC2PA8201_DisplayInversionOff();
  MC2PA8201_PutStrCentered(" Normal ",MC2PA8201_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Idle mode test",0);
  MC2PA8201_PutStrCentered("In idle mode color space",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("is reduced to 8 colors",MC2PA8201_GetHeight()/2+16/2);
  DWT_Delay(3000000);

  GradientBars();
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

  MC2PA8201_PutStrCentered("Nokia E61 LCD - Rotating 3D cube",0);
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
