/*
 *  3d.c
 *
 *  Author: Kestutis Bivainis
 *
 *  adapted code from:
 *  https://github.com/majestik666/Beagle_GPIO/blob/master/Beagle_GPIO_Nokia6100_test_3d.cc
 */

#include <stdint.h>
#include "colors.h"
#include "3D.h"

#define abs(a)  (a>0?(a):-(a))

uint32_t frame[MAX_X_3D*MAX_Y_3D];

// Vertices
const int nbPts = 8;
const float cube[nbPts*3] = {
  -1.0, -1.0, -1.0,  // 0
   1.0, -1.0, -1.0,  // 1
   1.0,  1.0, -1.0,  // 2
  -1.0,  1.0, -1.0,  // 3
  -1.0, -1.0,  1.0,  // 4
   1.0, -1.0,  1.0,  // 5
   1.0,  1.0,  1.0,  // 6
  -1.0,  1.0,  1.0,  // 7
};

// Edges
const int nbEdges = 12;
const int edges[nbEdges*2] = {
  0, 1,
  1, 2,
  2, 3,
  3, 0,
  4, 5,
  5, 6,
  6, 7,
  7, 4,
  0, 4,
  1, 5,
  2, 6,
  3, 7,
};

// Faces
const int nbFaces = 6;
const int faces[nbFaces*4] = {
  0, 1, 2, 3,  // Front
  4, 5, 6, 7,  // Back
  0, 1, 5, 4,  // Bottom
  3, 2, 6, 7,  // Top
  1, 5, 6, 2,  // Right
  0, 4, 7, 3,  // Left
};


// Faces colors
const uint32_t faces_color[nbFaces] = {
  BLUE,
  GREEN,
  RED,
  YELLOW,
  MAGENTA,
  CYAN
};

void rasterLine( int x0, int y0, int x1, int y1, int *rl ) {

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
  for(;;) {
    if (y0>=0 && y0<MAX_X_3D) {
      if (x0 < rl[2*y0+0])
        rl[2*y0+0] = x0 >= 0 ? x0 : 0;
      if (x0 > rl[2*y0+1])
        rl[2*y0+1] = x0 < MAX_X_3D ? x0 : MAX_X_3D-1;
    }

    // Did we reach the end ?
    if (x0==x1 && y0==y1)
      return;

    err2 = 2*err;

    if (err2 > -dy) {
      err -= dy;
      x0 += sx;
    }

    if (err2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void drawPolygon( uint8_t _x0, uint8_t _y0,
                  uint8_t _x1, uint8_t _y1,
                  uint8_t _x2, uint8_t _y2,
                  uint8_t _x3, uint8_t _y3,
                  uint32_t _c) {

  int rl[MAX_X_3D*2];
  int j,i;

  for (i=0;i<MAX_X_3D;++i) {
    rl[2*i+0] = MAX_Y_3D+1;
    rl[2*i+1] = -1;
  }

  rasterLine( _x0, _y0, _x1, _y1, rl );
  rasterLine( _x1, _y1, _x2, _y2, rl );
  rasterLine( _x2, _y2, _x3, _y3, rl );
  rasterLine( _x3, _y3, _x0, _y0, rl );

  for (i=0;i<MAX_X_3D;++i) {
    if (rl[2*i+1]>rl[2*i+0]) {
      for( j=rl[2*i+0]; j<=rl[2*i+1]; ++j ) {
        frame[j*MAX_Y_3D+i]=_c;
      }
    }
  }
}

void drawPolygons( float *tp, int *pp) {

  float d[nbFaces];
  int i,j,p;
  int sp[nbFaces] = { 0,1,2,3,4,5 };
  float t;
  int tt;
  // First calculate mid z position of faces
  for (i=0;i<nbFaces;++i) {
    d[i] = 0.0;
    for (j=0;j<4;++j)
      d[i] += tp[3*faces[4*i+j]+2];
  }

  // Sort the faces
  for (i=0;i<nbFaces-1;++i) {
    for (j=i;j<nbFaces;++j) {
      if (d[i]<d[j]) {
        t = d[j]; d[j] = d[i]; d[i] = t;
        tt = sp[j]; sp[j] = sp[i]; sp[i] = tt;
      }
    }
  }

  // Draw back to front
  for (p=0;p<nbFaces;++p) {
    // Sorted Face Index
    int pi = sp[p];
    int p0 = faces[4*pi+0];
    int p1 = faces[4*pi+1];
    int p2 = faces[4*pi+2];
    int p3 = faces[4*pi+3];

    drawPolygon( pp[2*p0+0], pp[2*p0+1],
          pp[2*p1+0], pp[2*p1+1],
          pp[2*p2+0], pp[2*p2+1],
          pp[2*p3+0], pp[2*p3+1],
          faces_color[pi]);
  }
}
