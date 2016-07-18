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
 Controls buttons 'Save' 'Quit' 'Load' 'Add'
and compressed file name
-awt only-

Delegates to ShareDrawServer
 */

package lasnier.sharedraw;

import java.awt.event.*;
import java.awt.*;

public class ShareDrawServerControl
    extends Frame
    implements ItemListener
{

    /**
	 * 
	 */
	private static final long serialVersionUID = 8611062519987592917L;
	
	final static String DEFAULT_FILE="test.img";
    final static String DEFAULT_COMPRESSED_FILE="compress.imc";
    
    ShareDrawServer thisServer = null;
    TextField mImageNameTF = null;

  public ShareDrawServerControl()
  {
    super();
    create();
  }

  public void itemStateChanged(java.awt.event.ItemEvent event) {
  }

  void create() {
    setLayout( new FlowLayout());
    setSize( 300, 300);
    mImageNameTF = new TextField(DEFAULT_COMPRESSED_FILE, 30);
    Button buttonSave = new Button( "Save");
    buttonSave.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          thisServer.save(DEFAULT_FILE);
          thisServer.saveKompressed(mImageNameTF.getText());
        }
      }
    );
    Button buttonQuit = new Button( "Quit");
    buttonQuit.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          thisServer.finish();
        }
      }
    );
    Button buttonLoad = new Button( "Load");
    buttonLoad.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          // thisServer.load(DEFAULT_FILE);
	    thisServer.loadExpanded(mImageNameTF.getText());
        }
      }
    );
    Button buttonImport = new Button( "Import");
    buttonImport.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
	    thisServer.importImage("IMG",mImageNameTF.getText());
        }
      }
    );
    Button buttonAdd = new Button( "Add");
    buttonAdd.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          thisServer.addClient();
        }
      }
    );
    Button buttonClear = new Button( "Clear");
    buttonClear.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          thisServer.clear();
        }
      }
    );

    add( mImageNameTF);
    add( buttonSave);
    add( buttonQuit);
    add( buttonLoad);
    add( buttonImport);
    add( buttonAdd);
    add( buttonClear);
  }

  public void setServer( ShareDrawServer server)
  {
    thisServer = server;
  }
   
    public ShareDrawServer getServer()
    {
	return thisServer;
    }
}




