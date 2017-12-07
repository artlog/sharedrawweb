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
ShareDrawServer.java

 - Handle multiple ShareDrawClient that display drawing on client side
 - Load and Save Drawings to local file

@author Leila Semlali
@author Philippe Lhardy
**/
package lasnier.sharedraw;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.net.ServerSocket;
import java.util.ArrayList;

import org.artisanlogiciel.graphics.IMAImporter;
import org.artisanlogiciel.graphics.Importer;
import org.artisanlogiciel.graphics.IMAWriter;
import org.artisanlogiciel.graphics.DrawingLine;
import org.artisanlogiciel.graphics.ResetException;
import org.artisanlogiciel.graphics.NoMoreLineException;

public class ShareDrawServer implements
ShareDrawServerMethods, Runnable {

  transient private ShareDrawing localImage;
  transient private ArrayList<ShareDrawClientMethods> localUsers;

    private boolean mDebug=false;

  public static void main( String args[]) {

    System.out.println( getLicensingString());    

    ShareDrawServerControl control = launch();
    control.setVisible(true);
    System.out.print( "server control started");
  }

    public void setDebug(boolean pDebug)
    {
	mDebug = pDebug;
    }

    public static ShareDrawServerControl launch()
    {
	    ShareDrawServerMethods server = null;
	    try {
		server = new ShareDrawServer();
	    } catch (Exception e) {
		System.err.println("ComputeEngine exception: " +
				   e.getMessage());
		e.printStackTrace();
	    }

	    /*
	      ShareDrawClient client1;
	    */
	    ShareDrawServerControl control = new ShareDrawServerControl();
	    control.setServer( (ShareDrawServer) server);

	    return control;
    }
    
    /**
     * WARNING UNDER GPL LICENSE TERMS
     * THIS MESSAGE SHOULD APPEARS AND NOT BE MODIFIED UNDER ANY OTHER LICENSE
     */
    public static String getLicensingString()
    {
	return 
	    "ShareDrawWeb is free software; you can redistribute it and/or modify\n"
	    +"it under the terms of the GNU General Public License as published by\n"
	    +"the Free Software Foundation; either version 2 of the License, or\n"
	    +"any later version.\n"
	    +"\n"
	    +"ShareDrawWeb is distributed in the hope that it will be useful,\n"
	    +"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	    +"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	    +"GNU General Public License for more details.\n"
	    +"\n"
	    +"You should have received a copy of the GNU General Public License\n"
	    +"along with ShareDrawWeb; if not, write to the Free Software\n"
	    +"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n";

    }

    public void run() {
		ServerSocket serversocket = null;  	
		try 
		{
			serversocket = new ServerSocket(0);
			System.out.println( "listen on " + serversocket.getLocalPort());
			while ( true ) {
			    new ShareDrawServerKompressedAccess( this, serversocket.accept());
			}
	    }
		catch ( java.io.IOException io)
		{
			System.err.println( " error on server listenning ");
			io.printStackTrace();
		}		
		catch ( RemoteException re)
	    {
			re.printStackTrace();
	    }
		finally
		{
			if ( serversocket != null)
			{
				try {
					serversocket.close();
				}
				catch ( Exception any)
				{					
				}
			}

		}
    }

  public ShareDrawServer() throws lasnier.sharedraw.RemoteException {
    super();
    localUsers = new ArrayList<ShareDrawClientMethods>(2);
    localImage = new ShareDrawing();
    Thread server_thread = new Thread(this);
    server_thread.start();
  }

    /**
       restart with an empty image
    */
  public void clear()
  {
      localImage = new ShareDrawing();
      reset();
  }

  public void addLine( DrawingLine line) {
    /* line is cloned to not use same reference as caller
       once added (by a client) line can't be changed
    */
    try {
      localImage.addLine( (DrawingLine) line.clone());
    }
    catch( java.lang.CloneNotSupportedException exception) {
      return;
    }
    updateClients( line);
  }

  public void save( String ref) {
    try {
      FileOutputStream fi = new FileOutputStream( ref);
      saveLines( new DataOutputStream( fi));
    }
    catch( java.io.FileNotFoundException fnfe) {
    }
    catch ( java.io.IOException ioe) {
    }
  }

    public void exportIMA( String ref) {
	try 
	{
	    FileOutputStream fi = new FileOutputStream( ref);
	    DataOutputStream out = new DataOutputStream( fi);
	    IMAWriter writer = new IMAWriter(localImage.getLines());
	    writer.writeTo( out );
	    out.flush();
	    fi.close();
	}
	catch( java.io.FileNotFoundException fnfe) {
	}
	catch ( java.io.IOException ioe) {
	}
    }

  public void load( String ref) {
    try {
      FileInputStream fi = new FileInputStream( ref);
      loadLines( new DataInputStream( fi));
    }
    catch( java.io.FileNotFoundException fnfe) {
    }
    catch ( java.io.IOException ioe) {
    }
    reset();
  }

  public void saveKompressed( String ref) {
    try {
      FileOutputStream fi = new FileOutputStream( ref);
      localImage.saveLinesKompressed( new DataOutputStream( fi));
    }
    catch( java.io.FileNotFoundException fnfe) {
    }
    catch ( java.io.IOException ioe) {
    }
  }

  public void loadExpanded( String ref) {
    try {
      FileInputStream fi = new FileInputStream( ref);
      localImage = new ShareDrawing();
      localImage.loadLinesExpanded( new DataInputStream( fi));
    }
    catch( java.io.FileNotFoundException fnfe) {
    }
    catch ( java.io.IOException ioe) {
    }
    reset();
  }
  
  public void importImage( String pImportType, String ref)
  {
    try {
        FileInputStream fi = new FileInputStream( ref);
        localImage = new ShareDrawing();
        Importer importer = new IMAImporter(new DataInputStream( fi));
	importer.setDebug(mDebug);
        localImage.importImage( importer);
      }
      catch( java.io.FileNotFoundException fnfe) {
      }
      catch ( java.io.IOException ioe) {
      }
      reset();	  
  }

  /* save Image to destination */
  public void saveLines( DataOutputStream destination ) throws
    java.io.IOException {
    localImage.saveLines( destination);
  }

  /* load Image from source */
  public void loadLines( DataInputStream source) throws
    java.io.IOException {
    localImage = new ShareDrawing();
    localImage.loadLines(source);
  }

  /**
    reset every client
  */
  public void reset()
  {
    for (ShareDrawClientMethods user: localUsers)
    {
      try
      {
        user.reset();
      }
      catch( lasnier.sharedraw.RemoteException r)
      {
        System.err.println( "problem with user on client side" + r.toString());
      }
      resetClient( user);
    }
  }

  /**
    reset a client
  */
  public void resetClient( ShareDrawClientMethods user)
  {
    for ( DrawingLine line : localImage.getInternLines())
    {
		try
		{
		    user.addLine( line);
		}
		catch( lasnier.sharedraw.RemoteException r)
		{
		    System.err.println( "problem with user on client side" +
					r.toString());
		}
    }
  }

  private ShareDrawClient createClient()
  {
    ShareDrawClient client = null;
    try {
      client = new ShareDrawClient();
    }
    catch( lasnier.sharedraw.RemoteException r) {
      System.err.println( "rmi error at client creation " + r.toString());
      return null;
    }
    client.setServer( this);
    client.reset();
    localUsers.add( client);
    return client;
  }

  public void addRemoteClient( ShareDrawClientMethods remote) throws
    lasnier.sharedraw.RemoteException
  {
    System.out.println( "client : " + remote);
    localUsers.add( remote);
  }

  public ShareDrawClient addClient()
  {
    ShareDrawClient client = createClient();
    client.run();
    resetClient( client);
    return client;
  }

  private void deleteClient( ShareDrawClient client) {
    client.finish();
    localUsers.remove( client);
  }

  private void updateClients( DrawingLine line) {
    for (ShareDrawClientMethods user : localUsers) {
      try {
        user.addLine( line);
      }
      catch( lasnier.sharedraw.RemoteException r) {
        System.err.println( "problem with user on client side" +
           r.toString());
      }
    }
  }

  public void finish() {
    System.exit(0);
  }

  public DrawingLine getLine( int lineindex) throws
    RemoteException,
    ResetException,
    NoMoreLineException {
    return localImage.getLine( lineindex);
  }
}

