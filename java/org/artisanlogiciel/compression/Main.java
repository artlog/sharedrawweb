package org.artisanlogiciel.compression;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import org.artisanlogiciel.graphics.SvgWriter;
import org.artisanlogiciel.graphics.Drawing;

/**
 * Very simple utility to convert from .imc to svg.
 * 
 * TODO : could be more clever and not load full image into memory to save it afterwards ( for very small embedded systems ).
 */

public class Main
{

	Drawing mLocalImage;
	
	public static void main(String[] args) {
		Main m = new Main();
		if ( args.length > 1 )
		{
			m.loadExpanded(args[0]);
			try {
				File out = new File(args[1]);
				if (out.exists())
				{
					System.err.println("File already exists" + out + " Not overriding it");
					
				}
				else
				{
					m.saveSvg(new DataOutputStream(new FileOutputStream(out)));
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		else
		{
			System.out.println("Very simple utility to convert from .imc to svg.");
			System.out.println("first argument is compressed file, second  is svg output file (should  not already exist)");
		}

	}
	
	public void saveSvg(DataOutputStream s)
	throws IOException
	{
		SvgWriter writer = new SvgWriter(mLocalImage.getLines());
		writer.writeTo(s);
	}
	
	  public void saveKompressed( String ref) {
		    try {
		      FileOutputStream fi = new FileOutputStream( ref);
		      mLocalImage.saveLinesKompressed( new DataOutputStream( fi));
		    }
		    catch( java.io.FileNotFoundException fnfe) 
		    {
		    	fnfe.printStackTrace(System.err);
		    }
		    catch ( java.io.IOException ioe) {
		    	ioe.printStackTrace(System.err);
		    }		    
		  }

	  public void loadExpanded( String ref) {
	    try {
	      FileInputStream fi = new FileInputStream( ref);
	      mLocalImage = new Drawing();
	      mLocalImage.loadLinesExpanded( new DataInputStream( fi));
	    }
	    catch( java.io.FileNotFoundException fnfe) {
	    	fnfe.printStackTrace(System.err);
	    }
	    catch ( java.io.IOException ioe) {
	    	ioe.printStackTrace(System.err);
	    }
	  }

}
