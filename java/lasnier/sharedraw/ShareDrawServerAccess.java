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

 author: Philippe Lhardy
*/
package lasnier.sharedraw;

import java.net.Socket;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.IOException;

import org.artisanlogiciel.graphics.DrawingLine;

public class ShareDrawServerAccess
implements ShareDrawServerMethods, Runnable {

    final static int METH_ADDLINE=1;
    final static int METH_SAVE=2;
    final static int METH_LOAD=3;
    final static int METH_SAVEKOMPRESSED=4;
    final static int METH_LOADEXPANDED=5;
    final static int METH_RESET=6;
    final static int METH_GETLINE=7;

    final static int ACK_RETURN=1024;
    final static int ACK_EXCEPTION=1025;

    protected InputStream mInputStream;
    protected OutputStream mOutputStream;

    protected ObjectInputStream objectinputstream;
    protected ObjectOutputStream objectoutputstream;

    protected Socket mConnection = null;
    protected ShareDrawServer mServer = null;

    /**
     * client case
     */
    public ShareDrawServerAccess( String pServerAddress, int pServerPort)
	throws lasnier.sharedraw.RemoteException
    {
	try {
	    mConnection = new Socket( pServerAddress, pServerPort);
	}
	catch ( java.net.UnknownHostException uhe)
	    {
		System.err.println( uhe );
		uhe.printStackTrace();
		throw new RemoteException();
	    }
	catch ( IOException io )
	    {
		System.err.println( "client handling disconnection");
		io.printStackTrace();
	    }
	setstreams();
    }

    /** 
     * server accept client case
     */
    public ShareDrawServerAccess( ShareDrawServer pServer, Socket pClientSocket)
	throws lasnier.sharedraw.RemoteException
    {
	mConnection = pClientSocket;
	mServer = pServer;
	setstreams();
	Thread connectionthread = new Thread(this);
	connectionthread.start();

    }

    protected void setstreams()
    {
	/*
	  warning always create objectoutputstream first
	  because objectinputstream read a header from it
	*/
	try {
	    mInputStream = mConnection.getInputStream();
	    mOutputStream = mConnection.getOutputStream();
	    objectoutputstream = new ObjectOutputStream( mOutputStream);
	    objectinputstream = new ObjectInputStream( mInputStream);
	}
	catch ( IOException io )
	    {
		System.err.println( "problem when extracting streams from sockets");
		io.printStackTrace();
	    }

    }

    
    /**
     * server case
     * listen for one client.
     */
    public void run()
    {
	try {
	    while ( ! server_receive( objectinputstream.readInt()));
	}
	catch ( IOException io )
	    {
		System.err.println( "client handling disconnection");
		io.printStackTrace();
	    }
	catch ( DrawException e)
	    {
		e.printStackTrace();
	    }
	catch ( RemoteException re )
	    {
		System.err.println( "client handling excepetion");
		re.printStackTrace();
	    }
    }

    protected void handleexception()
	throws DrawException,
	       RemoteException
    {
	try
	{
	int retValue = objectinputstream.readInt();

        if ( retValue == ACK_EXCEPTION )
	    {
		System.out.println("exception received !");
		DrawException  drawexception = (DrawException) objectinputstream.readObject();
		System.out.println( "=>" + drawexception );
		throw drawexception;
	    }
	if ( retValue != ACK_RETURN)
	    {
		System.err.println( " protocol error ");
	    }
	}
	catch ( IOException io )
        {
	    System.err.println( "protocol or network problem" + io.toString());
	    io.printStackTrace();
        }
	catch ( ClassNotFoundException cnf)
        {
	    System.err.println( "are all classes in sync ?" + cnf.toString());
	    cnf.printStackTrace();
        }

    }

    /**
     * has to be overriden for new extensions
     */
    protected boolean server_receive_extended( int pCommand)
	throws IOException,
	       DrawException,
	       RemoteException
    {
	return true;
    }
    
    public boolean server_receive(int pCommand)
	throws IOException,
	       DrawException,
	       RemoteException

    {

	System.out.println( pCommand);
	boolean error = false;
	try {
	String ref;
	switch ( pCommand )
	{
	    case METH_ADDLINE:
		DrawingLine line = (DrawingLine) objectinputstream.readObject();
		// DrawingLine line = new DrawingLine();
		// line.readObject( objectinputstream );
		mServer.addLine( line);
		break;
	    case METH_SAVE:
		ref = objectinputstream.readUTF();
		mServer.save( ref);
		break;
	    case METH_LOAD:
		ref = objectinputstream.readUTF();
		mServer.load( ref);
		break;
	    case METH_SAVEKOMPRESSED:
		ref = objectinputstream.readUTF();
		mServer.saveKompressed( ref);
		break;
	    case METH_LOADEXPANDED:
		ref = objectinputstream.readUTF();
		mServer.loadExpanded( ref);
		break;
	    case METH_RESET:
		mServer.reset();
		break;
	    case METH_GETLINE:
		int lineindex = objectinputstream.readInt();
		DrawingLine gotline = mServer.getLine( lineindex );
		objectoutputstream.writeInt(ACK_RETURN);
		objectoutputstream.writeObject( gotline);
		objectoutputstream.flush();
		return error;
	default:
	    // if no exception then extended should handle itself ACK_RETURN
	    error = server_receive_extended( pCommand);
	    return error;
	}
	objectoutputstream.writeInt(ACK_RETURN);
	objectoutputstream.flush();
	}
	catch ( DrawException drawexception )
	    {
		// should thow it to the other side !
		System.err.println( "propagate exception");
		objectoutputstream.writeInt(ACK_EXCEPTION);
		objectoutputstream.flush();
		System.err.println( drawexception );
		objectoutputstream.writeObject( drawexception);
		objectoutputstream.flush();
	    }
	catch ( ClassNotFoundException cnf)
	    {
		System.err.println( "are all classes in sync ?");
		cnf.printStackTrace();
	    }

	return error;
    }

   protected void handleremoteexception()
   throws RemoteException
   {
      try
      {
	  handleexception();
      }
      catch ( DrawException e)
      {
          System.err.println( "protocol error" + e.toString());
          e.printStackTrace();
      }
  }

  public synchronized void addLine( DrawingLine line)
      throws RemoteException
  {
      try
      {
	  objectoutputstream.writeInt( METH_ADDLINE);
	  objectoutputstream.writeObject( line);
	  objectoutputstream.flush();
      }
      catch ( IOException io )
      {
	  System.err.println( "server disconnection" + io.toString());
	  io.printStackTrace();
      }
      handleremoteexception();
  }

  public  synchronized void save( String ref)
      throws RemoteException
  {
      try
	  {
      objectoutputstream.writeInt( METH_SAVE);
      objectoutputstream.writeUTF( ref);
	  objectoutputstream.flush();
	}
	catch ( IOException io )
	    {
		System.err.println( "server disconnection");
		io.printStackTrace();
	    }
      handleremoteexception();
  }

  public  synchronized void load( String ref)
      throws RemoteException
  {
      try
      {
	  objectoutputstream.writeInt( METH_LOAD);
	  objectoutputstream.writeUTF( ref);
	  objectoutputstream.flush();
      }
      catch ( IOException io )
      {
	  System.err.println( "server disconnection");
	  io.printStackTrace();
      }
      handleremoteexception();
  }

  public  synchronized void saveKompressed( String ref)
      throws RemoteException
  {
      try
	  {
      objectoutputstream.writeInt( METH_SAVEKOMPRESSED);
      objectoutputstream.writeUTF( ref);
	  objectoutputstream.flush();
	}
	catch ( IOException io )
	    {
		System.err.println( "server disconnection");
		io.printStackTrace();
	    }
      handleremoteexception();
  }

  public  synchronized void loadExpanded( String ref)
      throws RemoteException
  {
      try
      {
	  objectoutputstream.writeInt( METH_LOADEXPANDED);
	  objectoutputstream.writeUTF( ref);
	  objectoutputstream.flush();
      }
      catch ( IOException io )
      {
	  System.err.println( "server disconnection");
	  io.printStackTrace();
      }
      handleremoteexception();
  }

  public  synchronized void reset()
      throws RemoteException
  {
      try
      {
	  objectoutputstream.writeInt( METH_RESET);
	  objectoutputstream.flush();
      }
      catch ( IOException io )
      {
	   System.err.println( "server disconnection");
	   io.printStackTrace();
      }
      handleremoteexception();
  }

  public synchronized DrawingLine getLine( int pLineIndex)
      throws
    RemoteException,
    DrawException
  {
      DrawingLine line = null;
      System.out.println( "getLine(" + pLineIndex  +")");
      try
      {
	  objectoutputstream.writeInt( METH_GETLINE);
	  objectoutputstream.writeInt( pLineIndex);
	  objectoutputstream.flush();
	  handleexception();
	  line = (DrawingLine) objectinputstream.readObject();
      }
      catch ( IOException io )
      {
	  System.err.println( "server disconnection " + io.toString());
	  io.printStackTrace();
      }
      catch ( ClassNotFoundException cnf)
      {
	  System.err.println( "network or protocol error" + cnf.toString());
	  cnf.printStackTrace();
      }
      return line;

  }

}


