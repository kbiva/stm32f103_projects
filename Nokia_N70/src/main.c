/*
 *  main.c
 *
 *  Nokia N70 LCD demo
 *
 *  Author: Kestutis Bivainis
 *
 */

#include "stm32f10x_conf.h"
#include "MC2PA8201.h"
#include "colors.h"
#include "delay.h"
#include "hy_stm32.h"
#include "jpeg.h"
#include <math.h>
#include <stdio.h>
#include "xorshift.h"

extern uint8_t data1[];
extern uint8_t data2[];
extern uint8_t data3[];
extern uint8_t data4[];
extern uint8_t data5[];

extern uint8_t* dataptr;
COLOR_MODE current_color_mode;

uint8_t LUT12bit[128]={
  0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3F,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3F,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3F,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3F,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

uint8_t LUT16bit[128]={
  0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,0x12,0x14,0x16,0x18,0x1A,0x1C,0x1E,
  0x20,0x22,0x24,0x26,0x28,0x2A,0x2C,0x2E,0x30,0x32,0x34,0x36,0x38,0x3A,0x3C,0x3F,

  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
  0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
  0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,

  0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,0x12,0x14,0x16,0x18,0x1A,0x1C,0x1E,
  0x20,0x22,0x24,0x26,0x28,0x2A,0x2C,0x2E,0x30,0x32,0x34,0x36,0x38,0x3A,0x3C,0x3F,
};

void show_fire(void);

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

  for(j=0;j<64;j++) {

    MC2PA8201_Fill(i,y1,i+2,y2,c);
    i+=3;
    c+=color_inc;
  }
}

void GradientBars(void) {

  GradientFill(0,                       MC2PA8201_GetHeight()/7-1,  0x040000);
  GradientFill(MC2PA8201_GetHeight()/7,  MC2PA8201_GetHeight()/7*2-1,0x000400);
  GradientFill(MC2PA8201_GetHeight()/7*2,MC2PA8201_GetHeight()/7*3-1,0x000004);
  GradientFill(MC2PA8201_GetHeight()/7*3,MC2PA8201_GetHeight()/7*4-1,0x040004);
  GradientFill(MC2PA8201_GetHeight()/7*4,MC2PA8201_GetHeight()/7*5-1,0x000404);
  GradientFill(MC2PA8201_GetHeight()/7*5,MC2PA8201_GetHeight()/7*6-1,0x040400);
  GradientFill(MC2PA8201_GetHeight()/7*6,MC2PA8201_GetHeight()-1,    0x040404);
}

