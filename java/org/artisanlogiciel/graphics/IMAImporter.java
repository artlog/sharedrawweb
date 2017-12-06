package org.artisanlogiciel.graphics;

import java.awt.Point;
import java.io.DataInputStream;

/**
.IMA format is an old proprietary format for laser show program
it contain a list of point the laser beam should goe through,
x=255 has a special meaning to hide (255,254) ( send beam into the box) or show it (255,255): let is flow outside.
*/
public class IMAImporter implements Importer 
{
	
	final DataInputStream mStream;
	boolean mBeanOn = false;
	int mIndex =0;
	boolean mDebug = false;
	
	public IMAImporter(DataInputStream inputStream)
	{
		mStream=inputStream;
	}

    public void setDebug(boolean pDebug)
    {
	mDebug=pDebug;
    }

	@Override
	public void importInto(Drawing drawing)
	{
		int x,y;
		DrawingLine line = null;
		mBeanOn = false;
		try {
			// little endian unsigned short
			int pointCount = mStream.readUnsignedByte();
			pointCount = ( 256 * mStream.readUnsignedByte() ) + pointCount;
			if ( mDebug )
			{
				System.out.println("point count :" + pointCount);
			}
			for (int j = 0 ; j < pointCount; j+=2)
			{
				mIndex=j;
				x = mStream.readUnsignedByte();
				y = mStream.readUnsignedByte();
				// special beam on/off
				if ( ( x == 0xff ) && ( y >= 0xfe ) )
				{
					mBeanOn = ( y == 0xff);
					if ( mDebug )
					{
						System.out.println("beam change at " + mIndex + " " + mBeanOn);
					}
				}
				else
				{
					y = 255-y;
					if ( mDebug )
					{
						System.out.println("point at " + mIndex + " " + mBeanOn + " x "  + x + " y " + y);
					}					
					if ( mBeanOn )
					{
						if (line == null )
						{
							line = new DrawingLine();
						}
						line.addPoint(new Point(x,y));					
					}
					else
					{
						if ( line != null )
						{
							drawing.addLine(line);
							line = null;
						}
					}
				}
			}
			if ( line != null )
			{
				drawing.addLine(line);
				line = null;
			}
		}
		catch( Exception any)
		{
			System.err.println(" error at index " + mIndex);
			any.printStackTrace(System.err);
		}	
	}
}
