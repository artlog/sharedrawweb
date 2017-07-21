#ifndef _DRAWLINECOMPRESSOR_H_
#define _DRAWLINECOMPRESSOR_H_

#include "bitfieldwriter.h"
#include "drawlinecommon.h"
#include "sdlines.h"

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

int drawlinecompressor_write_imc(struct outputstream * output,struct sdlines * sdlines,int debug);

#endif