void ColorDepthTest(void) {

  uint8_t pixelFormat;

  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  GradientBars();
  switch(pixelFormat) {
    case COLOR_12BIT:MC2PA8201_PutStr("12",192,MC2PA8201_GetHeight()/2-48/2);break;
    case COLOR_16BIT:MC2PA8201_PutStr("16",192,MC2PA8201_GetHeight()/2-48/2);break;
    case COLOR_18BIT:MC2PA8201_PutStr("18",192,MC2PA8201_GetHeight()/2-48/2);break;
    default:MC2PA8201_PutStr("--",180,MC2PA8201_GetHeight()/2-48/2);break;
  }
  MC2PA8201_PutStr("b",196,MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStr("i",196,MC2PA8201_GetHeight()/2+16/2);
  MC2PA8201_PutStr("t",196,MC2PA8201_GetHeight()/2+48/2);
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
  MC2PA8201_ScrollArea(0,208);

  for(j=0;j<20;j++) {
    for(i=0;i<30;i++) {
      MC2PA8201_SetScrollPosition(i);
      DWT_Delay(10000);
    }

    for(i=0;i<30;i++) {
      MC2PA8201_SetScrollPosition(30-i);
      DWT_Delay(10000);
    }
  }
  MC2PA8201_SetScrollPosition(0);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_OrientationMode(ORIENTATION_PORTRAIT);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Hardware",MC2PA8201_GetHeight()/2-32/2);
  MC2PA8201_PutStrCentered("scrolling",MC2PA8201_GetHeight()/2);
  MC2PA8201_PutStrCentered("test",MC2PA8201_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  MC2PA8201_ScrollArea(0,208);

  for(j=0;j<20;j++) {
    for(i=0;i<30;i++) {
      MC2PA8201_SetScrollPosition(i);
      DWT_Delay(10000);
    }

    for(i=0;i<30;i++) {
      MC2PA8201_SetScrollPosition(30-i);
      DWT_Delay(10000);
    }
  }
  MC2PA8201_SetScrollPosition(0);
  MC2PA8201_OrientationMode(ORIENTATION_LANDSCAPE);
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
    case COLOR_18BIT:sprintf(buf,"%02X%02X%02X",buf1[0],buf1[1],buf1[2]);break;
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
    case COLOR_12BIT:MC2PA8201_PutStrCentered("12bit color depth",14);break;
    case COLOR_16BIT:MC2PA8201_PutStrCentered("16bit color depth",14);break;
    case COLOR_18BIT:MC2PA8201_PutStrCentered("18bit color depth",14);break;
    default:MC2PA8201_PutStrCentered("error",14);break;
  }

  MC2PA8201_SetFont(FONT_6x8);

  MC2PA8201_PutStr("Reading pixel at 5,5 ... (BLACK)",0,30);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  MC2PA8201_PutStr("Read back: ",0,40);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr(buf,70,40);

  MC2PA8201_PutStr("Setting pixel at 5,5 to RED",0,50);
  MC2PA8201_SetPixel(5,5,RED);
  MC2PA8201_PutStr("Reading pixel at 5,5 ...",0,60);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr("Read back: ",0,70);
  MC2PA8201_PutStr(buf,70,70);

  MC2PA8201_PutStr("Setting pixel at 5,5 to GREEN",0,80);
  MC2PA8201_SetPixel(5,5,GREEN);
  MC2PA8201_PutStr("Reading pixel at 5,5 ...",0,90);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr("Read back: ",0,100);
  MC2PA8201_PutStr(buf,70,100);

  MC2PA8201_PutStr("Setting pixel at 5,5 to BLUE",0,110);
  MC2PA8201_SetPixel(5,5,BLUE);
  MC2PA8201_PutStr("Reading pixel at 5,5 ...",0,120);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr("Read back: ",0,130);
  MC2PA8201_PutStr(buf,70,130);

  MC2PA8201_PutStr("Setting pixel at 5,5 to WHITE",0,140);
  MC2PA8201_SetPixel(5,5,WHITE);
  MC2PA8201_PutStr("Reading pixel at 5,5 ...",0,150);
  MC2PA8201_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  MC2PA8201_PutStr("Read back: ",0,160);
  MC2PA8201_PutStr(buf,70,160);

  DWT_Delay(3000000);
}

void printColorMode(void) {

  switch(current_color_mode) {
    case COLOR_12BIT:MC2PA8201_PutStrCentered("12 bit",0);break;
    case COLOR_16BIT:MC2PA8201_PutStrCentered("16 bit",0);break;
    case COLOR_18BIT:MC2PA8201_PutStrCentered("18 bit",0);break;
    default:MC2PA8201_PutStrCentered("error",0);break;
  }
}

void JPEGTest(uint8_t* picture) {

  uint8_t pixelFormat;

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

int main (void) {

  uint32_t i,max_x,max_y,t_start,t_end;
  uint16_t x0,y0,x1,y1,tmp;
  char buf[16];
  uint8_t regs[5];

  DWT_Init();

  DWT_Delay(1000000);

  BSP_Buttons_Init(BUTTON_MODE_EXTI);

  MC2PA8201_Init(0,1);
  MC2PA8201_SetDimensions(208,176);
  MC2PA8201_SetMADCTL_params(0x60,0x00,0xA0,0xC0);

  MC2PA8201_SetLUT_params(COLOR_12BIT,sizeof(LUT12bit),LUT12bit);
  MC2PA8201_SetLUT_params(COLOR_16BIT,sizeof(LUT16bit),LUT16bit);

  MC2PA8201_ColorMode(COLOR_18BIT);
  MC2PA8201_OrientationMode(ORIENTATION_LANDSCAPE);
  MC2PA8201_ClearScreen(BLACK);

  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Nokia N70 LCD",0);
  MC2PA8201_SetFont(FONT_6x8);
  MC2PA8201_PutStr("Markings on the back:",0,25);
  MC2PA8201_PutStr("50366P00 626GL 0319690004 4850875",0,35);
  MC2PA8201_PutStr("LCD type: TFT",0,45);
  MC2PA8201_PutStr("Screen size: 2.1 inches",0,55);
  MC2PA8201_PutStr("Resolution: 208x176 px",0,65);
  MC2PA8201_PutStr("Colors: 262K (18 bit/pixel)",0,75);
  MC2PA8201_PutStr("Controller:",0,85);
  MC2PA8201_PutStr("MagnaChip MC2PA8201 compatible",0,95);
  MC2PA8201_PutStr("Connector: 24pin with 0.4mm pitch",0,105);
  MC2PA8201_PutStr("Backlight: 4 LEDs",0,115);
  MC2PA8201_PutStr("MPU Interface: 8bit 8080 parallel",0,125);
  DWT_Delay(5000000);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Color mode used",MC2PA8201_GetHeight()/2-32/2);
  MC2PA8201_PutStrCentered("in this demo",MC2PA8201_GetHeight()/2);
  MC2PA8201_PutStrCentered("is 262K (18 bit/pixel)",MC2PA8201_GetHeight()/2+32/2);
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
  MC2PA8201_PutStrCentered("Gradient color bars are",MC2PA8201_GetHeight()/2-32/2);
  MC2PA8201_PutStrCentered("displayed at different ",MC2PA8201_GetHeight()/2);
  MC2PA8201_PutStrCentered("LCD color depth",MC2PA8201_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  MC2PA8201_ClearScreen(BLACK);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_16BIT);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_12BIT);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_16BIT);
  ColorDepthTest();
  MC2PA8201_ColorMode(COLOR_18BIT);
  ColorDepthTest();

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("JPEG pictures test",0);
  MC2PA8201_PutStrCentered("The same jpeg picture",MC2PA8201_GetHeight()/2-32/2);
  MC2PA8201_PutStrCentered("is displayed at different",MC2PA8201_GetHeight()/2);
  MC2PA8201_PutStrCentered("LCD color depth",MC2PA8201_GetHeight()/2+32/2);

  DWT_Delay(3000000);
  JPEGTest(data1);
  JPEGTest(data2);
  JPEGTest(data3);
  JPEGTest(data4);
  JPEGTest(data5);
  MC2PA8201_ColorMode(COLOR_18BIT);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Gamma curves test",0);
  MC2PA8201_PutStrCentered("There are 4 predefined",MC2PA8201_GetHeight()/2-32/2);
  MC2PA8201_PutStrCentered("gamma curves",MC2PA8201_GetHeight()/2);
  MC2PA8201_PutStrCentered("Default is gamma curve 1",MC2PA8201_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  MC2PA8201_ClearScreen(BLACK);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x040000);
  GammaCurvesTest();
  MC2PA8201_Gamma(GAMMA_CURVE1);

  MC2PA8201_ClearScreen(BLACK);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x0400);
  GammaCurvesTest();
  MC2PA8201_Gamma(GAMMA_CURVE1);

  MC2PA8201_ClearScreen(BLACK);
  GradientFill(0,MC2PA8201_GetHeight()-1,0x04);
  GammaCurvesTest();
  MC2PA8201_Gamma(GAMMA_CURVE1);

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_PutStrCentered("Reading from registers",0);
  MC2PA8201_SetFont(FONT_6x8);
  MC2PA8201_ReadRegister(READ_DISPLAY_ID,3,regs);
  MC2PA8201_PutStr("0x04 ID:",0,35);
  sprintf(buf,"%02X %02X %02X",regs[0],regs[1],regs[2]);
  MC2PA8201_PutStr(buf,140,35);

  MC2PA8201_ReadRegister(READ_DISPLAY_STATUS,4,regs);
  MC2PA8201_PutStr("0x09 STATUS:",0,45);
  sprintf(buf,"%02X %02X %02X %02X",regs[0],regs[1],regs[2],regs[3]);
  MC2PA8201_PutStr(buf,140,45);

  MC2PA8201_ReadRegister(READ_DISPLAY_POWER_MODE,1,regs);
  MC2PA8201_PutStr("0x0A POWER_MODE:",0,55);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,55);

  MC2PA8201_ReadRegister(READ_DISPLAY_MADCTL,1,regs);
  MC2PA8201_PutStr("0x0B MADCTL:",0,65);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,65);

  MC2PA8201_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,regs);
  MC2PA8201_PutStr("0x0C PIXEL_FORMAT:",0,75);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,75);

  MC2PA8201_ReadRegister(READ_DISPLAY_IMAGE_MODE,1,regs);
  MC2PA8201_PutStr("0x0D IMAGE_MODE:",0,85);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,85);

  MC2PA8201_ReadRegister(READ_DISPLAY_SIGNAL_MODE,1,regs);
  MC2PA8201_PutStr("0x0E SIGNAL_MODE:",0,95);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,95);

  MC2PA8201_ReadRegister(READ_DISPLAY_SELF_DIAGNOSTICS,1,regs);
  MC2PA8201_PutStr("0x0F SELF_DIAGNOSTICS:",0,105);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,105);

  MC2PA8201_ReadRegister(READ_ID1,1,regs);
  MC2PA8201_PutStr("0xDA ID1:",0,115);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,115);

  MC2PA8201_ReadRegister(READ_ID2,1,regs);
  MC2PA8201_PutStr("0xDB ID2:",0,125);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,125);

  MC2PA8201_ReadRegister(READ_ID3,1,regs);
  MC2PA8201_PutStr("0xDC ID3:",0,135);
  sprintf(buf,"%02X",regs[0]);
  MC2PA8201_PutStr(buf,140,135);
  DWT_Delay(3000000);

  MemoryReadTest();
  MC2PA8201_ColorMode(COLOR_16BIT);
  MemoryReadTest();
  MC2PA8201_ColorMode(COLOR_12BIT);
  MemoryReadTest();
  MC2PA8201_ColorMode(COLOR_18BIT);

  HardwareScrollingTest();

  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Partial mode test",0);
  MC2PA8201_PutStrCentered("Setting partial area",MC2PA8201_GetHeight()/2-48/2);
  MC2PA8201_PutStrCentered("to 40,168",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("and filling all screen",MC2PA8201_GetHeight()/2+16/2);
  MC2PA8201_PutStrCentered("with one color",MC2PA8201_GetHeight()/2+48/2);
  DWT_Delay(3000000);
  MC2PA8201_PartialArea(40,168);
  MC2PA8201_PartialMode();
  MC2PA8201_ClearScreen(RED);
  DWT_Delay(1000000);
  MC2PA8201_ClearScreen(GREEN);
  DWT_Delay(1000000);
  MC2PA8201_ClearScreen(BLUE);
  DWT_Delay(1000000);

  MC2PA8201_NormalDisplayMode();
  MC2PA8201_ClearScreen(BLACK);
  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Partial mode test",0);
  MC2PA8201_PutStrCentered("Setting partial area",MC2PA8201_GetHeight()/2-48/2);
  MC2PA8201_PutStrCentered("to 168,40",MC2PA8201_GetHeight()/2-16/2);
  MC2PA8201_PutStrCentered("and filling all screen",MC2PA8201_GetHeight()/2+16/2);
  MC2PA8201_PutStrCentered("with one color",MC2PA8201_GetHeight()/2+48/2);
  DWT_Delay(3000000);
  MC2PA8201_PartialArea(168,40);
  MC2PA8201_PartialMode();
  MC2PA8201_ClearScreen(RED);
  DWT_Delay(1000000);
  MC2PA8201_ClearScreen(GREEN);
  DWT_Delay(1000000);
  MC2PA8201_ClearScreen(BLUE);
  DWT_Delay(1000000);
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
  MC2PA8201_ClearScreen(BLACK);

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
  MC2PA8201_ColorMode(COLOR_16BIT);
  show_fire();
  while(1){};
}

