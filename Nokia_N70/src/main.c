/*
 *  main.c
 *
 *  Nokia N70 LCD demo
 *
 *  Author: Kestutis Bivainis
 *
 */

#include "stm32f10x_conf.h"
#include "NokiaN70.h"
#include "colors.h"
#include "delay.h"
#include "hy_stm32.h"
#include "jpeg.h"
#include <math.h>
#include <stdio.h>

extern uint8_t data1[];
extern uint8_t data2[];
extern uint8_t data3[];
extern uint8_t data4[];
extern uint8_t data5[];

extern uint8_t* dataptr;
COLOR_MODE current_color_mode;

void show_fire(void);

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
           max_x=NokiaN70_GetWidth(),
           max_y=NokiaN70_GetHeight();

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("TOP",0);
  NokiaN70_PutStrCentered("BOTTOM",max_y-14);
  NokiaN70_PutStr("LEFT",0,max_y/2);
  NokiaN70_PutStr("RIGHT",max_x-8*5,max_y/2);
  NokiaN70_PutStrCentered("Orientation:",max_y/2-16/2);
  NokiaN70_PutStrCentered(text,max_y/2+16/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("10K colored text lines",max_y/2);
  DWT_Delay(3000000);

  NokiaN70_SetFont(FONT_6x8);
  for(i=0;i<10000;i++) {
    NokiaN70_SetTextColors(xor128()%0xFFFFFF,xor128()%0xFFFFFF);
    NokiaN70_PutStr("This is some text",xor128()%max_x,xor128()%max_y);
  }
  NokiaN70_SetTextColors(WHITE,BLACK);
  NokiaN70_SetFont(FONT_8x14);
  DWT_Delay(3000000);
}

void GradientFill(uint16_t y1,uint16_t y2,uint32_t color_inc) {

  uint32_t i,j,c;

  c=0;
  i=0;

  for(j=0;j<64;j++) {

    NokiaN70_Fill(i,y1,i+2,y2,c);
    i+=3;
    c+=color_inc;
  }
}

void GradientBars(void) {

  GradientFill(0,                       NokiaN70_GetHeight()/7-1,  0x040000);
  GradientFill(NokiaN70_GetHeight()/7,  NokiaN70_GetHeight()/7*2-1,0x000400);
  GradientFill(NokiaN70_GetHeight()/7*2,NokiaN70_GetHeight()/7*3-1,0x000004);
  GradientFill(NokiaN70_GetHeight()/7*3,NokiaN70_GetHeight()/7*4-1,0x040004);
  GradientFill(NokiaN70_GetHeight()/7*4,NokiaN70_GetHeight()/7*5-1,0x000404);
  GradientFill(NokiaN70_GetHeight()/7*5,NokiaN70_GetHeight()/7*6-1,0x040400);
  GradientFill(NokiaN70_GetHeight()/7*6,NokiaN70_GetHeight()-1,    0x040404);
}

void ColorDepthTest(void) {

  uint8_t pixelFormat;

  NokiaN70_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  GradientBars();
  switch(pixelFormat) {
    case COLOR_12BIT:NokiaN70_PutStr("12",192,NokiaN70_GetHeight()/2-48/2);break;
    case COLOR_16BIT:NokiaN70_PutStr("16",192,NokiaN70_GetHeight()/2-48/2);break;
    case COLOR_18BIT:NokiaN70_PutStr("18",192,NokiaN70_GetHeight()/2-48/2);break;
    default:NokiaN70_PutStr("--",180,NokiaN70_GetHeight()/2-48/2);break;
  }
  NokiaN70_PutStr("b",196,NokiaN70_GetHeight()/2-16/2);
  NokiaN70_PutStr("i",196,NokiaN70_GetHeight()/2+16/2);
  NokiaN70_PutStr("t",196,NokiaN70_GetHeight()/2+48/2);
  DWT_Delay(3000000);
}

