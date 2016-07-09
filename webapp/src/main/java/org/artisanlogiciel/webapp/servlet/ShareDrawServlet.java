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
	byte buffer[] = new byte[4096];
	int readen = 0;
	ServletInputStream in = req.getInputStream();
	while ( ( readen = in.read(buffer,0,4096) ) != -1 )
	    {
		System.out.println("received " + readen);
	    };	
        ServletOutputStream out = resp.getOutputStream();
        out.write("NOT YET IMPLEMENTED".getBytes());
        out.flush();
        out.close();
    }

}
