#ifndef _DRAWLINEEXPANDER_H_
#define _DRAWLINEEXPANDER_H_

#include "albitfieldreader.h"
#include "drawlinecommon.h"

struct drawlineexpander
{
  struct pointlist *expandedLines;
  int currentSize;
  struct bitfieldreader * fieldreader;
  int debug;
};


/** initialsize is number of points expected , will grow as needed 
don't forget to release it after user
*/
void drawlineexpander_init( struct drawlineexpander * this, int initialsize );

void drawlineexpander_release( struct drawlineexpander * this);

struct pointlist * drawlineexpander_expand( struct drawlineexpander *this, struct alinputstream * input );

#endif
