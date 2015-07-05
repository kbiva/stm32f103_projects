/*
 *  main.c
 *
 *  Nokia 6151 LCD demo
 *
 *  Author: Kestutis Bivainis
 *
 */

#include "stm32f10x_conf.h"
#include "SPFD54124B.h"
#include "colors.h"
#include "delay.h"
#include "hy_stm32.h"
#include "jpeg.h"
#include <stdio.h>
#include "adc.h"
#include "explosion.h"
#include "xorshift.h"

extern uint8_t data1[];
extern uint8_t data2[];
extern uint8_t data3[];
extern uint8_t data4[];
extern uint8_t data5[];

extern uint8_t* dataptr;
COLOR_MODE current_color_mode;

void displayOrientation(char* text) {

  uint32_t i,
           max_x=SPFD54124B_GetWidth(),
           max_y=SPFD54124B_GetHeight();

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("TOP",0);
  SPFD54124B_PutStrCentered("BOTTOM",max_y-14);
  SPFD54124B_PutStr("LEFT",0,max_y/2);
  SPFD54124B_PutStr("RIGHT",max_x-8*5,max_y/2);
  SPFD54124B_PutStrCentered("Orientation:",max_y/4-16/2);
  SPFD54124B_PutStrCentered(text,max_y/4+16/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("10K colored",max_y/2-16/2);
  SPFD54124B_PutStrCentered("text lines",max_y/2+16/2);
  DWT_Delay(3000000);

  SPFD54124B_SetFont(FONT_6x8);
  for(i=0;i<10000;i++) {
    SPFD54124B_SetTextColors(xor128()%0xFFFFFF,xor128()%0xFFFFFF);
    SPFD54124B_PutStr("This is some text",xor128()%max_x,xor128()%max_y);
  }
  SPFD54124B_SetTextColors(WHITE,BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  DWT_Delay(3000000);
}

void GradientFill(uint16_t y1,uint16_t y2,uint32_t color_inc) {

  uint32_t i,j,c;

  c=0;
  i=0;

  for(j=0;j<64;j++) {

    SPFD54124B_Fill(i,y1,i+1,y2,c);
    i+=2;
    c+=color_inc;
  }
}

void GradientBars(void) {

  GradientFill(0,                         SPFD54124B_GetHeight()/7-1,  0x040000);
  GradientFill(SPFD54124B_GetHeight()/7,  SPFD54124B_GetHeight()/7*2-1,0x000400);
  GradientFill(SPFD54124B_GetHeight()/7*2,SPFD54124B_GetHeight()/7*3-1,0x000004);
  GradientFill(SPFD54124B_GetHeight()/7*3,SPFD54124B_GetHeight()/7*4-1,0x040004);
  GradientFill(SPFD54124B_GetHeight()/7*4,SPFD54124B_GetHeight()/7*5-1,0x000404);
  GradientFill(SPFD54124B_GetHeight()/7*5,SPFD54124B_GetHeight()/7*6-1,0x040400);
  GradientFill(SPFD54124B_GetHeight()/7*6,SPFD54124B_GetHeight()-1,    0x040404);
}

void ColorDepthTest(void) {

  uint8_t pixelFormat;

  SPFD54124B_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  GradientBars();
  switch(pixelFormat) {
    case COLOR_12BIT:SPFD54124B_PutStr("12",138,SPFD54124B_GetHeight()/2-48/2);break;
    case COLOR_16BIT:SPFD54124B_PutStr("16",138,SPFD54124B_GetHeight()/2-48/2);break;
    case COLOR_18BIT:SPFD54124B_PutStr("18",138,SPFD54124B_GetHeight()/2-48/2);break;
    default:SPFD54124B_PutStr("--",138,SPFD54124B_GetHeight()/2-48/2);break;
  }
  SPFD54124B_PutStr("b",142,SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStr("i",142,SPFD54124B_GetHeight()/2+16/2);
  SPFD54124B_PutStr("t",142,SPFD54124B_GetHeight()/2+48/2);
  DWT_Delay(3000000);
}

void HardwareScrollingTest(void) {

  uint32_t i,j;

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Hardware",SPFD54124B_GetHeight()/2-32/2);
  SPFD54124B_PutStrCentered("scrolling",SPFD54124B_GetHeight()/2);
  SPFD54124B_PutStrCentered("test",SPFD54124B_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  SPFD54124B_ScrollArea(0,162);

  for(j=0;j<20;j++) {
    for(i=0;i<30;i++) {
      SPFD54124B_SetScrollPosition(i);
      DWT_Delay(10000);
    }

    for(i=0;i<30;i++) {
      SPFD54124B_SetScrollPosition(30-i);
      DWT_Delay(10000);
    }
  }
  SPFD54124B_SetScrollPosition(0);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_OrientationMode(ORIENTATION_PORTRAIT);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Hardware",SPFD54124B_GetHeight()/2-32/2);
  SPFD54124B_PutStrCentered("scrolling",SPFD54124B_GetHeight()/2);
  SPFD54124B_PutStrCentered("test",SPFD54124B_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  SPFD54124B_ScrollArea(0,162);

  for(j=0;j<20;j++) {
    for(i=0;i<30;i++) {
      SPFD54124B_SetScrollPosition(i);
      DWT_Delay(10000);
    }

    for(i=0;i<30;i++) {
      SPFD54124B_SetScrollPosition(30-i);
      DWT_Delay(10000);
    }
  }
  SPFD54124B_SetScrollPosition(0);
  SPFD54124B_OrientationMode(ORIENTATION_LANDSCAPE);
}

void GammaCurvesTest(void) {

  SPFD54124B_PutStrCentered("Gamma curve 1 (2.2)",SPFD54124B_GetHeight()/2-8/2);
  SPFD54124B_Gamma(GAMMA_CURVE1);
  DWT_Delay(3000000);
  SPFD54124B_PutStrCentered("Gamma curve 2 (1.8)",SPFD54124B_GetHeight()/2-8/2);
  SPFD54124B_Gamma(GAMMA_CURVE2);
  DWT_Delay(3000000);
  SPFD54124B_PutStrCentered("Gamma curve 3 (2.5)",SPFD54124B_GetHeight()/2-8/2);
  SPFD54124B_Gamma(GAMMA_CURVE3);
  DWT_Delay(3000000);
  SPFD54124B_PutStrCentered("Gamma curve 4 (1.0)",SPFD54124B_GetHeight()/2-8/2);
  SPFD54124B_Gamma(GAMMA_CURVE4);
  DWT_Delay(3000000);
}

void sprintfReadBack(uint8_t pixelFormat,char* buf,uint8_t* buf1) {

  switch(pixelFormat) {
    case COLOR_12BIT:sprintf(buf,"%02X%01X",(buf1[0]&0xF0)|((buf1[1]&0xF0)>>4),buf1[2]>>4);break;
    case COLOR_16BIT:sprintf(buf,"%02X%02X",(buf1[0]&0xF8)|((buf1[1]&0xFC)>>5),((buf1[1]&0x1C)<<3)|((buf1[2]&0xF8)>>3));break;
    case COLOR_18BIT:sprintf(buf,"%02X%02X%02X",buf1[0],buf1[1],buf1[2]);break;
    default:sprintf(buf,"error");break;
  }
}

void MemoryReadTest(void) {

  uint8_t buf1[16];
  char buf[32];
  uint8_t pixelFormat;

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Reading from memory",0);

  SPFD54124B_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  switch(pixelFormat) {
    case COLOR_12BIT:SPFD54124B_PutStrCentered("12bit color depth",14);break;
    case COLOR_16BIT:SPFD54124B_PutStrCentered("16bit color depth",14);break;
    case COLOR_18BIT:SPFD54124B_PutStrCentered("18bit color depth",14);break;
    default:SPFD54124B_PutStrCentered("error",14);break;
  }

  SPFD54124B_SetFont(FONT_6x8);

  SPFD54124B_SetPixel(2,2,BLACK);
  SPFD54124B_PutStr("Set pixel at 2,2 to BLACK",0,30);
  SPFD54124B_SetPixel(2,3,RED);
  SPFD54124B_PutStr("Set pixel at 2,3 to RED",0,39);
  SPFD54124B_SetPixel(2,4,GREEN);
  SPFD54124B_PutStr("Set pixel at 2,4 to GREEN",0,48);
  SPFD54124B_SetPixel(2,5,BLUE);
  SPFD54124B_PutStr("Set pixel at 2,5 to BLUE",0,57);
  SPFD54124B_SetPixel(2,6,WHITE);
  SPFD54124B_PutStr("Set pixel at 2,6 to WHITE",0,66);

  SPFD54124B_PutStr("Read pixel at 2,2:",0,75);
  SPFD54124B_ReadMemory(2,2,2,2,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  SPFD54124B_PutStr(buf,120,75);

  SPFD54124B_PutStr("Read pixel at 2,3:",0,84);
  SPFD54124B_ReadMemory(2,3,2,3,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  SPFD54124B_PutStr(buf,120,84);

  SPFD54124B_PutStr("Read pixel at 2,4:",0,93);
  SPFD54124B_ReadMemory(2,4,2,4,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  SPFD54124B_PutStr(buf,120,93);

  SPFD54124B_PutStr("Read pixel at 2,5:",0,102);
  SPFD54124B_ReadMemory(2,5,2,5,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  SPFD54124B_PutStr(buf,120,102);

  SPFD54124B_PutStr("Read pixel at 2,6:",0,111);
  SPFD54124B_ReadMemory(2,6,2,6,buf1);
  sprintfReadBack(pixelFormat,buf,buf1);
  SPFD54124B_PutStr(buf,120,111);

  DWT_Delay(3000000);
}

void printColorMode(void) {

  switch(current_color_mode) {
    case COLOR_12BIT:SPFD54124B_PutStrCentered("12 bit",0);break;
    case COLOR_16BIT:SPFD54124B_PutStrCentered("16 bit",0);break;
    case COLOR_18BIT:SPFD54124B_PutStrCentered("18 bit",0);break;
    default:SPFD54124B_PutStrCentered("error",0);break;
  }
}

void JPEGTest(uint8_t* picture) {

  uint8_t pixelFormat;

  SPFD54124B_ColorMode(COLOR_18BIT);
  SPFD54124B_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);

  SPFD54124B_ColorMode(COLOR_16BIT);
  SPFD54124B_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);

  SPFD54124B_ColorMode(COLOR_12BIT);
  SPFD54124B_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,&pixelFormat);
  current_color_mode=(COLOR_MODE)pixelFormat;
  dataptr=picture;
  load_jpg();
  printColorMode();
  DWT_Delay(3000000);

}

void Read_Registers(void) {

  char buf[16];
  uint8_t regs[5];

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Reading registers",0);
  SPFD54124B_SetFont(FONT_6x8);

  SPFD54124B_PutStr("REG  NAME    VALUE",0,19);
  SPFD54124B_Fill(0,29,SPFD54124B_GetWidth()-1,29,WHITE);

  SPFD54124B_ReadRegister(READ_DISPLAY_ID,3,regs);
  SPFD54124B_PutStr("0x04 ID:",0,33);
  sprintf(buf,"%02X %02X %02X",regs[0],regs[1],regs[2]);
  SPFD54124B_PutStr(buf,80,33);

  SPFD54124B_ReadRegister(READ_DISPLAY_STATUS,4,regs);
  SPFD54124B_PutStr("0x09 STATUS:",0,42);
  sprintf(buf,"%02X %02X %02X %02X",regs[0],regs[1],regs[2],regs[3]);
  SPFD54124B_PutStr(buf,80,42);

  SPFD54124B_ReadRegister(READ_DISPLAY_POWER_MODE,1,regs);
  SPFD54124B_PutStr("0x0A POWER:",0,51);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,51);

  SPFD54124B_ReadRegister(READ_DISPLAY_MADCTL,1,regs);
  SPFD54124B_PutStr("0x0B MADCTL:",0,60);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,60);

  SPFD54124B_ReadRegister(READ_DISPLAY_PIXEL_FORMAT,1,regs);
  SPFD54124B_PutStr("0x0C PIXEL:",0,69);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,69);

  SPFD54124B_ReadRegister(READ_DISPLAY_IMAGE_MODE,1,regs);
  SPFD54124B_PutStr("0x0D IMAGE:",0,78);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,78);

  SPFD54124B_ReadRegister(READ_DISPLAY_SIGNAL_MODE,1,regs);
  SPFD54124B_PutStr("0x0E SIGNAL:",0,87);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,87);

  SPFD54124B_ReadRegister(READ_DISPLAY_SELF_DIAGNOSTICS,1,regs);
  SPFD54124B_PutStr("0x0F DIAG:",0,96);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,96);

  SPFD54124B_ReadRegister(READ_ID1,1,regs);
  SPFD54124B_PutStr("0xDA ID1:",0,105);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,105);

  SPFD54124B_ReadRegister(READ_ID2,1,regs);
  SPFD54124B_PutStr("0xDB ID2:",0,114);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,114);

  SPFD54124B_ReadRegister(READ_ID3,1,regs);
  SPFD54124B_PutStr("0xDC ID3:",0,123);
  sprintf(buf,"%02X",regs[0]);
  SPFD54124B_PutStr(buf,80,123);
  DWT_Delay(3000000);
}

