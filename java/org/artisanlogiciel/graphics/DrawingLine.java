/**
 DrawingLine
 keep track of list of Points constituting a line
*/
package org.artisanlogiciel.graphics;

import java.awt.Point;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Serializable;
import java.util.ArrayList;

import org.artisanlogiciel.compression.graphics.DrawLineExpander;
import org.artisanlogiciel.compression.graphics.DrawLineKompressor;

public class DrawingLine implements Cloneable, Serializable
{

    ArrayList<Point> lines;

  public DrawingLine() {
    lines = new ArrayList<>();
  }

  public void addPoint( Point point) {
    lines.add( point);
  }

  public void setPoints( ArrayList<Point> points) {
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
    lines.add( point);
  }

  public void save( DataOutputStream destination) throws
    java.io.IOException {
    destination.writeInt( lines.size());
    for (int i=0; i < lines.size(); i++) {
      writePoint( (Point) lines.get(i), destination);
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

    public ArrayList<Point> getPoints() {
	
	return new ArrayList<Point>(lines); 
    }

    public ArrayList<Point> internalGetPoints() {
	
	return lines; 
    }
    

}

