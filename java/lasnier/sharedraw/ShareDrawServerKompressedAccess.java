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
 ShareDrawServerAccess.java
 
 does what rmi did it serialize requests/responses on a socket
 and handle exceptions.

 add getline and addline kompressed ( normal gain x4 )

 author: Philippe Lhardy
*/
package lasnier.sharedraw;

import java.io.IOException;
import java.net.Socket;

public class ShareDrawServerKompressedAccess
extends ShareDrawServerAccess {

    final static int METH_KOMP_ADDLINE = 512;
    final static int METH_KOMP_GETLINE = 513;

    /**
     * client case
     */
    public ShareDrawServerKompressedAccess( String pServerAddress, int pServerPort)
	throws lasnier.sharedraw.RemoteException
    {
	super( pServerAddress, pServerPort);
    }

    /** 
     * server accept client case
     */
    public ShareDrawServerKompressedAccess( ShareDrawServer pServer, Socket pClientSocket)
	throws lasnier.sharedraw.RemoteException
    {
	super( pServer, pClientSocket);
    }

    protected void setstreams()
    {
	super.setstreams();
    }

    /**
     * has to be overriden for new extensions
     */
    protected boolean server_receive_extended( int pCommand)
	throws IOException,
	       DrawException,
	       RemoteException
    {
	boolean error = false;
	ShareDrawingLine line = null; 
	switch ( pCommand)
	    {
	    case METH_KOMP_ADDLINE:
		line = new ShareDrawingLine();
		line.loadExpanded(mInputStream);
		mServer.addLine(line);
		break;
	    case METH_KOMP_GETLINE:
		int lineindex = objectinputstream.readInt();
		line = mServer.getLine( lineindex );
		objectoutputstream.writeInt(ACK_RETURN);
		objectoutputstream.flush();
		line.saveKompressed(mOutputStream);
		mOutputStream.flush();
		//to not write twice ACK_RETURN
		return error;
	    default:
		error = true;
	    }
	objectoutputstream.writeInt(ACK_RETURN);
	objectoutputstream.flush();
	return error;
    }
    

  public synchronized void addLine( ShareDrawingLine line)
      throws RemoteException
  {
      try
      {
	  objectoutputstream.writeInt( METH_KOMP_ADDLINE);
	  objectoutputstream.flush();
	  line.saveKompressed( mOutputStream);
	  objectoutputstream.flush();
      }
      catch ( IOException io )
      {
	  System.err.println( "server disconnection" + io.toString());
	  io.printStackTrace();
      }
      handleremoteexception();
  }

  public synchronized ShareDrawingLine getLine( int pLineIndex)
      throws
    RemoteException,
    DrawException
  {
      ShareDrawingLine line = null;
      System.out.println( "getLine(" + pLineIndex  +")");
      try
      {
	  objectoutputstream.writeInt( METH_KOMP_GETLINE);
	  objectoutputstream.writeInt( pLineIndex);
	  objectoutputstream.flush();
	  handleexception();
	  line = new ShareDrawingLine();
	  line.loadExpanded(mInputStream);
      }
      catch ( IOException io )
      {
	  System.err.println( "server disconnection " + io.toString());
	  io.printStackTrace();
      }
      return line;
  }

}


