package org.artisanlogiciel.graphics;

import java.awt.Point;
//import java.io.DataOutputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.util.ArrayList;

/**
 * Write a DrawLine (using its internal Vector representation) into svg
 * 
 * @author philippe
 *
 */
public class SvgWriter {

	private final ArrayList<DrawingLine> mLines;
	
	public SvgWriter(ArrayList<DrawingLine> pLines)
	{
		mLines = pLines;
	}
	
	public void writeTo(OutputStream pData)
	throws IOException
	{
	    pData.write("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>".getBytes("UTF8"));
	    pData.write("<svg>".getBytes("UTF8"));
		for (DrawingLine line : mLines)
		{
			pData.write("<g><path ".getBytes("UTF8"));
			pData.write("style=\"fill:none;fill-rule:evenodd;stroke:#000000;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\" d=\"".getBytes("UTF8"));
			pData.write( getSvgPathString(line.getPoints()).getBytes("UTF8"));
			pData.write(" \" /></g>".getBytes("UTF8"));
		}
		pData.write("</svg>".getBytes("UTF8"));
	}
	
	String getSvgPathString(ArrayList<Point> pPoints)
	{		
		StringBuilder sb = new StringBuilder(pPoints.size() * 10);				
		Point second = null;
		Point previous = null;
		for ( Point p : pPoints)
		{
		    if ( previous != null )
			{
			     if ( second == null )
				 {
				     second = p;
				     // 'l' a line ( 'c' woudl be a curve )
				     sb.append(" l ");
				 }
			     sb.append( ""  +  (p.getX() - previous.getX()) + "," + (p.getY() - previous.getY()) );
			     previous = p;
			}
			else
			{
			    sb.append("m " + p.getX() + "," + p.getY());
			    previous = p;
			}
		}
		return  sb.toString();
	}
}