uint32_t pal[256];
uint8_t buf[144*208];

void show_fire(void) {

  int32_t i,j;
  uint8_t *bottom;
  int32_t v;

  uint8_t width=208,
          height=144,
          dest1;

  for(i=0;i<64;i++) {
    pal[i]=i<<18;
    pal[i+64]=(i<<10)|0x00FF0000;
    pal[i+128]=(i<<2)|0x00FFFF00;
    pal[i+192]=0x00FFFFFF;
  }

  MC2PA8201_SetFont(FONT_8x14);
  MC2PA8201_PutStrCentered("Nokia N70 LCD - 3D fire",0);
  MC2PA8201_PutStrCentered("kbiva.wordpress.com",160);

  while(1) {

    bottom=buf+(height-1)*width;

    for(i=0;i<width;i++) {
      *(bottom+i)=xor128()%256;
    }

    for(i=height-2;i>=0;i--) {
      for(j=1;j<width-1;j++) {
        v=(buf[i*width+j]+buf[(i+1)*width+j]+buf[(i+1)*width+j-1]+buf[(i+1)*width+j+1])/4-2;
        //if(v<0) v=0;
        buf[i*width+j]=v;
        //MC2PA8201_SetPixel(j,i+16,pal[v]);
      }
    }
    MC2PA8201_SetWriteWindow(0,16,width-1,16+height-1);
    for(i=0;i<height*width;i++) {
      dest1=((pal[buf[i]] & 0xf80000) >> 16) | ((pal[buf[i]] & 0xe000) >> 13);
      LCD_DAT8 = dest1;
      dest1=((pal[buf[i]] & 0x1c00) >> 5) | ((pal[buf[i]] & 0xf8) >> 3);
      LCD_DAT8 = dest1;
    }
  }
}
