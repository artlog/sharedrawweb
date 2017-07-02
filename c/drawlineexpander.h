#ifndef _DRAWLINEEXPANDER_H_
#define _DRAWLINEEXPANDER_H_

#include "fieldreader.h"
#include "drawlinecommon.h"

struct drawlineexpander
{
  struct pointlist *expandedLines;
  int currentSize;
  struct bitfieldreader * fieldreader;
  int debug;
};


void drawlineexpander_init( struct drawlineexpander * this );

struct pointlist * drawlineexpander_expand( struct drawlineexpander *this, struct inputstream * input );

#endif
