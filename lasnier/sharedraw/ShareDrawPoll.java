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
 ShareDrawPoll.java
 from Leila Semlali && Philippe Lhardy

 in applet context applet should initiate request of informations

*/
package lasnier.sharedraw;

import java.util.*;
import java.awt.*;
import java.io.Serializable;

public class ShareDrawPoll extends Thread {

  transient private ShareDrawServerMethods drawServer;
  transient private ShareDrawClientMethods drawClient;
  int pollingtime = 0;
  boolean isRunning = false;

  public ShareDrawPoll() {
    super();
    drawServer = null;
  }

  public void setDrawServer( ShareDrawServerMethods server) {
    drawServer = server;
  }

  public void setDrawClient( ShareDrawClientMethods client) {
    drawClient = client;
  }

  public void run()  {
    try {
      drawClient.reset();
    }
    catch ( lasnier.sharedraw.RemoteException r) {
      // should not occur given drawClient is no more remote
      return;
    }
    isRunning = true;
    while ( isRunning) {
      try {
      if ( pollingtime > 0 ) {
        sleep( pollingtime);
      }
      }
      catch( InterruptedException i) {
        isRunning = false;
        return;
      }
      pollServer();
    }
  }

  public void requireStop() {
    isRunning=false;
    pollingtime=0;
  }

  private void pollServer() {

    ShareDrawingLine line = null;
    try {
      line = drawServer.getLine(drawClient.getNextLineIndex());
      drawClient.addLine( line);
    }
    catch( lasnier.sharedraw.RemoteException r) {
      System.out.println( "polling problem " + r.getMessage());
      // BUG SHOULD TRY TO RESET CONNECTION
      slower( 500);
      return;
    }
    catch( ResetException e) {
      try {
        drawClient.reset();
        // no wait
        faster(1);
      }
      catch ( lasnier.sharedraw.RemoteException r2) {
      }
    }
    catch( NoMoreLineException e) {
      // should sait a longer wait.
      slower( 500);
      return;
    }
    catch( DrawException de) {
	System.err.println( "unexcpected exception");
	de.printStackTrace();
    }
    faster(3);

  }

  public void slower( int value) {
    pollingtime += value;
  }

  public void faster( int value) {
    pollingtime = pollingtime - pollingtime / value;
  }


}