void HardwareScrollingTest(void) {

  uint32_t i,j;

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Hardware",NokiaN70_GetHeight()/2-32/2);
  NokiaN70_PutStrCentered("scrolling",NokiaN70_GetHeight()/2);
  NokiaN70_PutStrCentered("test",NokiaN70_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  NokiaN70_ScrollArea(0,208);

  for(j=0;j<20;j++) {
    for(i=0;i<30;i++) {
      NokiaN70_SetScrollPosition(i);
      DWT_Delay(10000);
    }

    for(i=0;i<30;i++) {
      NokiaN70_SetScrollPosition(30-i);
      DWT_Delay(10000);
    }
  }
  NokiaN70_SetScrollPosition(0);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_OrientationMode(ORIENTATION_PORTRAIT);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Hardware",NokiaN70_GetHeight()/2-32/2);
  NokiaN70_PutStrCentered("scrolling",NokiaN70_GetHeight()/2);
  NokiaN70_PutStrCentered("test",NokiaN70_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  NokiaN70_ScrollArea(0,208);

  for(j=0;j<20;j++) {
    for(i=0;i<30;i++) {
      NokiaN70_SetScrollPosition(i);
      DWT_Delay(10000);
    }

    for(i=0;i<30;i++) {
      NokiaN70_SetScrollPosition(30-i);
      DWT_Delay(10000);
    }
  }
  NokiaN70_SetScrollPosition(0);
  NokiaN70_OrientationMode(ORIENTATION_LANDSCAPE);
}

void GammaCurvesTest(void) {

  NokiaN70_PutStrCentered("Gamma curve 1 (2.2)",NokiaN70_GetHeight()/2-8/2);
  NokiaN70_Gamma(GAMMA_CURVE1);
  DWT_Delay(3000000);
  NokiaN70_PutStrCentered("Gamma curve 2 (1.8)",NokiaN70_GetHeight()/2-8/2);
  NokiaN70_Gamma(GAMMA_CURVE2);
  DWT_Delay(3000000);
  NokiaN70_PutStrCentered("Gamma curve 3 (2.5)",NokiaN70_GetHeight()/2-8/2);
  NokiaN70_Gamma(GAMMA_CURVE3);
  DWT_Delay(3000000);
  NokiaN70_PutStrCentered("Gamma curve 4 (1.0)",NokiaN70_GetHeight()/2-8/2);
  NokiaN70_Gamma(GAMMA_CURVE4);
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

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Reading from memory",0);

  NokiaN70_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  switch(pixelFormat) {
    case COLOR_12BIT:NokiaN70_PutStrCentered("12bit color depth",14);break;
    case COLOR_16BIT:NokiaN70_PutStrCentered("16bit color depth",14);break;
    case COLOR_18BIT:NokiaN70_PutStrCentered("18bit color depth",14);break;
    default:NokiaN70_PutStrCentered("error",14);break;
  }

  NokiaN70_SetFont(FONT_6x8);

  NokiaN70_PutStr("Reading pixel at 5,5 ... (BLACK)",0,30);
  NokiaN70_ReadMemory(5,5,5,5,buf1);
  NokiaN70_PutStr("Read back: ",0,40);
  sprintfReadBack(pixelFormat,buf,buf1);
  NokiaN70_PutStr(buf,70,40);

  NokiaN70_PutStr("Setting pixel at 5,5 to RED",0,50);
  NokiaN70_SetPixel(5,5,RED);
  NokiaN70_PutStr("Reading pixel at 5,5 ...",0,60);
  NokiaN70_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  NokiaN70_PutStr("Read back: ",0,70);
  NokiaN70_PutStr(buf,70,70);

  NokiaN70_PutStr("Setting pixel at 5,5 to GREEN",0,80);
  NokiaN70_SetPixel(5,5,GREEN);
  NokiaN70_PutStr("Reading pixel at 5,5 ...",0,90);
  NokiaN70_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  NokiaN70_PutStr("Read back: ",0,100);
  NokiaN70_PutStr(buf,70,100);

  NokiaN70_PutStr("Setting pixel at 5,5 to BLUE",0,110);
  NokiaN70_SetPixel(5,5,BLUE);
  NokiaN70_PutStr("Reading pixel at 5,5 ...",0,120);
  NokiaN70_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  NokiaN70_PutStr("Read back: ",0,130);
  NokiaN70_PutStr(buf,70,130);

  NokiaN70_PutStr("Setting pixel at 5,5 to WHITE",0,140);
  NokiaN70_SetPixel(5,5,WHITE);
  NokiaN70_PutStr("Reading pixel at 5,5 ...",0,150);
  NokiaN70_ReadMemory(5,5,5,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  NokiaN70_PutStr("Read back: ",0,160);
  NokiaN70_PutStr(buf,70,160);

  DWT_Delay(3000000);
}

void printColorMode(void) {

  switch(current_color_mode) {
    case COLOR_12BIT:NokiaN70_PutStrCentered("12 bit",0);break;
    case COLOR_16BIT:NokiaN70_PutStrCentered("16 bit",0);break;
    case COLOR_18BIT:NokiaN70_PutStrCentered("18 bit",0);break;
    default:NokiaN70_PutStrCentered("error",0);break;
  }
}

void JPEGTest(uint8_t* picture) {

  uint8_t pixelFormat;

  NokiaN70_ColorMode(COLOR_18BIT);
  NokiaN70_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);

  NokiaN70_ColorMode(COLOR_16BIT);
  NokiaN70_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);

  NokiaN70_ColorMode(COLOR_12BIT);
  NokiaN70_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
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

  // reset pseudo-random number generator
  x = 123456789;
  y = 362436069;
  z = 521288629;
  w = 88675123;

  DWT_Init();

  DWT_Delay(1000000);

  BSP_Buttons_Init(BUTTON_MODE_EXTI);

  NokiaN70_Init(0,1);
  NokiaN70_ColorMode(COLOR_18BIT);
  NokiaN70_OrientationMode(ORIENTATION_LANDSCAPE);
  NokiaN70_ClearScreen(BLACK);

  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Nokia N70 LCD",0);
  NokiaN70_SetFont(FONT_6x8);
  NokiaN70_PutStr("Markings on the back:",0,25);
  NokiaN70_PutStr("50366P00 626GL 0319690004 4850875",0,35);
  NokiaN70_PutStr("LCD type: TFT",0,45);
  NokiaN70_PutStr("Screen size: 2.1 inches",0,55);
  NokiaN70_PutStr("Resolution: 208x176 px",0,65);
  NokiaN70_PutStr("Colors: 262K (18 bit/pixel)",0,75);
  NokiaN70_PutStr("Controller:",0,85);
  NokiaN70_PutStr("MagnaChip MC2PA8201 compatible",0,95);
  NokiaN70_PutStr("Connector: 24pin with 0.4mm pitch",0,105);
  NokiaN70_PutStr("Backlight: 4 LEDs",0,115);
  NokiaN70_PutStr("MPU Interface: 8bit 8080 parallel",0,125);
  DWT_Delay(5000000);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Color mode used",NokiaN70_GetHeight()/2-32/2);
  NokiaN70_PutStrCentered("in this demo",NokiaN70_GetHeight()/2);
  NokiaN70_PutStrCentered("is 262K (18 bit/pixel)",NokiaN70_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Color test",0);
  NokiaN70_PutStrCentered("Basic colors",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  NokiaN70_ClearScreen(RED);
  NokiaN70_PutStrCentered("Red",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(GREEN);
  NokiaN70_PutStrCentered("Green",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(BLUE);
  NokiaN70_PutStrCentered("Blue",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(CYAN);
  NokiaN70_PutStrCentered("Cyan",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(MAGENTA);
  NokiaN70_PutStrCentered("Magenta",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(YELLOW);
  NokiaN70_PutStrCentered("Yellow",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);


  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Color test",0);
  NokiaN70_PutStrCentered("Gradient with basic colors",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(BLACK);
  GradientBars();
  DWT_Delay(3000000);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Color depth test",0);
  NokiaN70_PutStrCentered("Gradient color bars are",NokiaN70_GetHeight()/2-32/2);
  NokiaN70_PutStrCentered("displayed at different ",NokiaN70_GetHeight()/2);
  NokiaN70_PutStrCentered("LCD color depth",NokiaN70_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(BLACK);
  ColorDepthTest();
  NokiaN70_ColorMode(COLOR_16BIT);
  ColorDepthTest();
  NokiaN70_ColorMode(COLOR_12BIT);
  ColorDepthTest();
  NokiaN70_ColorMode(COLOR_16BIT);
  ColorDepthTest();
  NokiaN70_ColorMode(COLOR_18BIT);
  ColorDepthTest();

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("JPEG pictures test",0);
  NokiaN70_PutStrCentered("The same jpeg picture",NokiaN70_GetHeight()/2-32/2);
  NokiaN70_PutStrCentered("is displayed at different",NokiaN70_GetHeight()/2);
  NokiaN70_PutStrCentered("LCD color depth",NokiaN70_GetHeight()/2+32/2);

  DWT_Delay(3000000);
  JPEGTest(data1);
  JPEGTest(data2);
  JPEGTest(data3);
  JPEGTest(data4);
  JPEGTest(data5);
  NokiaN70_ColorMode(COLOR_18BIT);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Gamma curves test",0);
  NokiaN70_PutStrCentered("There are 4 predefined",NokiaN70_GetHeight()/2-32/2);
  NokiaN70_PutStrCentered("gamma curves",NokiaN70_GetHeight()/2);
  NokiaN70_PutStrCentered("Default is gamma curve 1",NokiaN70_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  NokiaN70_ClearScreen(BLACK);
  GradientFill(0,NokiaN70_GetHeight()-1,0x040000);
  GammaCurvesTest();
  NokiaN70_Gamma(GAMMA_CURVE1);

  NokiaN70_ClearScreen(BLACK);
  GradientFill(0,NokiaN70_GetHeight()-1,0x0400);
  GammaCurvesTest();
  NokiaN70_Gamma(GAMMA_CURVE1);

  NokiaN70_ClearScreen(BLACK);
  GradientFill(0,NokiaN70_GetHeight()-1,0x04);
  GammaCurvesTest();
  NokiaN70_Gamma(GAMMA_CURVE1);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Reading from registers",0);
  NokiaN70_SetFont(FONT_6x8);
  NokiaN70_ReadRegister(READ_DISPLAY_ID,3,regs);
  NokiaN70_PutStr("0x04 ID:",0,35);
  sprintf(buf,"%02X %02X %02X",regs[0],regs[1],regs[2]);
  NokiaN70_PutStr(buf,140,35);

  NokiaN70_ReadRegister(READ_DISPLAY_STATUS,4,regs);
  NokiaN70_PutStr("0x09 STATUS:",0,45);
  sprintf(buf,"%02X %02X %02X %02X",regs[0],regs[1],regs[2],regs[3]);
  NokiaN70_PutStr(buf,140,45);

  NokiaN70_ReadRegister(READ_DISPLAY_POWER_MODE,1,regs);
  NokiaN70_PutStr("0x0A POWER_MODE:",0,55);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,55);

  NokiaN70_ReadRegister(READ_DISPLAY_MADCTL,1,regs);
  NokiaN70_PutStr("0x0B MADCTL:",0,65);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,65);

  NokiaN70_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,regs);
  NokiaN70_PutStr("0x0C PIXEL_FORMAT:",0,75);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,75);

  NokiaN70_ReadRegister(READ_DISPLAY_IMAGE_MODE,1,regs);
  NokiaN70_PutStr("0x0D IMAGE_MODE:",0,85);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,85);

  NokiaN70_ReadRegister(READ_DISPLAY_SIGNAL_MODE,1,regs);
  NokiaN70_PutStr("0x0E SIGNAL_MODE:",0,95);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,95);

  NokiaN70_ReadRegister(READ_DISPLAY_SELF_DIAGNOSTICS,1,regs);
  NokiaN70_PutStr("0x0F SELF_DIAGNOSTICS:",0,105);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,105);

  NokiaN70_ReadRegister(READ_ID1,1,regs);
  NokiaN70_PutStr("0xDA ID1:",0,115);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,115);

  NokiaN70_ReadRegister(READ_ID2,1,regs);
  NokiaN70_PutStr("0xDB ID2:",0,125);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,125);

  NokiaN70_ReadRegister(READ_ID3,1,regs);
  NokiaN70_PutStr("0xDC ID3:",0,135);
  sprintf(buf,"%02X",regs[0]);
  NokiaN70_PutStr(buf,140,135);
  DWT_Delay(3000000);

  MemoryReadTest();
  NokiaN70_ColorMode(COLOR_16BIT);
  MemoryReadTest();
  NokiaN70_ColorMode(COLOR_12BIT);
  MemoryReadTest();
  NokiaN70_ColorMode(COLOR_18BIT);

  HardwareScrollingTest();

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Partial mode test",0);
  NokiaN70_PutStrCentered("Setting partial area",NokiaN70_GetHeight()/2-48/2);
  NokiaN70_PutStrCentered("to 40,168",NokiaN70_GetHeight()/2-16/2);
  NokiaN70_PutStrCentered("and filling all screen",NokiaN70_GetHeight()/2+16/2);
  NokiaN70_PutStrCentered("with one color",NokiaN70_GetHeight()/2+48/2);
  DWT_Delay(3000000);
  NokiaN70_PartialArea(40,168);
  NokiaN70_PartialMode();
  NokiaN70_ClearScreen(RED);
  DWT_Delay(1000000);
  NokiaN70_ClearScreen(GREEN);
  DWT_Delay(1000000);
  NokiaN70_ClearScreen(BLUE);
  DWT_Delay(1000000);

  NokiaN70_NormalDisplayMode();
  NokiaN70_ClearScreen(BLACK);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Partial mode test",0);
  NokiaN70_PutStrCentered("Setting partial area",NokiaN70_GetHeight()/2-48/2);
  NokiaN70_PutStrCentered("to 168,40",NokiaN70_GetHeight()/2-16/2);
  NokiaN70_PutStrCentered("and filling all screen",NokiaN70_GetHeight()/2+16/2);
  NokiaN70_PutStrCentered("with one color",NokiaN70_GetHeight()/2+48/2);
  DWT_Delay(3000000);
  NokiaN70_PartialArea(168,40);
  NokiaN70_PartialMode();
  NokiaN70_ClearScreen(RED);
  DWT_Delay(1000000);
  NokiaN70_ClearScreen(GREEN);
  DWT_Delay(1000000);
  NokiaN70_ClearScreen(BLUE);
  DWT_Delay(1000000);
  NokiaN70_NormalDisplayMode();

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Orientation test",0);
  DWT_Delay(3000000);

  NokiaN70_OrientationMode(ORIENTATION_LANDSCAPE);
  displayOrientation("Lansdcape");
  NokiaN70_OrientationMode(ORIENTATION_PORTRAIT);
  displayOrientation("Portrait");
  NokiaN70_OrientationMode(ORIENTATION_PORTRAIT_REV);
  displayOrientation("Portrait reversed");
  NokiaN70_OrientationMode(ORIENTATION_LANDSCAPE_REV);
  displayOrientation("Lansdcape reversed");

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_OrientationMode(ORIENTATION_LANDSCAPE);
  NokiaN70_PutStrCentered("Sleep mode test",0);
  NokiaN70_PutStrCentered("Sleeping for 3 seconds",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_Sleep();
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Woken up",NokiaN70_GetHeight()/2-8/2);
  NokiaN70_Wakeup();
  DWT_Delay(3000000);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Display on/off test",0);
  NokiaN70_PutStrCentered("Display off for 3 seconds",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_DisplayOff();
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Display on",NokiaN70_GetHeight()/2-8/2);
  NokiaN70_DisplayOn();
  DWT_Delay(3000000);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Display inversion test",0);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(BLACK);
  GradientBars();
  NokiaN70_PutStrCentered(" Normal ",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_DisplayInversionOn();
  NokiaN70_PutStrCentered("Inverted",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_DisplayInversionOff();
  NokiaN70_PutStrCentered(" Normal ",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Idle mode test",0);
  NokiaN70_PutStrCentered("In idle mode color space",NokiaN70_GetHeight()/2-16/2);
  NokiaN70_PutStrCentered("is reduced to 8 colors",NokiaN70_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  NokiaN70_ClearScreen(BLACK);

  GradientBars();
  NokiaN70_PutStrCentered("Idle mode off",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_IdleModeOn();
  NokiaN70_PutStrCentered("Idle mode  on",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  NokiaN70_IdleModeOff();
  NokiaN70_PutStrCentered("Idle mode off",NokiaN70_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  max_x=NokiaN70_GetWidth();
  max_y=NokiaN70_GetHeight();

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Rectangles",0);
  NokiaN70_PutStrCentered("1000 colored rectangles",NokiaN70_GetHeight()/2);
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

    NokiaN70_Fill(x0,y0,x1,y1,xor128()%0xFFFFFF);
  }
  t_end=DWT_Get();
  sprintf(buf,"in %d ms",(t_end-t_start)/(SystemCoreClock/1000));
  NokiaN70_ClearScreen(BLACK);
  NokiaN70_PutStrCentered("Rectangles",0);
  NokiaN70_PutStrCentered("1000 colored rectangles",NokiaN70_GetHeight()/2);
  NokiaN70_PutStrCentered(buf,NokiaN70_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  NokiaN70_ClearScreen(BLACK);
  NokiaN70_ColorMode(COLOR_16BIT);
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

  NokiaN70_SetFont(FONT_8x14);
  NokiaN70_PutStrCentered("Nokia N70 LCD - 3D fire",0);
  NokiaN70_PutStrCentered("kbiva.wordpress.com",160);

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
        //NokiaN70_SetPixel(j,i+16,pal[v]);
      }
    }
    NokiaN70_SetWriteWindow(0,16,width-1,16+height-1);
    for(i=0;i<height*width;i++) {
      dest1=((pal[buf[i]] & 0xf80000) >> 16) | ((pal[buf[i]] & 0xe000) >> 13);
      LCD_DAT8 = dest1;
      dest1=((pal[buf[i]] & 0x1c00) >> 5) | ((pal[buf[i]] & 0xf8) >> 3);
      LCD_DAT8 = dest1;
    }
  }
}
