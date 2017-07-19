#ifndef __INCLUDE_SDLINES_HEADER__
#define __INCLUDE_SDLINES_HEADER__

// for FILE
#include <stdio.h>

#include "sdpoint.h"
#include "pointlist.h"

#include<X11/Xlib.h>

enum {
  SDLINE_DYNAMIC=1,
  SDLINE_CONCRETE=2,
  // when adding lines over a CONCRETE OBJECT...
  SDLINE_MIXED=4,
  // when freed or in bad shape
  SDLINE_ROTTEN=8,
  // there is space allocated from more data
  SDLINE_GROWABLE=16
};

#define FLAG_IS_SET(x,flag) (x & flag) != 0

struct vectbuffer {
  int allocated;
    // wil be allocated dynamically for a maximum count number of elements
  float vector[1][3];  
};

// linked list of lines dynamically allocated
// one vectlist represent a sequence of three dimensional points.
struct vectlist {
  // ored combination of SDLINES_xx 
  int flags;
  // NULL for last
  struct vectlist * next;
  // a pointer within vectbuffer buffer of points.
  float * vectorp ;
  //! allocated number of points ( == index if not growable )
  int count;
  //! current index withing dynamically allocated vector
  int index;
  //! wil be allocated dynamically for a maximum count number of elements if SDLINE_CONCRETE not set
  // DON'T USE PLEASE
  float vector[1][3];
};

// content will be allocated dynamically
struct sdlines {
  int flags;
  //! number of lines
  int lines;
  //! number of points (SDLINES_CONCRETE set)
  int points;
  // ! when using a pool from whole sdlines
  struct vectbuffer * buffer;
  //! first line
  struct vectlist * first;
  //! last line
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

// count is maximal number of points, it should be remembered by caller...
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

/** get ith veclist of sdlines  */
struct vectlist * sdlines_get_vectlist(struct sdlines * this, int i);

/** skips i elements from this vectlist */
struct vectlist * veclist_get_next(struct vectlist * this, int i);

/**
convert a vectlist to a newly allocated pointlist
**/
struct pointlist * vectlist_to_pointlist(struct vectlist * this);

struct sdlines_iterator_callback {
  // run once before looping on each element (header) can be NULL
  void (*f_before) (struct sdlines * this, struct sdlines_iterator_callback * callback, void * data);
  // run for each element (content) SHOULD be set (ie non NULL) index is line index
  void (*f_for_each) (struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data);
  // run after for each (footer/collector) can be NULL
  void (*f_after) (struct sdlines * this, struct sdlines_iterator_callback * callback, void * data);
};

void sdlines_foreach(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data);

// call foreach twice, one with callback_collect then with callback_complete
void dump_generic_callbacks(struct sdlines * lines, struct sdlines_iterator_callback * callback_collect, struct sdlines_iterator_callback * callback_complete, void * data);

// get float * index on x
float * vectlist_get(  struct vectlist * vectlist , int index);

struct sdlines * sdlines_concrete( struct sdlines * this);

void sdlines_free( struct sdlines * this);

// returns index of point added ( 0 if not added ).
int vectlist_add_point(struct vectlist * vect, struct sdpoint * point);

struct sdlines * sdlines_create_growable(int count);

struct xlines * xlines_clone(struct xlines * source, int points, int offset);

#endif
