/**
  Drawing

  contains a set of DrawingLine to create a Drawing
  - feature of load / save
*/
package org.artisanlogiciel.graphics;

import java.awt.Graphics;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.ArrayList;

import org.artisanlogiciel.graphics.Importer;
import org.artisanlogiciel.graphics.ResetException;
import org.artisanlogiciel.graphics.NoMoreLineException;

public class Drawing
{

  private ArrayList<DrawingLine> lines;

  public Drawing()
  {
      reset();
  }

    public void reset() {
    lines = new ArrayList<DrawingLine>(10);
  }

  public void addLine( DrawingLine line) {
    lines.add( line);
  }

  public void saveLines( DataOutputStream destination) throws
    java.io.IOException {
    destination.writeInt( lines.size());
    for ( DrawingLine line : lines )
    {
      line.save( destination);
    }
  }

  public void loadLines( DataInputStream source) throws
    java.io.IOException {
    int nb_lines = source.readInt();
    DrawingLine line;
    for ( int i = 0; i < nb_lines; i++ ) {
      line = new DrawingLine();
      line.load( source);
      lines.add( line);
    }
  }
  
  public void importImage( Importer importer) throws
	    java.io.IOException 
  {
	  importer.importInto(this);
  }

  public void saveLinesKompressed( DataOutputStream destination) throws
    java.io.IOException {
    destination.writeInt( lines.size());    
    for ( DrawingLine line : lines )
    {
      line.saveKompressed( destination);
    }
  }

  public void loadLinesExpanded( DataInputStream source) throws
    java.io.IOException {
    // DrawLineExpander expander = new DrawLineExpander();
    int nb_lines = source.readInt();
    DrawingLine line;
    for ( int i = 0; i < nb_lines; i++ ) {
      line = new DrawingLine();
      line.loadExpanded( source);
      lines.add( line);
    }
  }

  public int length() {
    return lines.size();
  }

  public DrawingLine getLine( int line) throws ResetException, NoMoreLineException {
    if ( line == lines.size() ) {
      throw new NoMoreLineException();
    }
    if ( line > lines.size() ) {
      throw new ResetException();
    }
    return lines.get( line);
  }

  public ArrayList<DrawingLine> getLines() { 
	return new ArrayList<DrawingLine>(lines);
  }
  
  /** Not a copy, use with care */
  public ArrayList<DrawingLine> getInternLines() { 
	return lines;
  }

}
