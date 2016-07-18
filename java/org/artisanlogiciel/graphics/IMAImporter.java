package org.artisanlogiciel.graphics;

import java.awt.Point;
import java.io.DataInputStream;

import lasnier.sharedraw.ShareDrawing;
import lasnier.sharedraw.ShareDrawingLine;

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

	@Override
	public void importInto(ShareDrawing drawing)
	{
		int x,y;
		ShareDrawingLine line = null;
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
							line = new ShareDrawingLine();
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
