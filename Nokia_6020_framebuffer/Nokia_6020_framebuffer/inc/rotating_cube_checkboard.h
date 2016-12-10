/*
 *  rotating_cube_checkboard.h
 *
 *  Author: Kestutis Bivainis
 */

#ifndef ROTATING_CUBE_CHECKBOARD_H
#define ROTATING_CUBE_CHECKBOARD_H

#define MAX_X 132
#define MAX_Y 132
#define START_X 0
#define END_X 131
#define START_Y 0
#define END_Y 131

void display_rotating_cube_checkboard_pixel_BB(void);
void display_rotating_cube_checkboard_pixel_SPI(void);
void display_rotating_cube_checkboard_framebuffer_BB(void);
void display_rotating_cube_checkboard_framebuffer_SPI(void);
void display_rotating_cube_checkboard_framebuffer_SPI_DMA(void);
void display_rotating_cube_checkboard_double_framebuffer_SPI_DMA(void);

void drawPolygons_pixel_BB( float *tp, int *pp );
void drawPolygon_pixel_BB( unsigned char _x0, unsigned char _y0,
             unsigned char _x1, unsigned char _y1,
             unsigned char _x2, unsigned char _y2,
             unsigned char _x3, unsigned char _y3,
             unsigned char _c);

void drawPolygons_pixel_SPI( float *tp, int *pp );
void drawPolygon_pixel_SPI( unsigned char _x0, unsigned char _y0,
             unsigned char _x1, unsigned char _y1,
             unsigned char _x2, unsigned char _y2,
             unsigned char _x3, unsigned char _y3,
             unsigned char _c);

void drawFrame_framebuffer_BB(void);
void drawPolygons_framebuffer_BB( float *tp, int *pp );
void drawPolygon_framebuffer_BB( unsigned char _x0, unsigned char _y0,
             unsigned char _x1, unsigned char _y1,
             unsigned char _x2, unsigned char _y2,
             unsigned char _x3, unsigned char _y3,
             unsigned char _c);

void drawFrame_framebuffer_SPI(void);
void drawPolygons_framebuffer_SPI( float *tp, int *pp );
void drawPolygon_framebuffer_SPI( unsigned char _x0, unsigned char _y0,
             unsigned char _x1, unsigned char _y1,
             unsigned char _x2, unsigned char _y2,
             unsigned char _x3, unsigned char _y3,
             unsigned char _c);

void drawFrame_framebuffer_SPI_DMA(void);
void drawPolygons_framebuffer_SPI_DMA( float *tp, int *pp );
void drawPolygon_framebuffer_SPI_DMA( unsigned char _x0, unsigned char _y0,
             unsigned char _x1, unsigned char _y1,
             unsigned char _x2, unsigned char _y2,
             unsigned char _x3, unsigned char _y3,
             unsigned char _c);

void drawFrame_double_framebuffer_SPI_DMA(void);
void drawPolygons_double_framebuffer_SPI_DMA( float *tp, int *pp );
void drawPolygon_double_framebuffer_SPI_DMA( unsigned char _x0, unsigned char _y0,
             unsigned char _x1, unsigned char _y1,
             unsigned char _x2, unsigned char _y2,
             unsigned char _x3, unsigned char _y3,
             unsigned char _c);

#define nbPts 8
#define nbEdges 12
#define nbFaces 6

#endif
