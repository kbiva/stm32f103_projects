/*
 *  access_pixel_SPI.c
 *
 *  library for accessing Nokia 6020 LCD
 *  using pixel and SPI
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include <math.h>
#include "PCF8833.h"
#include "colors_8bit.h"
#include "rotating_cube_checkboard.h"

#define abs(a)  (a>0?(a):-(a))

// Vertices
extern const float cube[];  // 7

// Edges
extern const int edges[];

// Faces
extern const int faces[];

// Faces colors
extern const unsigned char faces_color[nbFaces];

void rasterLine_pixel_SPI( int x0, int y0, int x1, int y1, int *rl )
{
  // Calculate deltas
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int err = dx-dy;
  int err2;

  // Calculate directions
  int sx = 1;
  int sy = 1;
  if (x0>x1) sx=-1;
  if (y0>y1) sy=-1;

  // Loop on the segment
  for(;;)
  {
    if (y0>=0 && y0<MAX_X)
    {
      if (x0 < rl[2*y0+0])
        rl[2*y0+0] = x0 >= 0 ? x0 : 0;
      if (x0 > rl[2*y0+1])
        rl[2*y0+1] = x0 < MAX_X ? x0 : MAX_X-1;
    }

    // Did we reach the end ?
    if (x0==x1 && y0==y1)
      return;

    err2 = 2*err;
    if (err2 > -dy)
    {
      err -= dy;
      x0 += sx;
    }
    if (err2 < dx)
    {
      err += dx;
      y0 += sy;
    }
  }
}

void drawPolygon_pixel_SPI( unsigned char _x0, unsigned char _y0,
             unsigned char _x1, unsigned char _y1,
             unsigned char _x2, unsigned char _y2,
             unsigned char _x3, unsigned char _y3,
             unsigned char _c)
{
  int rl[MAX_X*2];
  int j,i;

  for (i=0;i<MAX_X;++i)
  {
    rl[2*i+0] = MAX_Y+1;
    rl[2*i+1] = -1;
  }

  rasterLine_pixel_SPI( _x0, _y0, _x1, _y1, rl );
  rasterLine_pixel_SPI( _x1, _y1, _x2, _y2, rl );
  rasterLine_pixel_SPI( _x2, _y2, _x3, _y3, rl );
  rasterLine_pixel_SPI( _x3, _y3, _x0, _y0, rl );

  for (i=0;i<MAX_X;++i)
  {
    if ( rl[2*i+1] > rl[2*i+0] )
    {
      for( j=rl[2*i+0]; j<=rl[2*i+1]; ++j ) {
        PCF8833_SPI9bits(PCF8833_PASET);
        PCF8833_SPI9bits(j|0x100);
        PCF8833_SPI9bits(j|0x100);
        PCF8833_SPI9bits(PCF8833_CASET);
        PCF8833_SPI9bits(i|0x100);
        PCF8833_SPI9bits(i|0x100);
        PCF8833_SPI9bits(PCF8833_RAMWR);
        PCF8833_SPI9bits(_c|0x100);
      }
    }
  }
}

void drawPolygons_pixel_SPI( float *tp, int *pp )
{
  float d[nbFaces];
  int i,j,p;
  int sp[nbFaces] = { 0,1,2,3,4,5 };
  float t;
  int tt;
  // First calculate mid z position of faces
  for (i=0;i<nbFaces;++i)
  {
    d[i] = 0.0;
    for (j=0;j<4;++j)
      d[i] += tp[3*faces[4*i+j]+2];
  }

  // Sort the faces
  for (i=0;i<nbFaces-1;++i)
  {
    for (j=i;j<nbFaces;++j)
    {
      if (d[i]<d[j])
      {
        t = d[j]; d[j] = d[i]; d[i] = t;
        tt = sp[j]; sp[j] = sp[i]; sp[i] = tt;
      }
    }
  }

  // Draw back to front
  for (p=0;p<nbFaces;++p)
  {
    // Sorted Face Index
    int pi = sp[p];
    int p0 = faces[4*pi+0];
    int p1 = faces[4*pi+1];
    int p2 = faces[4*pi+2];
    int p3 = faces[4*pi+3];

    drawPolygon_pixel_SPI( pp[2*p0+0], pp[2*p0+1],
          pp[2*p1+0], pp[2*p1+1],
          pp[2*p2+0], pp[2*p2+1],
          pp[2*p3+0], pp[2*p3+1],
          faces_color[pi]);
  }
}

void display_rotating_cube_checkboard_pixel_SPI(void) {
  //////////////////////////////////////////////////////////////////////////
  // 3D CUBE
  //////////////////////////////////////////////////////////////////////////

  float tp[24];

  int pp[16];

  //float camx = 0.0;
  //float camy = 0.0;
  float camz = 2.0;

  int x,y,xx,yy,j,i=0;

  float cosT;
  float sinT;
  float cosP;
  float sinP;
  float cosTcosP;
  float cosTsinP;
  float sinTcosP;
  float sinTsinP;

  int scaleFactor = MAX_Y/4;
  float near = 3;
  float nearToObj;
  float x0,y0,z0;
  float fac;
  int xo;
  int yo;
  float rotation[3] = { 0.0, 0.0, 0.0 };
  float rot_speed[3] = { 0.04, -0.08, 0.033 };

  for(i=0;i<200;i++)
  {
    // Checkerboard background
    xo = 25.0*sin(10*i/100.0)*cos(6*i/100.0);
    yo = 25.0*sin(5*i/100.0)*cos(7.5*i/100.0);

    for (x=0;x<MAX_Y;x++) {
      xx = (50+x+xo)% 32;
      for (y=0;y<MAX_X;++y) {
        yy = (50+y+yo)% 32;

        PCF8833_SPI9bits(PCF8833_PASET);
        PCF8833_SPI9bits(x|0x100);
        PCF8833_SPI9bits(x|0x100);
        PCF8833_SPI9bits(PCF8833_CASET);
        PCF8833_SPI9bits(y|0x100);
        PCF8833_SPI9bits(y|0x100);
        PCF8833_SPI9bits(PCF8833_RAMWR);
        if((xx<16 && yy<16) || (xx>16 && yy>16)) {
          PCF8833_SPI9bits(WHITE|0x100);
        } else {
          PCF8833_SPI9bits(BLACK|0x100);
        }
      }
    }

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

      pp[2*j+0] = (int)(MAX_Y/2.0f + fac*tp[3*j+0] + 0.5 );
      pp[2*j+1] = (int)(MAX_Y/2.0f + fac*tp[3*j+1] + 0.5 );
    }

    drawPolygons_pixel_SPI( tp, pp );

    for (j=0;j<3;++j)
      rotation[j] += rot_speed[j];
  }
}
