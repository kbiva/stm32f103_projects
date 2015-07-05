/*
 *  explosion.h
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

#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <stdint.h>

#define NUMBER_OF_PARTICLES 4095
#define SCREEN_WIDTH 162
#define SCREEN_HEIGHT 110

/*particle structure*/
typedef struct
{
  uint8_t xpos,ypos,xdir,ydir;
  uint8_t colorindex;
  uint8_t dead;
} PARTICLE;

void explosion(void);

#endif
