// DrawLineKompressor to be fully converted to c

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "drawlinecompressor.h"

#define FALSE 0
#define TRUE 1

void drawlinecompressor_init( struct drawlinecompressor * this, struct pointlist * fromlines)
{
  memset(this,0, sizeof(*this));
  this->fromLines=fromlines;
  this->fieldWriter = new_bitfieldwriter();
  bitfieldwriter_init(this->fieldWriter);
  this->debug=0;
}

void drawlinecompressor_writeAbs( struct drawlinecompressor * this, struct sdpoint * point, int size);

void drawlinecompressor_writeRel(struct drawlinecompressor * this, struct sdpoint * point, int size);

void drawlincecompressor_before_pointlist(struct pointlist * pointlist, struct sdadapter * adapter, int count)
{
  struct drawlinecompressor * this = (struct drawlinecompressor *) adapter->data;
  bitfieldwriter_write( this->fieldWriter, count, 32);
}

void drawlincecompressor_after_pointlist(struct pointlist * pointlist, struct sdadapter * adapter)
{
  struct drawlinecompressor * this = (struct drawlinecompressor *) adapter->data;
  bitfieldwriter_padtoword( this->fieldWriter);
}

void drawlincecompressor_foreach_pointlist(struct pointlist * pointlist, struct sdpoint * point, struct sdadapter * adapter)
{
  struct drawlinecompressor * this = (struct drawlinecompressor *) adapter->data;
  int center, size;
  
  if ( adapter->count == 0)
    {
      drawlinecompressor_writeAbs(this, point, 64);
    }
  else
    {
        struct sdpoint topoint;
	
        topoint.x = point->x - this->frompoint->x;
        topoint.y = point->y - this->frompoint->y;
        // find which size to use...
	if (( topoint.x == 0 ) && ( topoint.y == 0 ))
	    {
		// ARGH THIS IS INVALID !
	      printf("%s\n","[ERROR] (0,0) point is invalid for compression !!!");
	      return;
	    }
        if ( (abs( topoint.x) < 2) && (abs(topoint.y) < 2))
	  {
	    drawlinecompressor_writeRel(this, &topoint, 3);
	  }
        else
	  {
          size = 64;
          for ( int index = 1; index < ( SCODE_MAX - 1 ); index ++)
	    {
	      // center is biased there are more + than - , ex for 6bits :(x,y) within ( -3,4 ) x (-3,4)
	      center = 1 << ( (  global_scode[index]  / 2 )  - 1 );
	      if ( ( topoint.x <= center  ) && ( topoint.x > -center )
		   && ( topoint.y <= center  ) && ( topoint.y > -center ) )
		  {
		      size = global_scode[index];
		      // always take the smaller size.
		      break; 
		  }
	    }
          if ( size < 32 )
	    {
	      drawlinecompressor_writeRel(this, &topoint, size);
	    }
          else
	    {
	      // warning absolute
	      drawlinecompressor_writeRel(this, point, 64);
	    }
	  }
    }
  // new frompoint is previous point
  /* alternative
  adapter->cx=point->x;
  adapter->cy=point->y;
  */
  this->frompoint = point;
  adapter->count ++;
}

void drawlinecompressor_compress( struct drawlinecompressor * this, struct outputstream * output )
{
  bitfieldwriter_setoutputstream( this->fieldWriter, output);

  /*
  int skipsame=0;
    
    if ( ( this->fromLines != NULL ) && ( fromLines.size() > 0 ) )
      {
	Point topoint = null;
	for ( int i = 1; i < fromLines.size(); i++) {
	    Point frompoint = (Point) fromLines.get( i);
	    if ( ( topoint != null )  && ( topoint.x == frompoint.x ) && ( topoint.y == frompoint.y ) )
		{
		    skipsame ++;
		}
	    topoint=frompoint;
	}
	if ( skipsame > 0 )
	    {
		printf("%\n","[WARNING] lines contains " + skipsame + " duplicates points");
	    }
    }
  */
  
  if ( ( this->fromLines != NULL) && ( this->fromLines->count > 0))
    {
        struct sdadapter adapter;
	adapter.f_before=drawlincecompressor_before_pointlist;
	adapter.f_for_each=drawlincecompressor_foreach_pointlist;
	adapter.f_after=drawlincecompressor_after_pointlist;
	adapter.data=this;
	adapter.count=0;

	pointlist_foreach(this->fromLines, &adapter);
    }
  }

  /*
    compute bits of size
   */
