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
 ShareDrawingLine
 keep track of list of Points constituting a line
*/
package lasnier.sharedraw;

import java.awt.*;
import java.util.Vector;
import java.io.*;
import java.io.Serializable;

public class ShareDrawingLine implements Cloneable, Serializable{

  Vector lines;

  public ShareDrawingLine() {
    lines = new Vector();
  }

  public void addPoint( Point point) {
    lines.addElement( point);
  }

  public void setPoints( Vector points) {
    lines = points;
  }

  public Object clone() throws java.lang.CloneNotSupportedException {
    return super.clone();
  }

  private void writePoint( Point p, DataOutputStream destination) throws
    java.io.IOException {
    destination.writeInt( p.x);
    destination.writeInt( p.y);
  }

  private void readPoint( DataInputStream source) throws java.io.IOException {
    Point point = new Point( source.readInt(), source.readInt());
    lines.addElement( point);
  }

  public void save( DataOutputStream destination) throws
    java.io.IOException {
    destination.writeInt( lines.size());
    for (int i=0; i < lines.size(); i++) {
      writePoint( (Point) lines.elementAt(i), destination);
    }
  }

  public void load( DataInputStream source) throws
    java.io.IOException {
    int point_number = source.readInt();
    for (int i=0; i < point_number; i++) {
      readPoint( source);
    }
  }

    /**
     * send over the stream a compressed version of this line
     */
  public void saveKompressed( OutputStream destination) throws
    java.io.IOException {
    DrawLineKompressor kompressor = new DrawLineKompressor( lines);
    kompressor.kompress( destination);
  }

    /**
     * load form the stream a compressed version of this line
     */
  public void loadExpanded( InputStream source) throws
    java.io.IOException {
    DrawLineExpander expander = new DrawLineExpander();
    lines = expander.expand( source);
  }

  public void paint(Graphics g) {
    int np = lines.size();

    /* draw the current lines */
    g.setColor(Color.black);
    g.setPaintMode();
    Point previous = null;
    for (int i=0; i < np; i++) {
      Point current = (Point) lines.elementAt(i);
      if ( i == 0 ) {
        previous = current;
      }
      g.drawLine(previous.x, previous.y, current.x, current.y);
      previous = current;
    }
  }

}

