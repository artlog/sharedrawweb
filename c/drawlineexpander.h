#include "sdpoint.h"
#include "pointlist.h"
#include "fieldreader.h"

#define SCODE_MAX 4

struct drawlineexpander
{
  struct pointlist *expandedLines;
  int currentSize;
  // code for size encoding
  int scode[5];
  struct bitfieldreader * fieldreader;
  int debug;
};


void drawlineexpander_init( struct drawlineexpander * this );

struct pointlist * drawlineexpander_expand( struct drawlineexpander *this, struct inputstream * input );
