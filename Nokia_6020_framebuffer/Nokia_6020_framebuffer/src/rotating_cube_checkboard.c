/*
 *  rotating_cube_checkboard.c
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "colors_8bit.h"
#include "rotating_cube_checkboard.h"

#define abs(a)  (a>0?(a):-(a))

__align(4) uint8_t framebuffer1[132*132*9/8];  // 19602 (17424)
__align(4) uint8_t framebuffer2[132*132*9/8];  // 19602

// Vertices
//const int nbPts = 8;
const float cube[nbPts*3] = {
  -1.0, -1.0, -1.0,  // 0
   1.0, -1.0, -1.0,  // 1
   1.0,  1.0, -1.0,  // 2
  -1.0,  1.0, -1.0,  // 3
  -1.0, -1.0,  1.0,  // 4
   1.0, -1.0,  1.0,  // 5
   1.0,  1.0,  1.0,  // 6
  -1.0,  1.0,  1.0 };  // 7

// Edges
//const int nbEdges = 12;
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
  3, 7 };

// Faces
//const int nbFaces = 6;
const int faces[nbFaces*4] = {
  0, 1, 2, 3,  // Front
  4, 5, 6, 7,  // Back
  0, 1, 5, 4,  // Bottom
  3, 2, 6, 7,  // Top
  1, 5, 6, 2,  // Right
  0, 4, 7, 3};  // Left

// Faces colors

const unsigned char faces_color[nbFaces] = {
  RED,
  GREEN,
  BLUE,
  YELLOW,
  MAGENTA,
  PINK
};
