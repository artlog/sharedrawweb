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

import java.awt.Graphics;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.ArrayList;
import java.util.Enumeration;

import org.artisanlogiciel.compression.graphics.DrawLineExpander;
import java.util.Collections;

public class ShareDrawing {

  private ArrayList<ShareDrawingLine> lines;

  public ShareDrawing() {
    reset();
  }

    /*
      @Deprecated Should get lines as ArrayList<ShareDrawingLine>
    */
  public Enumeration elements() {
      return Collections.enumeration(lines);
  }

  public void reset() {
    lines = new ArrayList<ShareDrawingLine>(10);
  }

  public void paint( Graphics g) {
    for (ShareDrawingLine line : lines )
    {
      line.paint( g);
    }
  }

  public void addLine( ShareDrawingLine line) {
    lines.add( line);
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
      lines.add( line);
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
      lines.add( line);
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
    return (ShareDrawingLine) lines.get( line);
  }

  public ArrayList<ShareDrawingLine> getLines() { 
	return new ArrayList<ShareDrawingLine>(lines);
  }
}
