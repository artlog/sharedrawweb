#include <stdio.h>
#include "drawlinetools.h"

#define EXPECTEDPOINTS 256

void setup_adapter(struct pointlist * this, struct sdadapter * adapter, int count)
{
  if (adapter == NULL)
    {
      fprintf(stderr,"adapter is NULL\n");
      return;
    }
  struct sdlines * lines = adapter->data;
  if ( lines == NULL)
    {
      fprintf(stderr, "data is NULL should have been created before");
      return;
    }
    {
      struct vectlist * vect = vectlist_new(count);
      sdlines_add_vectlist(lines,vect);
    }
}

void close_adapter(struct pointlist * this, struct sdadapter * adapter)
{
}

void adapt_point(struct pointlist * this, struct sdpoint * point, struct sdadapter * adapter)
{
  struct sdlines * lines = adapter->data;
  struct vectlist *vect = lines->last;
  set_vector( &vect->vector[vect->index][0], point, adapter);
  vect->index++;
}


int drawlineexpander_read_sdlines( struct drawlineexpander *this, struct inputstream * input,struct sdlines * sdlines)
{
  int      readok = 0;
  struct sdpoint min, max;
  
  struct sdadapter adapter={
    /*
    .cx=-100,
    .cy=-100,
    */
    .cx=0,
    .cy=0,
    /*
    .width=200,
    .height=-200,// negative to match standard y axis
    */
    .width=1,
    .height=1,
    .f_before=setup_adapter,
    .f_for_each=adapt_point,
    .f_after=close_adapter,
    .data=sdlines
  };

  int debug=this->debug;
  int lines = inputstream_readuint32(input);
  int expectedpoints = EXPECTEDPOINTS;
  if (lines < 10000)
    {
      for (int i=0; i< lines; i++)
	{
	  if ( debug > 0) { fprintf(stdout, "Line %u/%u\n", (i+1),lines); }
	  drawlineexpander_init(this,expectedpoints);
	  this->debug=debug;
	  drawlineexpander_expand(this, input);
	  if ( debug > 0 ) { pointlist_dump(this->expandedLines); }
	  pointlist_update_min_max(this->expandedLines,&min,&max);
	  pointlist_foreach(this->expandedLines, &adapter);
	  drawlineexpander_release(this);
	  ++ readok;
	}      
      sdpoint_dump(&max,"// max");
      sdpoint_dump(&min,"// min");
      if ( debug > 0 )
	{
	  dump_sdlines(sdlines,"expanded");
	}
    }
  else
    {
      fprintf(stderr, "Too many lines %xh", lines);
    }
  return readok;
}

