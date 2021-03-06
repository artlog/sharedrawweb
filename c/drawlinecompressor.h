#ifndef _DRAWLINECOMPRESSOR_H_
#define _DRAWLINECOMPRESSOR_H_

#include "albitfieldwriter.h"
#include "drawlinecommon.h"
#include "sdlines.h"

struct drawlinecompressor
{
  struct pointlist *fromLines;
  int previousSize;
  struct bitfieldwriter fieldWriter;
  int debug;
  struct sdpoint * frompoint;
};

void drawlinecompressor_init(struct drawlinecompressor * this, struct pointlist * fromlines);

void drawlinecompressor_compress( struct drawlinecompressor * this, struct aloutputstream * output );

int drawlinecompressor_write_imc(struct aloutputstream * output,struct sdlines * sdlines,int debug);

#endif
