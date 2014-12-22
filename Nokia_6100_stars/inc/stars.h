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


#define STARS_SPI             SPI1
#define STARS_SPI_DMA_CHANNEL DMA1_Channel3
#define STARS_SPI_DMA_TC      DMA1_FLAG_TC3
#define STARS_SPI_DMA_TCIF    DMA_ISR_TCIF3

/*
#define STARS_SPI             SPI2
#define STARS_SPI_DMA_CHANNEL DMA1_Channel5
#define STARS_SPI_DMA_TC      DMA1_FLAG_TC5
#define STARS_SPI_DMA_TCIF    DMA_ISR_TCIF5
*/

#define TO_8BIT_COLOR(c) (((c&0xE000)>>8)|((c&0x0700)>>6)|((c&0x018)>>3))

#define MAX_X 128
#define MAX_Y 128
#define MAXSTARS 4096

void stars_DMA_Init(void);
void stars_DMA_ClearBuf(void);
void stars_DMA_SpiSend(void);

void display_stars(void);

#endif
