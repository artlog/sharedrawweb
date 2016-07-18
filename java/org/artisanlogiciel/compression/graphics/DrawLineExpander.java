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
 DrawingLineExpander
 decompress a draw line from a stream

 main algorithm is to code only delta displacement in littlest possible size.
*/
package org.artisanlogiciel.compression.graphics;

import java.awt.Point;
import java.io.InputStream;
import java.util.ArrayList;

public class DrawLineExpander {

  ArrayList<Point> expandedLines;
  int currentSize;
  int scode[] = {3,6,14,30,64};
  final static int SCODE_MAX = 4;
  private BitFieldReader fieldReader = null;

  public DrawLineExpander() {
    expandedLines = new ArrayList<>();
    fieldReader = new BitFieldReader();
  }

  /* create a List of points */
  public ArrayList<Point> expand( InputStream input ) throws java.io.IOException {
    fieldReader.setInputStream( input);
    int point_count = fieldReader.read( 32);
    // System.out.println( point_count);
    expandedLines.add( readAbs( 64));
    for ( int i = 1; i < point_count; i ++ ) {
      Point point = readRel();
      if ( currentSize == 64 ) {
        // absolute
        expandedLines.add( point);
      }
      else {
        // relative
        Point previous = (Point) expandedLines.get(expandedLines.size()-1);
        point.x = point.x + previous.x;
        point.y = point.y + previous.y;
        expandedLines.add( point);
      }
    }
    return expandedLines;
  }

  /*
    read point with size encoding depending on first bits
   */
  private Point readRel() throws java.io.IOException {

    int index;

    /* 11 : next, 10 : previous ; 0* : same
        6 -> 14->30
        ^        |
        |        v
        3   <-  64
    */
    // find index of current size
    for ( index = 0; index < SCODE_MAX; index ++) {
      if ( scode[index] == currentSize ) break;
    }

    // read size modifier until it is good one
    while ( fieldReader.read(1) != 0 ) {
      index = index + ( ( fieldReader.read(1) == 0 ) ? -1 : 1);
      // handle circularity
      if ( index > SCODE_MAX ) {
        index = 0;
      }
      if ( index < 0 ) {
        index = SCODE_MAX;
      }
    }
    return readAbs( scode[index]);
  }

  /* write given Point with size */
  Point readAbs( int size) throws java.io.IOException {
    int codeval;
    int center, max;

    currentSize = size;

    Point point = new Point();
    if ( size > 32 ) {
      point.x = fieldReader.read(32);
      point.y = fieldReader.read(32);
    }
    else {
	boolean nocenter=false;
      codeval = fieldReader.read( size); 
      // System.out.println( codeval);
      /* 
        for coding/decoding scheme see
           lasnier.sharedraw.DrawLineKompressor.java 
      */
      if ( size == 3 ) {
        max = 3;
	center = 1;
	nocenter=true;
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
		  System.out.println(
				     "!!! reserved value for decompression should not occur !!!");
	      }
	  }
      point.x = ( codeval  %  max) - center;
      point.y = ( codeval   / max) - center;
      // System.out.println( point.toString());
    }
    return point;
  }

}

