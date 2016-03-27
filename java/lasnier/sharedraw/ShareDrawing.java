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
  ShareDrawing
  by Leila Semlali && Philippe Lhardy
  contains a set of ShareDrawingLine to create a Drawing
  - feature of load / save
  - paint
*/
package lasnier.sharedraw;

import java.awt.Graphics;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Vector;

import org.artisanlogiciel.compression.graphics.DrawLineExpander;

public class ShareDrawing {

  Vector<ShareDrawingLine> lines;

  public ShareDrawing() {
    reset();
  }

  public Enumeration elements() {
    return lines.elements();
  }

  public void reset() {
    lines = new Vector<>();
  }

  public void paint( Graphics g) {
    ShareDrawingLine line;
    for ( Enumeration e = lines.elements(); e.hasMoreElements(); ) {
      line = (ShareDrawingLine) e.nextElement();
      line.paint( g);
    }
  }

  public void addLine( ShareDrawingLine line) {
    lines.addElement( line);
  }

  public void saveLines( DataOutputStream destination) throws
    java.io.IOException {
    destination.writeInt( lines.size());
    for ( ShareDrawingLine line : lines )
    {
      line.save( destination);
    }
  }

  public void loadLines( DataInputStream source) throws
    java.io.IOException {
    int nb_lines = source.readInt();
    ShareDrawingLine line;
    for ( int i = 0; i < nb_lines; i++ ) {
      line = new ShareDrawingLine();
      line.load( source);
      lines.addElement( line);
    }
  }

  public void saveLinesKompressed( DataOutputStream destination) throws
    java.io.IOException {
    destination.writeInt( lines.size());    
    for ( ShareDrawingLine line : lines )
    {
      line.saveKompressed( destination);
    }
  }

  public void loadLinesExpanded( DataInputStream source) throws
    java.io.IOException {
    DrawLineExpander expander = new DrawLineExpander();
    int nb_lines = source.readInt();
    ShareDrawingLine line;
    for ( int i = 0; i < nb_lines; i++ ) {
      line = new ShareDrawingLine();
      line.loadExpanded( source);
      lines.addElement( line);
    }
  }

  public int length() {
    return lines.size();
  }

  public ShareDrawingLine getLine( int line) throws ResetException, NoMoreLineException {
    if ( line == lines.size() ) {
      throw new NoMoreLineException();
    }
    if ( line > lines.size() ) {
      throw new ResetException();
    }
    return (ShareDrawingLine) lines.elementAt( line);
  }

  public ArrayList<ShareDrawingLine> getLines() { 
	return new ArrayList<ShareDrawingLine>(lines);
  }
}
