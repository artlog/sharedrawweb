#ifndef __INCLUDE_SDLINES_HEADER__
#define __INCLUDE_SDLINES_HEADER__

#include "sdpoint.h"
#include "pointlist.h"

#include<X11/Xlib.h>

struct vectlist {
  struct vectlist * next;
  int index;
  float vector[1][3];
};

struct sdlines {
  int lines;  
  struct vectlist * first;
  struct vectlist * last;
};

struct xlines {
  int points;
  XPoint *vector;
};

struct sdlines_matrix6 {
  float a;
  float b;
  float c;
  float d;
  float e;
  float f;
};

/*

https://www.w3.org/TR/SVG/coords.html#TransformMatrixDefined

[ a c e 
  b d f
  0 0 1 ]
  
xn = a*x + c*y + e
yn = b*x + d*y + f

v[0] is x
v[1] is y
*/
void sdlines_matrix6_apply_matrix(struct sdlines_matrix6 * matrix, float * v, XPoint * dest );

struct vectlist* vectlist_new(int count);

void set_vector( float v[3], struct sdpoint * point, struct sdadapter * adapter);

void sdlines_add_vectlist(struct sdlines * lines, struct vectlist * vect);

void dump_sdlines(struct sdlines * lines, char * varname);

void dump_xlines(FILE * f, struct sdlines * lines, char * varname);

/**
convert sdlines to xlines ( XPoint vector usable for polygon drawing )
return an array of xlines 
start at 'start' and take count elements.
optional transform matrix to apply to sdlines
*/
struct xlines * build_xlines(struct sdlines * lines, int start, int count, struct sdlines_matrix6 * matrix);

/**
 *built is what was stored at build_xlines

 free built[0] and set it to NULL
 WARNING this is a pointer over the pointer to be sure to reset it to NULL.

 returns 0 of OK everything else if not freed
**/
int free_xlines(struct xlines ** built);
  
#endif
