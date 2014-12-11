/*
 *  main.c
 *
 *  Moving stars demo.
 *  Nokia 6100/6020 LCD connected to HY-STM32 board.
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "PCF8833.h"
#include "colors_16bit.h"
#include "hy_stm32.h"
#include "stars.h"
#include "adc.h"

int main(void) {

  DWT_Init();
  DWT_Delay(200000);

  BSP_Button_Init(BUTTON3,BUTTON_MODE_EXTI);
  BSP_Button_Init(BUTTON4,BUTTON_MODE_EXTI);

  // Initialize display
  PCF8833_Init(ACCESS_SPI9BITS);
#ifdef STARS_COLOR_8BIT
  PCF8833_ColorMode(COLOR_8BIT);
  PCF8833_SetupColor(RGB8ColorMap_Nokia6020,48);
#elif  defined STARS_COLOR_16BIT
  PCF8833_ColorMode(COLOR_16BIT);
#endif
  PCF8833_SetOrientation(ORIENTATION_PORTRAIT1,1);
  PCF8833_SetContrast(NOKIA6020_SETCON_DATA);
  PCF8833_ClearScreen(BLACK);

  ADC_DMA_Configuration();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  display_stars();

  while(1){};
}
