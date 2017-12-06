package org.artisanlogiciel.graphics;

import java.awt.Point;
import java.io.DataOutputStream;
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
	
	public void writeTo(DataOutputStream pData)
	throws IOException
	{
		pData.writeChars("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
		pData.writeChars("<svg>");
		for (DrawingLine line : mLines)
		{
			pData.writeChars("<g><path ");
			pData.writeChars("style=\"fill:none;fill-rule:evenodd;stroke:#000000;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\" d=\"");
			pData.writeChars( getSvgPathString(line.getPoints()));
			pData.writeChars("\" /></g>");
		}
		pData.writeChars("</svg>");
	}
	
	String getSvgPathString(ArrayList<Point> pPoints)
	{		
		StringBuilder sb = new StringBuilder(pPoints.size() * 10);				
		Point previous = null;
		for ( Point p : pPoints)
		{
			if ( previous != null )
			{
				sb.append(" c "  +  (p.getX() - previous.getX()) + "," + (p.getY() - previous.getY()) );
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
