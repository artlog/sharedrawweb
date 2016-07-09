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
 ShareDrawClient.java

 - receive images from ShareDrawServer
 - create new Line by user requirement
 - send update to server

@author Leila Semlali
@author Philippe Lhardy
**/
package lasnier.sharedraw;

import java.util.*;
import java.awt.*;
import java.io.Serializable;

public class ShareDrawClient implements
  ItemCreatedListener,
  ShareDrawClientMethods,
  Serializable {

  transient private ShareDrawing localImage;
  transient private ShareDrawServerMethods drawingServer;
  transient private ShareDrawingLineEditor lineEditor;
  transient private ShareDrawPoll serverPoller;

  public ShareDrawClient() throws lasnier.sharedraw.RemoteException {
    super();
    localImage = new ShareDrawing();
    drawingServer = null;
  }

  public void receiveItemCreated( ShareDrawingLine line)  {
    try {
      if ( drawingServer != null) {
        drawingServer.addLine( line);
      }
    }
    catch (lasnier.sharedraw.RemoteException e) {
      System.err.println( "server addLine RMI remote problem");
    }
  }

  public void addLine( ShareDrawingLine line)
  {
    ShareDrawingLine copy = null;
    if (localImage == null) {
      localImage = new ShareDrawing();
      if (lineEditor != null) {
        lineEditor.setDrawing( localImage);
      }
    }
    /* line is cloned to not use same reference as caller
       once added (by a server) line can't be changed
    */
    try {
      copy = (ShareDrawingLine) line.clone();
      localImage.addLine( copy );
    }
    catch( java.lang.CloneNotSupportedException exception) {
      System.err.println( "clone not supported");
      return;
    }
    if (lineEditor != null) {
      lineEditor.refreshLine( copy);
    }
  }

  public void reset() {
    /* previous image is garbaged */
    localImage = new ShareDrawing();
    if ( lineEditor != null)
    {
      lineEditor.setDrawing( localImage);
    }
  }

  public void initServerConnection( String server_address, int pServerPort) {

      /* we will need to study these with kaffe by example.
	 any case it won't be RMI...
    if (System.getSecurityManager() == null) {
      System.setSecurityManager(new RMISecurityManager());
    }
      */
    try {
      drawingServer = (ShareDrawServerMethods) new ShareDrawServerKompressedAccess( server_address, pServerPort);
    } catch (Exception e) {
      System.err.println("Connection server exception: " +
        e.getMessage());
      e.printStackTrace();
      return;
    }

    /* no more need for poller... ( no callback )
    try {
      drawingServer.addRemoteClient( this);
    }
    catch( lasnier.sharedraw.RemoteException r) {
      System.err.println("Remote client registring to server exception " +
        r.getMessage());
      r.printStackTrace();
    }
    */
    serverPoller = new ShareDrawPoll();
    serverPoller.setDrawServer( drawingServer);
    serverPoller.setDrawClient( this);
    serverPoller.start();
  }

  public void setLineEditor( ShareDrawingLineEditor editor) {
    lineEditor = editor;
  }

  public void run() {
    Frame f = new Frame("ShareDrawTest");
    ShareDrawingEditor drawTest = new ShareDrawingEditor();
    drawTest.init2();
    lineEditor = drawTest.getLineEditor();
    lineEditor.addItemCreatedListener( this);
    lineEditor.setDrawing( localImage);
    drawTest.start();
    f.add("Center", drawTest);
    f.setSize(300, 300);
    f.show();
  }

  public void stop() {
    serverPoller.requireStop();
  }

  public void setServer( ShareDrawServerMethods server) {
    drawingServer = server;
  }

  public void finish() {
    localImage = null;
    drawingServer = null;
  }

  public int getNextLineIndex() {
    if ( localImage == null) {
      return 0;
    }
    return localImage.length();
  }

}









