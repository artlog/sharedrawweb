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

import java.util.Vector;
// import javax.swing.*;

public class ShareDrawServerControl extends Frame implements ItemListener {

  ShareDrawServer thisServer = null;

  public ShareDrawServerControl() {
    super();
    create();
  }

  public void itemStateChanged(java.awt.event.ItemEvent event) {
  }

  void create() {
    setLayout( new FlowLayout());
    setSize( 300, 300);
    Button buttonSave = new Button( "save");
    buttonSave.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          thisServer.save("test.img");
          thisServer.saveKompressed("compress.imc");
        }
      }
    );
    Button buttonQuit = new Button( "quit");
    buttonQuit.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          thisServer.finish();
        }
      }
    );
    Button buttonLoad = new Button( "load");
    buttonLoad.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          // thisServer.load("test.img");
          thisServer.loadExpanded("compress.imc");
        }
      }
    );
    Button buttonAdd = new Button( "add");
    buttonAdd.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          thisServer.addClient();
        }
      }
    );
    add( buttonSave);
    add( buttonQuit);
    add( buttonLoad);
    add( buttonAdd);
  }

  public void setServer( ShareDrawServer server) {
    thisServer = server;
  }
}




