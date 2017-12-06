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
/**
  ShareDrawing

  contains a set of ShareDrawingLine to create a Drawing
  - feature of load / save
  - paint

@author Leila Semlali
@author Philippe Lhardy
*/
package lasnier.sharedraw;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Point;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.ArrayList;

import org.artisanlogiciel.graphics.Drawing;
import org.artisanlogiciel.graphics.DrawingLine;

public class ShareDrawing
extends Drawing
{

  public ShareDrawing()
  {
      super();
  }

  public static void paintLine(DrawingLine line, Graphics g) {
    /* draw the current lines */
    g.setColor(Color.black);
    g.setPaintMode();
    Point previous = null;
    for (Point current:line.internalGetPoints() ) {
      if ( previous == null ) {
        previous = current;
      }
      g.drawLine(previous.x, previous.y, current.x, current.y);
      previous = current;
    }
  }

  public void paint( Graphics g) {
    for (DrawingLine line : getInternLines() )
    {
	paintLine(line, g);
    }
  }

}
