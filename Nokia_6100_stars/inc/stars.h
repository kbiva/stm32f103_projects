/*
 *  stars.h
 *
 *  Moving stars demo.
 *  Nokia 6100/6020 LCD connected to HY-STM32 board.
 *
 *  Author: Kestutis Bivainis
 */

#ifndef STARS_H
#define STARS_H

#include <stdint.h>

typedef struct _TStar {
  uint8_t x, y;
  uint8_t plane;
} TStar;

#define STARS_COLOR_8BIT
//#define STARS_COLOR_16BIT

#define TO_8BIT_COLOR(c) (((c&0xE000)>>8)|((c&0x0700)>>6)|((c&0x018)>>3))

#define MAX_X 128
#define MAX_Y 128
#define MAXSTARS 4096

void stars_DMA_Init(void);
void stars_DMA_ClearBuf(void);
void stars_DMA_SpiSend(void);

void display_stars(void);

#endif
