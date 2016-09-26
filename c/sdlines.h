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

struct vectlist* vectlist_new(int count);

void set_vector( float v[3], struct sdpoint * point, struct sdadapter * adapter);

void sdlines_add_vectlist(struct sdlines * lines, struct vectlist * vect);

void dump_sdlines(struct sdlines * lines, char * varname);

void dump_xlines(FILE * f, struct sdlines * lines, char * varname);

/** return an array of xlines 
start at 'start' and take count elements.
*/
struct xlines * build_xlines(struct sdlines * lines, int start, int count);

#endif
