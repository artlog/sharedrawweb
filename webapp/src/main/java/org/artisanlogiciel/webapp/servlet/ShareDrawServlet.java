package org.artisanlogiciel.webapp.servlet;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
import javax.servlet.ServletOutputStream;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

@WebServlet(
        name = "ShareDraw",
        urlPatterns = {"/tests/images/newimage.imc"}
    )
public class ShareDrawServlet extends HttpServlet {

    private lasnier.sharedraw.ShareDrawServerControl mControl = null;


    public ShareDrawServlet()
    {
	super();

	final lasnier.sharedraw.ShareDrawServerControl control = lasnier.sharedraw.ShareDrawServer.launch();
	mControl = control;

	Thread other = new Thread() {
		public void run()
		{
		    control.show();
		    System.out.print( "server control started");
		}
	    };

	other.start();

    }
    /*
    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp)
            throws ServletException, IOException {
        ServletOutputStream out = resp.getOutputStream();
        out.write("hello".getBytes());
        out.flush();
        out.close();
    }
    */

    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse resp)
            throws ServletException, IOException {
	int readen = 0;
	ServletInputStream in = req.getInputStream();
	ServletOutputStream out = resp.getOutputStream();
	lasnier.sharedraw.ShareDrawingLine line = new lasnier.sharedraw.ShareDrawingLine();
	line.loadExpanded(in);
	System.out.println("\n *** test " + mControl);
	if ( mControl != null )
	    {
		mControl.getServer().addLine(line);
		out.write("line".getBytes());
	    }
	else
	    {
		out.write("ShareDrawingControler not set".getBytes());
	    }
        out.flush();
        out.close();
    }

}
