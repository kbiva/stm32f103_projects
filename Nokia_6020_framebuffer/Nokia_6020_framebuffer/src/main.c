/*
 *  main.c
 *
 *  Using various methods to access Nokia 6020 LCD
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "hy_stm32.h"
#include "PCF8833.h"
#include <stdio.h>
#include "colors_16bit.h"
#include "rotating_cube_checkboard.h"

extern volatile uint8_t framebuffer_transfer_complete;

int main(void) {

  uint32_t start_us,end_us;
  double fps1,fps2,fps3,fps4,fps5,fps6;
  char buf[32];

  SystemCoreClockUpdate();

  DWT_Init();
  DWT_Delay(200000);

  BSP_Button_Init(BUTTON3,BUTTON_MODE_EXTI);
  BSP_Button_Init(BUTTON4,BUTTON_MODE_EXTI);

  PCF8833_Init(PCF8833_ACCESS_BITBANG);
  PCF8833_ColorMode(PCF8833_COLOR_8BIT);
  PCF8833_SetupColor(RGB8ColorMap_Nokia6020,48);
  PCF8833_SetOrientation(PCF8833_ORIENTATION_LANDSCAPE1,0);
  PCF8833_SetContrast(NOKIA6020_SETCON_DATA);
  PCF8833_ClearScreen(BLACK);

  PCF8833_SetFont(PCF8833_FONT_8x14);
  PCF8833_PutStr("Nokia 6020 LCD",9,1);
  PCF8833_PutStr("and",49,16);
  sprintf(buf,"STM32F103:%dMhz",SystemCoreClock/1000000);
  PCF8833_PutStr(buf,1,31);
  PCF8833_PutStr("(overclocked)",9,46);
  PCF8833_PutStr("Rotating 3D cube",1,65);
  PCF8833_PutStr("with",49,80);
  PCF8833_PutStr("checkerboard",17,95);
  PCF8833_PutStr("background",25,110);
  DWT_Delay(3000000);

  PCF8833_ClearScreen(BLACK);
  PCF8833_SetFont(PCF8833_FONT_6x8);
  PCF8833_PutStr("",1,6);
  PCF8833_PutStr("Each time 200 frames",1,15);
  PCF8833_PutStr("are calculated and",1,24);
  PCF8833_PutStr("displayed, using",1,33);
  PCF8833_PutStr("six different methods",1,42);
  PCF8833_PutStr("to access LCD:",1,51);
  PCF8833_PutStr("1.Pixel,Bit Banging",1,60);
  PCF8833_PutStr("2.Pixel,SPI",1,69);
  PCF8833_PutStr("3.Framebuffer,",1,78);
  PCF8833_PutStr("  Bit Banging",1,87);
  PCF8833_PutStr("4.Framebuffer,SPI",1,96);
  PCF8833_PutStr("5.Framebuffer,SPI,DMA",1,105);
  PCF8833_PutStr("6.Double framebuffer,",1,114);
  PCF8833_PutStr("  SPI,DMA",1,123);
  DWT_Delay(3000000);

  PCF8833_ClearScreen(BLACK);
  PCF8833_PutStr("Nokia 6020 LCD",1,1);
  sprintf(buf,"STM32F103: %dMhz",SystemCoreClock/1000000);
  PCF8833_PutStr(buf,1,10);
  PCF8833_Line(1,48,130,48,WHITE);
  PCF8833_PutStr("Software bit-banging:",1,52);
  PCF8833_PutStr("direct output to LCD.",1,61);
  PCF8833_PutStr("8 transfers for every",1,70);
  PCF8833_PutStr("pixel. Each transfer",1,79);
  PCF8833_PutStr("is 9 bits.",1,88);
  PCF8833_PutStr("(Flicker is visible)",1,97);
  DWT_Delay(3000000);
  start_us=DWT_Get();
  display_rotating_cube_checkboard_pixel_BB();
  end_us=DWT_Get();
  // When MCU runs at 128 Mhz, DWT_Delay function has maximum
  // delay of (2^32)/128000000=33.554 seconds before it wraps around.
  // I know that my code runs just a litle bit for more than 33.554 seconds,
  // so I have to add 33554 to my calculations.
  sprintf(buf,"%d ms",(end_us-start_us)/(SystemCoreClock/1000)+33554);
  PCF8833_ClearScreen(BLACK);
  PCF8833_PutStr("200 frames in",1,51);
  PCF8833_PutStr(buf,82,51);
  PCF8833_SetFont(PCF8833_FONT_8x14);
  PCF8833_PutStr("FPS:",1,71);
  fps1=1000./((end_us-start_us)/(SystemCoreClock/(1000/200.))+33.554432*5);
  sprintf(buf,"%d.%d",(int16_t)fps1, (((int16_t)(fps1 * 10.0)) % 10));
  PCF8833_PutStr(buf,40,71);
  DWT_Delay(3000000);

  PCF8833_Init(PCF8833_ACCESS_SPI9BITS);
  PCF8833_ColorMode(PCF8833_COLOR_8BIT);
  PCF8833_SetupColor(RGB8ColorMap_Nokia6020,48);
  PCF8833_SetOrientation(PCF8833_ORIENTATION_LANDSCAPE1,0);
  PCF8833_SetContrast(NOKIA6020_SETCON_DATA);
  PCF8833_ClearScreen(BLACK);

  PCF8833_SetFont(PCF8833_FONT_6x8);
  PCF8833_PutStr("Nokia 6020 LCD",1,1);
  sprintf(buf,"STM32F103: %dMhz",SystemCoreClock/1000000);
  PCF8833_PutStr(buf,1,10);
  sprintf(buf,"SPI: %dMhz",(SystemCoreClock>>(((SPI1->CR1&0x38)>>3)+1))/1000000);
  PCF8833_PutStr(buf,1,19);
  PCF8833_Line(1,48,130,48,WHITE);
  PCF8833_PutStr("8-bit SPI over 9-bit",1,52);
  PCF8833_PutStr("SPI: direct output",1,61);
  PCF8833_PutStr("to LCD.",1,70);
  PCF8833_PutStr("8 transfers for every",1,79);
  PCF8833_PutStr("pixel. Each transfer",1,88);
  PCF8833_PutStr("is 9 bits.",1,97);
  PCF8833_PutStr("(Flicker is visible)",1,106);

  DWT_Delay(3000000);
  start_us=DWT_Get();
  display_rotating_cube_checkboard_pixel_SPI();
  end_us=DWT_Get();
  sprintf(buf,"%d ms",(end_us-start_us)/(SystemCoreClock/1000));
  PCF8833_ClearScreen(BLACK);
  PCF8833_PutStr("200 frames in",1,51);
  PCF8833_PutStr(buf,82,51);
  PCF8833_SetFont(PCF8833_FONT_8x14);
  PCF8833_PutStr("FPS:",1,71);
  fps2=1000./((end_us-start_us)/(SystemCoreClock/(1000/200.)));
  sprintf(buf,"%d.%d",(int16_t)fps2, (((int16_t)(fps2 * 10.0)) % 10));
  PCF8833_PutStr(buf,40,71);
  DWT_Delay(3000000);

  PCF8833_Init(PCF8833_ACCESS_BITBANG);
  PCF8833_ColorMode(PCF8833_COLOR_8BIT);
  PCF8833_SetupColor(RGB8ColorMap_Nokia6020,48);
  PCF8833_SetOrientation(PCF8833_ORIENTATION_LANDSCAPE1,0);
  PCF8833_SetContrast(NOKIA6020_SETCON_DATA);
  PCF8833_ClearScreen(BLACK);

  PCF8833_SetFont(PCF8833_FONT_6x8);
  PCF8833_PutStr("Nokia 6020 LCD",1,1);
  sprintf(buf,"STM32F103: %dMhz",SystemCoreClock/1000000);
  PCF8833_PutStr(buf,1,10);
  PCF8833_Line(1,48,130,48,WHITE);
  PCF8833_PutStr("Software bit-banging:",1,52);
  PCF8833_PutStr("output to LCD using",1,61);
  PCF8833_PutStr("framebuffer.",1,70);
  PCF8833_PutStr("1 transfer for every",1,79);
  PCF8833_PutStr("pixel. Each transfer",1,88);
  PCF8833_PutStr("is 9 bits.",1,97);
  PCF8833_PutStr("(Flicker-free)",1,106);

  DWT_Delay(3000000);
  start_us=DWT_Get();
  display_rotating_cube_checkboard_framebuffer_BB();
  end_us=DWT_Get();
  sprintf(buf,"%d ms",(end_us-start_us)/(SystemCoreClock/1000));
  PCF8833_ClearScreen(BLACK);
  PCF8833_PutStr("200 frames in",1,51);
  PCF8833_PutStr(buf,82,51);
  PCF8833_SetFont(PCF8833_FONT_8x14);
  PCF8833_PutStr("FPS:",1,71);
  fps3=1000./((end_us-start_us)/(SystemCoreClock/(1000/200.)));
  sprintf(buf,"%d.%d",(int16_t)fps3, (((int16_t)(fps3 * 10.0)) % 10));
  PCF8833_PutStr(buf,40,71);
  DWT_Delay(3000000);

  PCF8833_Init(PCF8833_ACCESS_SPI9BITS);
  PCF8833_ColorMode(PCF8833_COLOR_8BIT);
  PCF8833_SetupColor(RGB8ColorMap_Nokia6020,48);
  PCF8833_SetOrientation(PCF8833_ORIENTATION_LANDSCAPE1,0);
  PCF8833_SetContrast(NOKIA6020_SETCON_DATA);
  PCF8833_ClearScreen(BLACK);

  PCF8833_SetFont(PCF8833_FONT_6x8);
  PCF8833_PutStr("Nokia 6020 LCD",1,1);
  sprintf(buf,"STM32F103: %dMhz",SystemCoreClock/1000000);
  PCF8833_PutStr(buf,1,10);
  sprintf(buf,"SPI: %dMhz",(SystemCoreClock>>(((SPI1->CR1&0x38)>>3)+1))/1000000);
  PCF8833_PutStr(buf,1,19);
  PCF8833_Line(1,48,130,48,WHITE);
  PCF8833_PutStr("8-bit SPI over 9-bit",1,52);
  PCF8833_PutStr("SPI: output to LCD",1,61);
  PCF8833_PutStr("using framebuffer.",1,70);
  PCF8833_PutStr("1 transfer for every",1,79);
  PCF8833_PutStr("pixel. Each transfer",1,88);
  PCF8833_PutStr("is 9 bits.",1,97);
  PCF8833_PutStr("(Flicker-free)",1,106);

  DWT_Delay(3000000);
  start_us=DWT_Get();
  display_rotating_cube_checkboard_framebuffer_SPI();
  end_us=DWT_Get();
  sprintf(buf,"%d ms",(end_us-start_us)/(SystemCoreClock/1000));
  PCF8833_ClearScreen(BLACK);
  PCF8833_PutStr("200 frames in",1,51);
  PCF8833_PutStr(buf,82,51);
  PCF8833_SetFont(PCF8833_FONT_8x14);
  PCF8833_PutStr("FPS:",1,71);
  fps4=1000./((end_us-start_us)/(SystemCoreClock/(1000/200.)));
  sprintf(buf,"%d.%d",(int16_t)fps4, (((int16_t)(fps4 * 10.0)) % 10));
  PCF8833_PutStr(buf,40,71);
  DWT_Delay(3000000);

  PCF8833_Init(PCF8833_ACCESS_SPI9BITS);
  PCF8833_ColorMode(PCF8833_COLOR_8BIT);
  PCF8833_SetupColor(RGB8ColorMap_Nokia6020,48);
  PCF8833_SetOrientation(PCF8833_ORIENTATION_LANDSCAPE1,0);
  PCF8833_SetContrast(NOKIA6020_SETCON_DATA);
  PCF8833_ClearScreen(BLACK);

  PCF8833_SetFont(PCF8833_FONT_6x8);
  PCF8833_PutStr("Nokia 6020 LCD",1,1);
  sprintf(buf,"STM32F103: %dMhz",SystemCoreClock/1000000);
  PCF8833_PutStr(buf,1,10);
  sprintf(buf,"SPI: %dMhz",(SystemCoreClock>>(((SPI1->CR1&0x38)>>3)+1))/1000000);
  PCF8833_PutStr(buf,1,19);
  PCF8833_Line(1,48,130,48,WHITE);
  PCF8833_PutStr("8-bit SPI over 9-bit",1,52);
  PCF8833_PutStr("SPI: output to LCD",1,61);
  PCF8833_PutStr("using framebuffer",1,70);
  PCF8833_PutStr("and DMA.",1,79);
  PCF8833_PutStr("1 transfer for every",1,88);
  PCF8833_PutStr("pixel. Each transfer",1,97);
  PCF8833_PutStr("is 9 bits.",1,106);
  PCF8833_PutStr("(Flicker-free)",1,115);

  DWT_Delay(3000000);
  start_us=DWT_Get();
  display_rotating_cube_checkboard_framebuffer_SPI_DMA();
  end_us=DWT_Get();
  sprintf(buf,"%d ms",(end_us-start_us)/(SystemCoreClock/1000));
  PCF8833_ClearScreen(BLACK);
  PCF8833_PutStr("200 frames in",1,51);
  PCF8833_PutStr(buf,82,51);
  PCF8833_SetFont(PCF8833_FONT_8x14);
  PCF8833_PutStr("FPS:",1,71);
  fps5=1000./((end_us-start_us)/(SystemCoreClock/(1000/200.)));
  sprintf(buf,"%d.%d",(int16_t)fps5, (((int16_t)(fps5 * 10.0)) % 10));
  PCF8833_PutStr(buf,40,71);
  DWT_Delay(3000000);

  PCF8833_Init(PCF8833_ACCESS_SPI9BITS);
  PCF8833_ColorMode(PCF8833_COLOR_8BIT);
  PCF8833_SetupColor(RGB8ColorMap_Nokia6020,48);
  PCF8833_SetOrientation(PCF8833_ORIENTATION_LANDSCAPE1,0);
  PCF8833_SetContrast(NOKIA6020_SETCON_DATA);
  PCF8833_ClearScreen(BLACK);

  PCF8833_SetFont(PCF8833_FONT_6x8);
  PCF8833_PutStr("Nokia 6020 LCD",1,1);
  sprintf(buf,"STM32F103: %dMhz",SystemCoreClock/1000000);
  PCF8833_PutStr(buf,1,10);
  sprintf(buf,"SPI: %dMhz",(SystemCoreClock>>(((SPI1->CR1&0x38)>>3)+1))/1000000);
  PCF8833_PutStr(buf,1,19);
  PCF8833_Line(1,48,130,48,WHITE);
  PCF8833_PutStr("8-bit SPI over 9-bit",1,52);
  PCF8833_PutStr("SPI: output to LCD",1,61);
  PCF8833_PutStr("using double",1,70);
  PCF8833_PutStr("framebuffer and DMA.",1,79);
  PCF8833_PutStr("1 transfer for every",1,88);
  PCF8833_PutStr("pixel. Each transfer",1,97);
  PCF8833_PutStr("is 9 bits.",1,106);
  PCF8833_PutStr("(Flicker-free)",1,115);

  DWT_Delay(3000000);
  start_us=DWT_Get();
  display_rotating_cube_checkboard_double_framebuffer_SPI_DMA();
  // Wait for the last frame to complete
  while(!framebuffer_transfer_complete){};
  end_us=DWT_Get();
  sprintf(buf,"%d ms",(end_us-start_us)/(SystemCoreClock/1000));
  PCF8833_ClearScreen(BLACK);
  PCF8833_PutStr("200 frames in",1,51);
  PCF8833_PutStr(buf,82,51);
  PCF8833_SetFont(PCF8833_FONT_8x14);
  PCF8833_PutStr("FPS:",1,71);
  fps6=1000./((end_us-start_us)/(SystemCoreClock/(1000/200.)));
  sprintf(buf,"%d.%d",(int16_t)fps6, (((int16_t)(fps6 * 10.0)) % 10));
  PCF8833_PutStr(buf,40,71);
  DWT_Delay(3000000);

  PCF8833_ClearScreen(BLACK);
  PCF8833_SetFont(PCF8833_FONT_6x8);
  PCF8833_PutStr("Nokia 6020 LCD",1,1);
  sprintf(buf,"STM32F103: %dMhz",SystemCoreClock/1000000);
  PCF8833_PutStr(buf,1,10);
  sprintf(buf,"SPI: %dMhz",(SystemCoreClock>>(((SPI1->CR1&0x38)>>3)+1))/1000000);
  PCF8833_PutStr(buf,1,19);
  PCF8833_PutStr("Rotating 3D cube with",1,28);
  PCF8833_PutStr("checkerboard bkground",1,37);
  PCF8833_Line(1,48,130,48,WHITE);

  PCF8833_PutStr(" Method      FPS",1,56);
  PCF8833_Line(1,66,130,66,WHITE);
  PCF8833_Line(1,66,1,130,WHITE);
  PCF8833_Line(1,130,130,130,WHITE);
  PCF8833_Line(66,66,66,130,WHITE);
  PCF8833_Line(130,66,130,130,WHITE);

  PCF8833_PutStr("  1",2,71);
  sprintf(buf,"%d.%d",(int16_t)fps1, (((int16_t)(fps1 * 10.0)) % 10));
  PCF8833_PutStr(buf,78,71);

  PCF8833_PutStr("  2",2,81);
  sprintf(buf,"%d.%d",(int16_t)fps2, (((int16_t)(fps2 * 10.0)) % 10));
  PCF8833_PutStr(buf,78,81);

  PCF8833_PutStr("  3",2,91);
  sprintf(buf,"%d.%d",(int16_t)fps3, (((int16_t)(fps3 * 10.0)) % 10));
  PCF8833_PutStr(buf,78,91);

  PCF8833_PutStr("  4",2,101);
  sprintf(buf,"%d.%d",(int16_t)fps4, (((int16_t)(fps4 * 10.0)) % 10));
  PCF8833_PutStr(buf,78,101);

  PCF8833_PutStr("  5",2,111);
  sprintf(buf,"%d.%d",(int16_t)fps5, (((int16_t)(fps5 * 10.0)) % 10));
  PCF8833_PutStr(buf,78,111);

  PCF8833_PutStr("  6",2,121);
  sprintf(buf,"%d.%d",(int16_t)fps6, (((int16_t)(fps6 * 10.0)) % 10));
  PCF8833_PutStr(buf,78,121);

  while(1){};
}
