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
 awt Panel that is used to display / edit lines
 **/

package lasnier.sharedraw;

import java.awt.*;
import java.awt.event.*;

class ShareDrawingLineEditor
    extends Panel
    implements MouseListener,  MouseMotionListener
{
    // line currently edited
    ShareDrawingLine drawingLine;
    // full drawing
    ShareDrawing fullDrawing;
    ItemCreatedListener itemCreatedListener;

    public ShareDrawingLineEditor()
    {
      setBackground(Color.white);
      addMouseMotionListener(this);
      addMouseListener(this);
      fullDrawing = null;
      drawingLine = new ShareDrawingLine();
    }
  
    public void setDrawingLine( ShareDrawingLine line)
    {
      drawingLine = line;
    }

    public void setDrawing( ShareDrawing drawing)
    {
      fullDrawing = drawing;
      repaint();
    }

    public void addItemCreatedListener( ItemCreatedListener listener)
    {
      itemCreatedListener = listener;
    }

    public void mouseDragged(MouseEvent e)
    {
      e.consume();
      drawingLine.addPoint( new Point(e.getX(), e.getY()));
      refreshLine( drawingLine);
    }

    public void mouseMoved(MouseEvent e)
    {
    }

    public void mousePressed(MouseEvent e)
    {
      e.consume();
      drawingLine.addPoint( new Point(e.getX(), e.getY()));
      refreshLine( drawingLine);
    }

    public void mouseReleased(MouseEvent e)
    {
      e.consume();
      // should advertise for this new line
      if ( drawingLine != null) {
        if ( itemCreatedListener != null) {
          itemCreatedListener.receiveItemCreated( drawingLine);
        }
        drawingLine = new ShareDrawingLine();
      }      
    }

    public void mouseEntered(MouseEvent e)
    {
    }

    public void mouseExited(MouseEvent e)
    {
    }

    public void mouseClicked(MouseEvent e)
    {
    }

    public void refreshLine( ShareDrawingLine line)
    {
      line.paint( getGraphics());
    }

    public void paint(Graphics g)
    {
      drawingLine.paint( g);
      if ( fullDrawing != null)
      {
        fullDrawing.paint( g);
      }
    }

}