uint8_t buf1[128];

int main (void) {

  uint32_t i,max_x,max_y,t_start,t_end;
  uint16_t x0,y0,x1,y1,tmp;
  char buf[16];

  DWT_Init();

  DWT_Delay(1000000);

  BSP_Buttons_Init(BUTTON_MODE_EXTI);

  ADC_DMA_Configuration();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  SPFD54124B_Init(0,2);
  SPFD54124B_ColorMode(COLOR_18BIT);
  SPFD54124B_OrientationMode(ORIENTATION_LANDSCAPE);
  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_Gamma(GAMMA_CURVE3);

  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Nokia 6151 LCD",0);
  SPFD54124B_SetFont(FONT_6x8);
  SPFD54124B_PutStr("Markings on the back:",0,14);
  SPFD54124B_PutStr("4851050 H018IT01 V0",6,23);
  SPFD54124B_PutStr("S2C0704686226 MP3.2",6,32);
  SPFD54124B_PutStr("S4A74SI00ZC",6,41);
  SPFD54124B_PutStr("LCD: 1.8 inches TFT",0,50);
  SPFD54124B_PutStr("Resolution: 162x132 px",0,59);
  SPFD54124B_PutStr("Colors: 262K (18 bit/pixel)",0,68);
  SPFD54124B_PutStr("Controller: Orise Tech",0,77);
  SPFD54124B_PutStr("SPFD54124B compatible",6,86);
  SPFD54124B_PutStr("Connector: 22pin with 0.5mm",0,95);
  SPFD54124B_PutStr("pitch (Hirose DF23-22DS)",6,104);
  SPFD54124B_PutStr("MPU Interface:",0,113);
  SPFD54124B_PutStr("8bit 8080 parallel",6,122);
  DWT_Delay(5000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Color mode used",SPFD54124B_GetHeight()/2-32/2);
  SPFD54124B_PutStrCentered("in this demo is",SPFD54124B_GetHeight()/2);
  SPFD54124B_PutStrCentered("262K (18 bit/pixel)",SPFD54124B_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Color test",0);
  SPFD54124B_PutStrCentered("Basic colors",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(RED);
  SPFD54124B_PutStrCentered("Red",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(GREEN);
  SPFD54124B_PutStrCentered("Green",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLUE);
  SPFD54124B_PutStrCentered("Blue",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(CYAN);
  SPFD54124B_PutStrCentered("Cyan",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(MAGENTA);
  SPFD54124B_PutStrCentered("Magenta",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(YELLOW);
  SPFD54124B_PutStrCentered("Yellow",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Color test",0);
  SPFD54124B_PutStrCentered("Gradient with",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("basic colors",SPFD54124B_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLACK);
  GradientBars();
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Color depth test",0);
  SPFD54124B_PutStrCentered("Gradient color bars",SPFD54124B_GetHeight()/2-48/2);
  SPFD54124B_PutStrCentered("are displayed at",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("different LCD",SPFD54124B_GetHeight()/2+16/2);
  SPFD54124B_PutStrCentered("color depth",SPFD54124B_GetHeight()/2+48/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLACK);
  ColorDepthTest();
  SPFD54124B_ColorMode(COLOR_16BIT);
  ColorDepthTest();
  SPFD54124B_ColorMode(COLOR_12BIT);
  ColorDepthTest();
  SPFD54124B_ColorMode(COLOR_16BIT);
  ColorDepthTest();
  SPFD54124B_ColorMode(COLOR_18BIT);
  ColorDepthTest();

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("JPEG pictures test",0);
  SPFD54124B_PutStrCentered("The same jpeg",SPFD54124B_GetHeight()/2-48/2);
  SPFD54124B_PutStrCentered("picture is displayed",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("at different LCD",SPFD54124B_GetHeight()/2+16/2);
  SPFD54124B_PutStrCentered("color depth",SPFD54124B_GetHeight()/2+48/2);

  DWT_Delay(3000000);
  JPEGTest(data1);
  JPEGTest(data2);
  JPEGTest(data3);
  JPEGTest(data4);
  JPEGTest(data5);
  SPFD54124B_ColorMode(COLOR_18BIT);
  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Gamma curves test",0);
  SPFD54124B_PutStrCentered("There are 4",SPFD54124B_GetHeight()/2-48/2);
  SPFD54124B_PutStrCentered("predefined gamma",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("curves. Default is",SPFD54124B_GetHeight()/2+16/2);
  SPFD54124B_PutStrCentered("gamma curve 1",SPFD54124B_GetHeight()/2+48/2);
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_Gamma(GAMMA_CURVE1);
  GradientFill(0,SPFD54124B_GetHeight()-1,0x040000);
  GammaCurvesTest();

  SPFD54124B_Gamma(GAMMA_CURVE1);
  SPFD54124B_ClearScreen(BLACK);
  GradientFill(0,SPFD54124B_GetHeight()-1,0x0400);
  GammaCurvesTest();

  SPFD54124B_Gamma(GAMMA_CURVE1);
  SPFD54124B_ClearScreen(BLACK);
  GradientFill(0,SPFD54124B_GetHeight()-1,0x04);
  GammaCurvesTest();

  SPFD54124B_Gamma(GAMMA_CURVE3);

  Read_Registers();

  MemoryReadTest();
  SPFD54124B_ColorMode(COLOR_16BIT);
  MemoryReadTest();
  SPFD54124B_ColorMode(COLOR_12BIT);
  MemoryReadTest();
  SPFD54124B_ColorMode(COLOR_18BIT);
  HardwareScrollingTest();

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Partial mode test",0);
  SPFD54124B_PutStrCentered("Setting partial area",SPFD54124B_GetHeight()/2-48/2);
  SPFD54124B_PutStrCentered("to 30,132 and",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("filling all screen",SPFD54124B_GetHeight()/2+16/2);
  SPFD54124B_PutStrCentered("with one color",SPFD54124B_GetHeight()/2+48/2);
  DWT_Delay(3000000);
  SPFD54124B_PartialArea(30,132);
  SPFD54124B_PartialMode();
  SPFD54124B_ClearScreen(RED);
  DWT_Delay(1000000);
  SPFD54124B_ClearScreen(GREEN);
  DWT_Delay(1000000);
  SPFD54124B_ClearScreen(BLUE);
  DWT_Delay(1000000);

  SPFD54124B_NormalDisplayMode();
  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Partial mode test",0);
  SPFD54124B_PutStrCentered("Setting partial area",SPFD54124B_GetHeight()/2-48/2);
  SPFD54124B_PutStrCentered("to 132,30 and ",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("filling all screen",SPFD54124B_GetHeight()/2+16/2);
  SPFD54124B_PutStrCentered("with one color",SPFD54124B_GetHeight()/2+48/2);
  DWT_Delay(3000000);
  SPFD54124B_PartialArea(132,30);
  SPFD54124B_PartialMode();
  SPFD54124B_ClearScreen(RED);
  DWT_Delay(1000000);
  SPFD54124B_ClearScreen(GREEN);
  DWT_Delay(1000000);
  SPFD54124B_ClearScreen(BLUE);
  DWT_Delay(1000000);
  SPFD54124B_NormalDisplayMode();

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("Orientation test",0);
  DWT_Delay(3000000);

  SPFD54124B_OrientationMode(ORIENTATION_LANDSCAPE);
  displayOrientation("Lansdcape");
  SPFD54124B_OrientationMode(ORIENTATION_PORTRAIT);
  displayOrientation("Portrait");
  SPFD54124B_OrientationMode(ORIENTATION_PORTRAIT_REV);
  displayOrientation("Portrait reversed");
  SPFD54124B_OrientationMode(ORIENTATION_LANDSCAPE_REV);
  displayOrientation("Lansdcape reversed");

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_OrientationMode(ORIENTATION_LANDSCAPE);
  SPFD54124B_PutStrCentered("Sleep mode test",0);
  SPFD54124B_PutStrCentered("Sleeping for 3 sec",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_Sleep();
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Woken up",SPFD54124B_GetHeight()/2-8/2);
  SPFD54124B_Wakeup();
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Display on/off test",0);
  SPFD54124B_PutStrCentered("Display off",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("for 3 seconds",SPFD54124B_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  SPFD54124B_DisplayOff();
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Display on",SPFD54124B_GetHeight()/2-8/2);
  SPFD54124B_DisplayOn();
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Display inversion",0);
  SPFD54124B_PutStrCentered("test",16);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLACK);
  GradientBars();
  SPFD54124B_PutStrCentered(" Normal ",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_DisplayInversionOn();
  SPFD54124B_PutStrCentered("Inverted",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_DisplayInversionOff();
  SPFD54124B_PutStrCentered(" Normal ",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Idle mode test",0);
  SPFD54124B_PutStrCentered("In idle mode color",SPFD54124B_GetHeight()/2-32/2);
  SPFD54124B_PutStrCentered("space is reduced",SPFD54124B_GetHeight()/2);
  SPFD54124B_PutStrCentered("to 8 colors",SPFD54124B_GetHeight()/2+32/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLACK);

  GradientBars();
  SPFD54124B_PutStrCentered("Idle mode off",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_IdleModeOn();
  SPFD54124B_PutStrCentered("Idle mode  on",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);
  SPFD54124B_IdleModeOff();
  SPFD54124B_PutStrCentered("Idle mode off",SPFD54124B_GetHeight()/2-8/2);
  DWT_Delay(3000000);

  max_x=SPFD54124B_GetWidth();
  max_y=SPFD54124B_GetHeight();

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Rectangles",0);
  SPFD54124B_PutStrCentered("1000 colored",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("rectangles",SPFD54124B_GetHeight()/2+16/2);
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

    SPFD54124B_Fill(x0,y0,x1,y1,xor128()%0xFFFFFF);
  }
  t_end=DWT_Get();
  sprintf(buf,"in %d ms",(t_end-t_start)/(SystemCoreClock/1000));
  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Rectangles",0);
  SPFD54124B_PutStrCentered("1000 colored",SPFD54124B_GetHeight()/2-32/2);
  SPFD54124B_PutStrCentered("rectangles",SPFD54124B_GetHeight()/2);
  SPFD54124B_PutStrCentered(buf,SPFD54124B_GetHeight()/2+32/2);
  DWT_Delay(3000000);

  SPFD54124B_ClearScreen(BLACK);
  SPFD54124B_PutStrCentered("Particle Explosion",SPFD54124B_GetHeight()/2-16/2);
  SPFD54124B_PutStrCentered("Effect",SPFD54124B_GetHeight()/2+16/2);
  DWT_Delay(3000000);
  SPFD54124B_ClearScreen(BLACK);

  explosion();
  while(1){};
}
