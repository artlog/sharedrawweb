//correspond to public class DrawLineExpander c translation
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "drawlineexpander.h"

#define FALSE 0
#define TRUE 1

void drawlineexpander_init( struct drawlineexpander * this, int initialsize )
{
  memset(this,0,sizeof(*this));
  this->expandedLines = new_pointlist();
  pointlist_init(this->expandedLines,initialsize);
  this->fieldreader = new_fieldreader();
  fieldreader_init(this->fieldreader);
  this->debug=0;
}

void drawlineexpander_release( struct drawlineexpander * this)
{
  if ( this->expandedLines != NULL )
    {
      pointlist_release(this->expandedLines);
      free(this->expandedLines);
      this->expandedLines = NULL;
    }
}
struct sdpoint * drawlineexpander_readAbs(struct drawlineexpander *this, int size);

struct sdpoint * drawlineexpander_readRel(struct drawlineexpander *this);
  
/* create a Vector of points */
struct pointlist * drawlineexpander_expand( struct drawlineexpander *this, struct alinputstream * input )
{
  struct pointlist * expandedLines = this->expandedLines;
  struct sdpoint * point;
  
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
  point=drawlineexpander_readAbs( this,  64);
  pointlist_add( expandedLines,  point);
  if (this->debug)
    {
      fprintf(stderr,"add initial absolute point size %u x:%i y:%i \n",this->currentSize, point->x, point->y);
    }
  for ( int i = 1; i < point_count; i ++ )
    {
    point = drawlineexpander_readRel( this);
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
    if ( global_scode[index] == this->currentSize ) break;
  }

  if (this->debug)
    {
      fprintf(stderr,"index %u size %u \n",index, global_scode[index]);
    }

  // read size modifier until it is good one
  while ( fieldreader_read( this->fieldreader,1) != SCODE_SAME )
    {
      index = index + ( ( fieldreader_read( this->fieldreader, 1) == ( SCODE_PREVIOUS & 0x01 ) ) ? -1 : 1);
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
  return drawlineexpander_readAbs( this,  global_scode[index]);
}

/* write given struct sdpoint * with size */
struct sdpoint * drawlineexpander_readAbs(struct drawlineexpander *this, int size)
{
  int codeval;
  int center, max;
  
  this->currentSize = size;
  
  struct sdpoint * point = new_sdpoint();
  sdpoint_set(point,0,0);
  if ( size > 32 ) {
    int x = fieldreader_read( this->fieldreader, 32);
    int y = fieldreader_read( this->fieldreader, 32);
    sdpoint_set( point,x,y);
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


