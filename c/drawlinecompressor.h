#ifndef _DRAWLINECOMPRESSOR_H_
#define _DRAWLINECOMPRESSOR_H_

#include "bitfieldwriter.h"
#include "drawlinecommon.h"

struct drawlinecompressor
{
  struct pointlist *fromLines;
  int previousSize;
  struct bitfieldwriter * fieldWriter;
  int debug;
  struct sdpoint * frompoint;
};

void drawlinecompressor_init(struct drawlinecompressor * this, struct pointlist * fromlines);

void drawlinecompressor_compress( struct drawlinecompressor * this, struct outputstream * output );
			       
#endif
