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
package lasnier.sharedraw;

/**
 class ThresholdPolicy

 @author philippe lhardy
 */

import org.artisanlogiciel.graphics.DrawingLine;

class ThresholdPolicy implements ImageChangeListener {

  int threshold;
  ImageHeuristic heuristic;

  void setHeuristic( ImageHeuristic h) {
    heuristic = h;
  }

  ImageHeuristic getHeuristic() {
    return heuristic;
  }
  
  public void receiveImageChangeEvent( ImageChangeEvent event) {

    ShareDrawing image = event.getImage();
    DrawingLine line = event.getLine();

    if ( event.getType == ImageChangeEvent.AddLine ) {
      heuristic.addLineTo( line, image);
    }
    else 
    if ( event.getType == ImageChangeEvent.RemoveLine ) {
      heuristic.removeLineFrom( line, image);
    }

    if ( heuristic.getValue( image) > threshold ) {
      heuristic.reduceImage( image);
    }
  }
  
}
