/*
 *  bouncingballs.c
 *
 *  Author: Kestutis Bivainis
 *
 *  original code from: http://chipkit.net/forum/viewtopic.php?t=420
 */

#include "stm32f10x_conf.h"
#include "R63400.h"
#include "colors.h"
#include "delay.h"
#include "hy_stm32.h"
#include "xorshift.h"
#include "bouncingballs.h"
#include <math.h>

extern volatile uint16_t ADCConvertedValue[];

uint16_t display_width,display_height;

uint32_t balls_cnt;

extern uint8_t fill,exit_demo;

float x[MAX_BALLS];
float y[MAX_BALLS];
uint8_t r[MAX_BALLS];
uint32_t c[MAX_BALLS];

float oldX[MAX_BALLS];
float oldY[MAX_BALLS];

float velX[MAX_BALLS];
float velY[MAX_BALLS];

void R63400_DrawCircle(int x0, int y0, int radius, uint32_t color) {

  int x = radius;
  int y = 0;
  int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

  while( y <= x )
  {
    R63400_SetPixel( x + x0,  y + y0,color); // Octant 1
    R63400_SetPixel( y + x0,  x + y0,color); // Octant 2
    R63400_SetPixel(-x + x0,  y + y0,color); // Octant 4
    R63400_SetPixel(-y + x0,  x + y0,color); // Octant 3
    R63400_SetPixel(-x + x0, -y + y0,color); // Octant 5
    R63400_SetPixel(-y + x0, -x + y0,color); // Octant 6
    R63400_SetPixel( x + x0, -y + y0,color); // Octant 7
    R63400_SetPixel( y + x0, -x + y0,color); // Octant 8
    y++;
    if (decisionOver2<=0)
    {
      decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
    }
    else
    {
      x--;
      decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
    }
   }
}

void plot4points(int cx, int cy, int x, int y,uint32_t color) {

  R63400_Fill(cx - x, cy + y, cx + x,cy + y,color);

  if (x != 0 && y != 0)
    R63400_Fill(cx - x, cy - y, cx + x,cy - y,color);
}

void R63400_DrawFilledCircle(int x0, int y0, int radius, uint32_t color) {

  int error = -radius;
  int x = radius;
  int y = 0;

  while (x >= y) {

    int lastY = y;

    error += y;
    ++y;
    error += y;

    plot4points(x0, y0, x, lastY,color);

    if (error >= 0) {

      if (x != lastY)
        plot4points(x0, y0, lastY, x,color);

      error -= x;
      --x;
      error -= x;
    }
  }
}

static void setup(void) {

  uint8_t i;

  display_width=R63400_GetWidth();
  display_height=R63400_GetHeight();

  for(i = 0; i < MAX_BALLS; i++) {
    x[i] = display_width / 2;
    y[i] = display_height / 2;
    r[i] = xor128()%10+10;
    c[i] = xor128()%0xFFFFFF;

    oldX[i] = x[i];
    oldY[i] = y[i];

    velX[i] = 2;
    velY[i] = 2;
  }
}

static void collision(uint8_t i,uint8_t j) {

  float disX = x[j] - x[i];
  float disY = y[j] - y[i];
  float d2 = disX * disX + disY * disY;

  if(d2 != 0) {

    float rij = r[i] + r[j];
    float rij2 = rij * rij;

    if(d2 < rij2) {

      float ii = (disX * velX[i] + disY * velY[i]) / d2;
      float ji = (disX * velY[i] - disY * velX[i]) / d2;
      float ij = (disX * velX[j] + disY * velY[j]) / d2;
      float jj = (disX * velY[j] - disY * velX[j]) / d2;
      float ratio = rij / sqrtf(d2);

      velX[i] = ij * disX - ii * disY;
      velY[i] = ij * disY + ii * disX;
      velX[j] = ji * disX - jj * disY;
      velY[j] = ji * disY + jj * disX;

      disX *= (ratio - 1) / 2;
      disY *= (ratio - 1) / 2;

      x[j] += disX;
      y[j] += disY;
      x[i] -= disX;
      y[i] -= disY;
    }
  }
}

static void borders(uint8_t i) {

  if(x[i] >= display_width - r[i] - 1) {

    x[i] = display_width - r[i] - 1;
    velX[i] = -velX[i];
  } else if(x[i] <= r[i]) {

    x[i] = r[i];
    velX[i] = -velX[i];
  }

  if(y[i] >= display_height - r[i] - 1) {

    y[i] = display_height - r[i] - 1;
    velY[i] = -velY[i];
  } else if(y[i] <= r[i]) {

    y[i] = r[i];
    velY[i] = -velY[i];
  }
}

static uint8_t clear;

static void draw(uint8_t i) {

  if(fill) {
    R63400_DrawFilledCircle(x[i], y[i], r[i],c[i]);
    clear=1;
  }
  else {
    if(clear) {
      R63400_ClearScreen(BLACK);
      clear=0;
    }
    R63400_DrawCircle(oldX[i], oldY[i], r[i],BLACK);
    R63400_DrawCircle(x[i], y[i], r[i],c[i]);
  }


  oldX[i] = x[i];
  oldY[i] = y[i];
}

void bouncing_balls(void) {

  uint8_t i,j;

  setup();
  while(1) {

    if(exit_demo) return;

    balls_cnt=ADCConvertedValue[0]/100;

    for(i = 0; i < balls_cnt; i++) {

      x[i] += velX[i];
      y[i] += velY[i];

      for(j = i + 1; j < balls_cnt; j++)
        collision(i,j);

      borders(i);

      if((int)(x[i]) != (int)(oldX[i]) || (int)(y[i]) != (int)(oldY[i]))
        draw(i);
    }
  }
}