void drawlinecompressor_writeRel(struct drawlinecompressor * this,struct sdpoint * point, int size)
{


    /* 11 : next, 10 : previous ; 0* : same
        6 -> 14
        ^    |
        |    v
        3 <- 64
    */
    if ( size != this->previousSize) {
      int cindex, pindex;
      // find index of size
      for ( cindex = 0; cindex < SCODE_MAX; cindex ++) {
        if ( global_scode[cindex] == size ) break;
      }
      // find index of previous size
      for ( pindex = 0; pindex < SCODE_MAX; pindex ++) {
        if ( global_scode[pindex] == this->previousSize ) break;
      }

      if ( pindex < cindex ) {
        /*
         if way with = is better than - then use = next
                                        else use - previous
         0--<--pindex===>==cindex---<---SCODE_MAX
         */
         if ( ((SCODE_MAX - cindex) + pindex ) > ( pindex - cindex ) ) {
           // ( pindex - cindex ) times next
           for ( ; pindex < cindex; pindex ++) {
             bitfieldwriter_write(this->fieldWriter, SCODE_NEXT,2);
           }
         }
         else {
           // ((SCODE_MAX -cindex) + pindex ) fois previous
           for ( pindex = SCODE_MAX - cindex + pindex; pindex > 0; pindex --) {
             bitfieldwriter_write(this->fieldWriter, SCODE_PREVIOUS,2);
           }
         }
      }
      else {
        /*
         if way with = is better than - then use = previous
                                        else use - next
         0-->--cindex===<==pindex--->---SCODE_MAX
         */
         if ( ((SCODE_MAX - pindex) + cindex ) > cindex - pindex ) {
           // ( cindex - pindex ) times previous
           for ( ; cindex < pindex; pindex --) {
             bitfieldwriter_write(this->fieldWriter, SCODE_PREVIOUS,2);
           }
         }
         else {
           // ((SCODE_MAX - pindex) + cindex ) times next
           for ( pindex = SCODE_MAX - pindex + cindex; pindex > 0; pindex --)
           {
             bitfieldwriter_write(this->fieldWriter, SCODE_NEXT,2);
           }
         }
      }
    }  
    // this is good size.
    bitfieldwriter_write(this->fieldWriter,  0,1);
    drawlinecompressor_writeAbs(this, point, size);
  }

/* write given Point with size */
void drawlinecompressor_writeAbs( struct drawlinecompressor * this,struct sdpoint * point, int size)
{
    int codeval;
    int center, max;

    this->previousSize = size;

    if ( size > 32 ) {
      bitfieldwriter_write(this->fieldWriter,  point->x, 32);
      bitfieldwriter_write(this->fieldWriter,  point->y, 32);
    }
    else {
	int nocenter= FALSE;
	/*  size == 3 example :
           X - >
         Y 0 1 2
         | 3 . 4-
         v 5 6 7
         specific application of general case bellow.
      */

      /*    X ->
          Y        0
          | 0    center     max
          v        max

            0      1      2   ...center....                   (max - 1)
           max                                               2 * max - 1
            .                    (0,0)  center  * ( max + 1)
           (max-1)*max -1                                    (max ^ 2) - 1
2       */
      if ( size == 3 ) {
        max = 3;
	center = 1;
	nocenter = TRUE;
      }
      else {
        max = ( 1 << (  size  / 2 ) );
	// biased center ( -center +1, center )
	center = (max / 2) - 1;
      }
      codeval = point->x + center +  (( point->y + center ) * max );
      // central hole impossible BUT we should not really care ( in fact whole center square is impossible since covered by encoding with lower number of bits ) , avoid it only for size 3.
      if (nocenter) {
	  if ( codeval == ( center * max  ) + 1 ) {
	    printf("%s\n","!!! warning (0,0) point is invalid for compression !!!");
	  }
	  if ( codeval > (center * max)  ){
	      codeval --;
	  }
      }
      bitfieldwriter_write(this->fieldWriter,  codeval, size);
    }
}


