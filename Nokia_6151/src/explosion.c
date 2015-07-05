/*
 *  explosion.c
 *
 *  Author: Kestutis Bivainis
 *
 *  Code adaptation from:
 *  The_Demo_Effects_Collection-0.7.2
 */

/* Copyright (C) 2002 W.P. van Paassen - peter@paassen.tmfweb.nl

   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*note that the code has not been fully optimized*/

#include "stm32f10x_conf.h"
#include "SPFD54124B.h"
#include "explosion.h"
#include "adc.h"
#include <limits.h>
#include <stdio.h>
#include "xorshift.h"

extern volatile uint16_t ADCConvertedValue[];

static uint16_t actual_particles;
static uint8_t fire[SCREEN_WIDTH * SCREEN_HEIGHT];
static uint32_t colors[256];
static PARTICLE particles[NUMBER_OF_PARTICLES];

uint8_t gravity=1;

void init_particle(PARTICLE* particle) {

  /* randomly init particles, generate them in the center of the screen */
  particle->xpos = (SCREEN_WIDTH >> 1) - 20 + (int)(40.0 * (xor128()/(UINT_MAX+1.0)));
  particle->ypos = (SCREEN_HEIGHT >> 1) - 20 + (int)(40.0 * (xor128()/(UINT_MAX+1.0)));
  particle->xdir = -10 + (int)(20.0 * (xor128()/(UINT_MAX+1.0)));
  particle->ydir = -17 + (int)(19.0 * (xor128()/(UINT_MAX+1.0)));
  particle->colorindex = 255;
  particle->dead = 0;
}

void init(void) {

  uint32_t i;

  /* create a suitable fire palette, this is crucial for a good effect */
  /* black to blue, blue to red, red to yellow, yellow to white*/

  for (i = 0; i < 32; ++i) {

    /* black to blue, 32 values*/
    colors[i] = i << 1;
    /* blue to red, 32 values*/
    colors[i + 32] = ((i << 3)<<16)|(64 - (i << 1));
    /*red to yellow, 32 values*/
    colors[i + 64] = (255<<16)|((i << 3)<<8);
    /* yellow to white, 162 */
    colors[i + 96] =  (255<<16)|(255<<8)|(i << 2);
    colors[i + 128] = (255<<16)|(255<<8)|(64 + (i << 2));
    colors[i + 160] = (255<<16)|(255<<8)|(128 + (i << 2));
    colors[i + 192] = (255<<16)|(255<<8)|(192 + i);
    colors[i + 224] = (255<<16)|(255<<8)|(224 + i);
  }

  for (i = 0; i < actual_particles; i++) {
    init_particle(particles + i);
  }
}

void explosion(void) {

  uint8_t dest;
  uint32_t buf, index, temp;
  uint32_t i,j;
  char buf1[32];
  uint8_t alive=0;

  SPFD54124B_SetFont(FONT_8x14);
  SPFD54124B_PutStrCentered("kbiva.wordpress.com",118);
  SPFD54124B_SetFont(FONT_6x8);

  init();

  while( 1 ) {

   if(!alive) {

     actual_particles=ADCConvertedValue[0];
     sprintf(buf1,"Particles:%d",actual_particles);
     SPFD54124B_PutStrCEOL(buf1,0,0);
     if(gravity) {
        SPFD54124B_PutStrCEOL("Gravity:ON",90,0);
     } else {
       SPFD54124B_PutStrCEOL("Gravity:OFF",90,0);
     }
     for (i = 0; i < actual_particles; i++) {
       init_particle(particles + i);
     }
   }

   alive=0;

   /* move and draw particles into fire array */

    for (i = 0; i < actual_particles; i++) {

      if (!particles[i].dead) {

        alive=1;

        particles[i].xpos += particles[i].xdir;
        particles[i].ypos += particles[i].ydir;

        /* is particle dead? */
        if ((particles[i].ypos >= SCREEN_HEIGHT - 3) || particles[i].ypos < 2 || particles[i].colorindex == 0 ||
             particles[i].xpos <= 1 || particles[i].xpos >= SCREEN_WIDTH - 3) {
          particles[i].dead = 1;
          continue;
        }

        /* gravity takes over */
        if(gravity)
          particles[i].ydir++;

        /* particle cools off */
        particles[i].colorindex--;

        /* draw particle */
        temp = particles[i].ypos * SCREEN_WIDTH + particles[i].xpos;

        fire[temp] = particles[i].colorindex;
        fire[temp - 1] = particles[i].colorindex;
        fire[temp + SCREEN_WIDTH] = particles[i].colorindex;
        fire[temp - SCREEN_WIDTH] = particles[i].colorindex;
        fire[temp + 1] = particles[i].colorindex;
      }
    }

    /* create fire effect */

    for (i = 1; i < SCREEN_HEIGHT - 2; i++) {

      index = (i  - 1) * SCREEN_WIDTH;
      for (j = 1; j < SCREEN_WIDTH - 2; j++) {

        buf = index + j;

        temp = fire[buf];
        temp += fire[buf + 1];
        temp += fire[buf - 1];
        buf += SCREEN_WIDTH;
        temp += fire[buf - 1];
        temp += fire[buf + 1];
        buf += SCREEN_WIDTH;
        temp += fire[buf];
        temp += fire[buf + 1];
        temp += fire[buf - 1];

        temp >>= 3;

        if (temp > 4) {
          alive=1;
          temp -= 4;
        } else {
          temp = 0;
        }

        fire[buf - SCREEN_WIDTH] = temp;
      }
    }

    /* draw fire array to screen */

    SPFD54124B_SetWriteWindow(0,8,SCREEN_WIDTH-1,8+SCREEN_HEIGHT-1);

    for (i = 0; i<SCREEN_HEIGHT; i++) {
      temp = i * SCREEN_WIDTH;
      for (j = 0; j<SCREEN_WIDTH; j++) {

        dest=(colors[fire[temp + j]] & 0xfc0000) >> 16;
        LCD_DAT8 = dest;
        dest=(colors[fire[temp + j]] & 0x00fc00) >> 8;
        LCD_DAT8 = dest;
        dest=colors[fire[temp + j]] & 0x0000fc;
        LCD_DAT8 = dest;
      }
    }
  }
}
