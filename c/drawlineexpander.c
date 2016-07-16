//correspond to public class DrawLineExpander c translation
#include <string.h>
#include <stdio.h>

#include "drawlineexpander.h"

#define FALSE 0
#define TRUE 1

struct drawlineexpander initdrawline = {
  .currentSize=0,
  .scode={3,6,14,30,64}
};

void drawlineexpander_init( struct drawlineexpander * this )
{
  memcpy(this,&initdrawline, sizeof(*this));
  this->expandedLines = new_pointlist();
  pointlist_init(this->expandedLines);
  this->fieldreader = new_fieldreader();
  fieldreader_init(this->fieldreader);
  this->debug=0;
}


struct sdpoint * drawlineexpander_readAbs(struct drawlineexpander *this, int size);

struct sdpoint * drawlineexpander_readRel(struct drawlineexpander *this);
  
/* create a Vector of points */
struct pointlist * drawlineexpander_expand( struct drawlineexpander *this, struct inputstream * input )
{
  struct pointlist * expandedLines = this->expandedLines;

  if (expandedLines == NULL)
    {
      if (this->debug)
	{
	  fprintf(stderr,"NULL expandedLines for %p\n",this);
	}
      return NULL;
    }
     
  fieldreader_setinput( this->fieldreader, input );
  // number of points for this line
  int point_count = fieldreader_read( this->fieldreader, 32);

  if (input->eof)
    {
      if (this->debug)
	{
	  fprintf(stderr,"EOF reached (%u)\n",point_count);
	}
      return NULL;
    }

  if (this->debug)
    {
      fprintf(stderr,"new line, point count=%u\n",point_count);
    }
  if ( point_count > 10000 )
    {
      return NULL;
    }
  // first point of line, absolute
  pointlist_add( expandedLines, drawlineexpander_readAbs( this,  64));

  for ( int i = 1; i < point_count; i ++ )
    {
    struct sdpoint * point = drawlineexpander_readRel( this);
    if (input->eof)
      {
	if (this->debug)
	  {
	    fprintf(stderr,"EOF read point count=%i\n",i);
	  }
	break;
      }
    if (point == NULL)
      {
	if (this->debug)
	  {
	    fprintf(stderr,"NULL point count=%i\n",i);
	  }
	break;
      }
    if ( this->currentSize == 64 ) {
      // absolute      
      pointlist_add( expandedLines, point);
      if (this->debug)
      {
	fprintf(stderr,"add absolute point size %u index %i x:%i y:%i \n",this->currentSize, i, point->x, point->y);
      }
    }
    else {
      // relative
      struct sdpoint * previous = pointlist_getlast( expandedLines);
      if (this->debug)
      {
	fprintf(stderr,"add relative point size %u index %i dx:%i dy:%i \n",this->currentSize, i, point->x, point->y);
      }
      sdpoint_translate( point,
			 previous->x,
			 previous->y);
      pointlist_add( expandedLines, point);
    }
    }
  return expandedLines;
}

/*
  read point with size encoding depending on first bits
*/
struct sdpoint * drawlineexpander_readRel(struct drawlineexpander *this)
{
  int index;
  
  /* 11 : next, 10 : previous ; 0* : same
     6 -> 14->30
     ^        |
     |        v
     3   <-  64
  */
  // find index of current size
  for ( index = 0; index < SCODE_MAX; index ++)
  {
    if ( this->scode[index] == this->currentSize ) break;
  }

  if (this->debug)
    {
      fprintf(stderr,"index %u size %u \n",index, this->scode[index]);
    }

  // read size modifier until it is good one
  while ( fieldreader_read( this->fieldreader,1) != 0 )
    {
      index = index + ( ( fieldreader_read( this->fieldreader, 1) == 0 ) ? -1 : 1);
      // handle circularity
      if ( index > SCODE_MAX )
	{
	  index = 0;
	}
      if ( index < 0 )
	{
	  index = SCODE_MAX;
	}
    }
  return drawlineexpander_readAbs( this,  this->scode[index]);
}

/* write given struct sdpoint * with size */
struct sdpoint * drawlineexpander_readAbs(struct drawlineexpander *this, int size)
{
  int codeval;
  int center, max;
  
  this->currentSize = size;
  
  struct sdpoint * point = new_sdpoint();
  if ( size > 32 ) {
    sdpoint_set( point,
		 fieldreader_read( this->fieldreader, 32),
		 fieldreader_read( this->fieldreader, 32));
  }
  else {
    int nocenter=FALSE;
    codeval = fieldreader_read( this->fieldreader,  size); 
    // for coding/decoding scheme see DrawLineKompressor.java 
    if ( size == 3 ) {
      max = 3;
      center = 1;
      nocenter=TRUE;
      }
    else {
      max = ( 1 << (  size  / 2  ) );
      center = (max / 2) - 1;
    }
    // central hole impossible but we don't really care
    if ( nocenter )
      {
	if ( codeval > ( center * max  ) ) {
	  codeval ++;
	}
	if ( codeval >= ( max * max  )) {
	  fprintf(stderr,"!!! reserved value for decompression should not occur !!!");
	  }
      }
    sdpoint_set( point,
		 ( codeval  %  max) - center,
		 ( codeval   / max) - center
		 );
  }
  return point;
}


