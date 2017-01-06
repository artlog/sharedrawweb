package org.artisanlogiciel.graphics;

import java.awt.Point;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;

import lasnier.sharedraw.ShareDrawingLine;

/**
 * Write a ShareDrawLine (using its internal Vector representation) into IMA format
 * this is with loss ( ima is a 255x255 point resolution )
 *
 * @author philippe
 *
 */
public class IMAWriter {

    private final ArrayList<ShareDrawingLine> mLines;

    private int mByteCount = 0;
    // need to compute bounding box and normalize to 255x255
    private Point mOrig = new Point(100000,100000);
    private Point mMax = new Point(-100000,-100000);
    double ratioX = .1;
    double ratioY = .1;
	
    public IMAWriter(ArrayList<ShareDrawingLine> pLines)
    {
	mLines = pLines;
	setup();
    }

    private void updateOrigMax(Point p)
    {
	double ox = mOrig.getX(), oy = mOrig.getY(), mx = mMax.getX(), my = mMax.getY();
	
	if (p.getX() < ox )
	    {
		ox = p.getX();
	    }
	if (p.getX() > mx)
	    {
		mx = p.getX();
	    }
	if (p.getY() < oy )
	    {
		oy = p.getY();
	    }
	if (p.getY() > my)
	    {
		my = p.getY();
	    }
	mOrig.setLocation(ox,oy);
	mMax.setLocation(mx,my);
	
    }

    private void setup()
    {
	int count = 0;
	for (ShareDrawingLine line : mLines)
	    {
		for (Point p : line.getPoints())
		    {
			updateOrigMax(p);
			count+=2;
		    }
		// beam on + off
		count+=4;
	    }
	mByteCount=count;
	if ( mMax.getX() != mOrig.getX())
	    {
		ratioX = 255 / (mMax.getX() - mOrig.getX());
	    }
	else
	    {
		ratioX = 1.;
	    }
	if ( mMax.getY() != mOrig.getY())
	    {
		ratioY = 255 / (mMax.getY() - mOrig.getY());
	    }
	else
	    {
		ratioY = 1.;
	    }

    }

    public void writeTo(DataOutputStream pData)
	throws IOException
    {
	int wrote = 0;
	// writeheader : number of points ...
	// little endian unsigned short.
	pData.writeByte(mByteCount%256);
	pData.writeByte(mByteCount/256);	    
	for (ShareDrawingLine line : mLines)
	    {
		// write ima line
		wrote += writeLine(pData, line.getPoints());
	    }
	    
    }

    void writeBeamOnOff(DataOutputStream pData,boolean pOn)
	throws IOException
    {
	pData.writeByte(0xff);
	pData.writeByte(pOn ? 0xff : 0xfe);
    }

    void writePoint(DataOutputStream pData,Point pPoint)
	throws IOException
    {
	int x = (int) ((pPoint.getX()-mOrig.getX()) * ratioX);
	int y = 255 - ((int) ((pPoint.getY()-mOrig.getY()) * ratioY));
	if ( x == 0xff )
	    {
		x=0xfe;
	    }
	pData.writeByte(x);
	pData.writeByte(y);
    }
    
    int writeLine(DataOutputStream pData,ArrayList<Point> pPoints)	
	throws IOException
    {
	int count = 0;

	for (Point p : pPoints)
	    {
		writePoint(pData,p);
		if ( count == 0 )
		    {
			writeBeamOnOff(pData,true);
		    }		       
		count++;
	    }
	writeBeamOnOff(pData,false);
	return 2*(count +2);
    }
}
