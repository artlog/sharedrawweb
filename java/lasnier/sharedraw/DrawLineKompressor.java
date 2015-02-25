/*
    This file is part of ShareDrawWeb.

    ShareDrawWeb is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ShareDrawWeb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ShareDrawWeb; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*
 DrawingLineKompressor
 compress a draw line over a stream

 main algorithm is to code only delta displacement in littlest possible size.
*/
package lasnier.sharedraw;

import java.awt.*;
import java.util.Vector;
import java.io.*;
import java.io.Serializable;

public class DrawLineKompressor {

  Vector fromLines;
  int previousSize;
  int scode[] = {3,6,14,30,64};
  final static int SCODE_MAX = 4;
  private BitFieldWriter fieldWriter = null;

  public DrawLineKompressor( Vector lines) {
    fromLines = lines;
    fieldWriter = new BitFieldWriter();
  }

  public void kompress( OutputStream output ) throws java.io.IOException {
    fieldWriter.setOutputStream( output);
    int skipsame=0;
    
    if ( ( fromLines != null) && ( fromLines.size() > 0)) {
	Point topoint = null;
	for ( int i = 1; i < fromLines.size(); i++) {
	    Point frompoint = (Point) fromLines.elementAt( i);
	    if ( ( topoint != null )  && ( topoint.x == frompoint.x ) && ( topoint.y == frompoint.y ) )
		{
		    skipsame ++;
		}
	    topoint=frompoint;
	}
	if ( skipsame > 0 )
	    {
		System.out.println("[WARNING] lines contains " + skipsame + " duplicates points");
	    }
    }

    if ( ( fromLines != null) && ( fromLines.size() > 0)) {
      Point frompoint = (Point) fromLines.elementAt(0);
      Point point, topoint;
      topoint = new Point();
      int x, y, center, size;
      fieldWriter.write( fromLines.size() - skipsame, 32);
      writeAbs( frompoint, 64);
      for ( int i = 1; i < fromLines.size(); i++) {
        point = (Point) fromLines.elementAt( i);
        topoint.x = point.x - frompoint.x;
        topoint.y = point.y - frompoint.y;
        // find wich size to use...
	if (( topoint.x == 0 ) && ( topoint.y == 0 ))
	    {
		// ARGH THIS IS INVALID !
	      System.out.println("[ERROR] (0,0) point is invalid for compression !!!");
	      continue;
	    }
        if ( (Math.abs( topoint.x) < 2) && (Math.abs(topoint.y) < 2)) {
          writeRel( topoint, 3);
        }
        else {
          size = 64;
          for ( int index = 1; index < ( SCODE_MAX - 1 ); index ++) {
	      /* center = ( ( 1 << (  scode[index]  / 2 ) ) - 1 ) / 2;
            if ( (Math.abs(topoint.x) <= center)
              && (Math.abs(topoint.y) <= center))
            {                          
              size = scode[index];
            }
	      */
	      // center is biased there are more + than - , ex for 6bits :(x,y) within ( -3,4 ) x (-3,4)
	      center = 1 << ( (  scode[index]  / 2 )  - 1 );
	      if ( ( topoint.x <= center  ) && ( topoint.x > -center )
		   && ( topoint.y <= center  ) && ( topoint.y > -center ) )
		  {
		      size = scode[index];
		      // always take the smaller size.
		      break; 
		  }
          }
          if ( size < 32 ) {
            writeRel( topoint, size);
          }
          else {
            // warning absolute
            writeRel( point, 64);
          }
        }
        // new frompoint is previous point
        frompoint = point;
      }
      fieldWriter.padToWord();
    }
  }

  /*
    compute bits of size
   */
  void writeRel( Point point, int size) throws java.io.IOException {

    /*
    System.out.println( "point " + point.toString());
    System.out.println( "size " + Integer.toString( size));
    */

    /* 11 : next, 10 : previous ; 0* : same
        6 -> 14
        ^    |
        |    v
        3 <- 64
    */
    if ( size != previousSize) {
      int cindex, pindex;
      // find index of size
      for ( cindex = 0; cindex < SCODE_MAX; cindex ++) {
        if ( scode[cindex] == size ) break;
      }
      // find index of previous size
      for ( pindex = 0; pindex < SCODE_MAX; pindex ++) {
        if ( scode[pindex] == previousSize ) break;
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
             fieldWriter.write( 3,2);
           }
         }
         else {
           // ((SCODE_MAX -cindex) + pindex ) fois previous
           for ( pindex = SCODE_MAX - cindex + pindex; pindex > 0; pindex --) {
             fieldWriter.write( 2,2);
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
           // ( cindex - pindex ) fois previous
           for ( ; cindex < pindex; pindex --) {
             fieldWriter.write( 2,2);
           }
         }
         else {
           // ((SCODE_MAX - pindex) + cindex ) fois next
           for ( pindex = SCODE_MAX - pindex + cindex; pindex > 0; pindex --)
           {
             fieldWriter.write( 3,2);
           }
         }
      }
    }  
    // this is good size.
    fieldWriter.write( 0,1);
    writeAbs( point, size);
  }

  /* write given Point with size */
  void writeAbs( Point point, int size) throws java.io.IOException {
    int codeval;
    int center, max;

    previousSize = size;

    if ( size > 32 ) {
      fieldWriter.write( point.x, 32);
      fieldWriter.write( point.y, 32);
    }
    else {
	boolean nocenter = false;
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
	nocenter = true;
      }
      else {
        max = ( 1 << (  size  / 2 ) );
	// biased center ( -center +1, center )
	center = (max / 2) - 1;
      }
      /*
      System.out.println( "size" + Integer.toString( size));
      System.out.println( "max, center " + Integer.toString( max) + " " + Integer.toString( center));
      System.out.println( "x, y " + Integer.toString( point.x) + " " + Integer.toString( point.y));
      */
      codeval = point.x + center +  (( point.y + center ) * max );
      // central hole impossible BUT we should not really care ( in fact whole center square is impossible since covered by encoding with lower number of bits ) , avoid it only for size 3.
      if (nocenter) {
	  if ( codeval == ( center * max  ) + 1 ) {
	      System.out.println("!!! warning (0,0) point is invalid for compression !!!");
	  }
	  if ( codeval > (center * max)  ){
	      codeval --;
	  }
      }
      // System.out.println( "code " + Integer.toString( codeval));
      fieldWriter.write( codeval, size);
    }
  }


}

