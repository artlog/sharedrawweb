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
 */
package lasnier.sharedraw;

import java.awt.event.*;
import java.awt.*;
import java.applet.*;

import java.util.Vector;

public class ShareDrawingEditor extends Applet {

    ShareDrawingLineEditor panel;
    // case used like applet
    ShareDrawClient client;

    public void init2() {
      setLayout(new BorderLayout());
      panel = new ShareDrawingLineEditor();
      add("Center", panel);
    }

    public void init() {
      String server_address = getParameter( "drawServer");
      //try {
      int server_port = Integer.parseInt(getParameter( "drawport"));
      /*}
      catch ( )
	  {
	  }
      */
      initserver( server_address, server_port);
    }

    public void initserver(String pServer, int pPort)
        {
      init2();
      /* need to do init connection with server then... need to Create
         ShareDrawClient
      */
      try {
        client = new ShareDrawClient();
      }
      catch( lasnier.sharedraw.RemoteException r) {
        System.err.println( "rmi error " +
          r.getMessage());
      }
      client.setLineEditor( panel);
      client.initServerConnection( pServer, pPort);
      panel.addItemCreatedListener( client);        
        }

    public ShareDrawingLineEditor getLineEditor() {
      return panel;
    }

    public void destroy() {
      remove(panel);
    }

    public void stop() {
      super.stop();
      client.stop();
    }

    public static void main(String args[]) {
      Frame f = new Frame("ShareDrawTest");
      ShareDrawingEditor drawTest = new ShareDrawingEditor();
      String server_address = null;
      server_address = args[0];
      int server_port = 0;
      server_port = Integer.parseInt(args[1]);
      drawTest.initserver( server_address, server_port);
      drawTest.start();
      f.add("Center", drawTest);
      f.setSize(300, 300);
      f.show();
    }

    public String getAppletInfo() {
      return "A simple drawing program.";
    }
}





