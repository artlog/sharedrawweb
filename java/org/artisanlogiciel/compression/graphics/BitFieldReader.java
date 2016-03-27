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

package org.artisanlogiciel.compression.graphics;

/*
 simple class to read bitfields
 @see BitFieldWriter
 @author Philippe Lhardy
*/

import java.io.*;

class BitFieldReader {

   // size of nextWord
   final int dataSize = 32;

   /* offset of next bit to be read in current word
      should ensure bitOffset < dataSize
    */
   private int bitOffset = 0;

   /* word used waiting to be fully read
      dataSize is bit size of currentWord
      currentWord is part of word read that begins at bitOffset.
    */
   private int currentWord = 0;

   private DataInputStream inputStream;

   /* value returned is an int between 0 and 2^bits-1 */
   public int read( int bits) throws java.io.IOException {
     int field = 0;
     int head = 0;

     // a new word is needed
     if ( bitOffset == 0 ) {
       nextWord();
     }
     // if more bits needed that word currently used.
     if ( bits  + bitOffset > dataSize) {
       // warning recursive call ( not terminal )
       int bitsize = dataSize - bitOffset;
       head = read( dataSize - bitOffset);
       // current word had entirely been read, need a new one
       // don't do that... nextWord() will be done by next read...
       field = read( bits - bitsize);
       // reconstruct all
       // more significant bits in first word, least in last
       field = field | ( head << (bits - bitsize));
     }
     else {
       // terminal part do the job
       if ( bits == dataSize) {
         // special case to keep sign
         field = currentWord;
         currentWord = 0;
         bitOffset = 0;
       }
       else {       
         field = ( currentWord >> ( dataSize - bits) )
               & ( 0x7fffffff >> ( dataSize - 1 - bits) );
         // bits are read then remove them from currentWord
         currentWord <<= bits;
         bitOffset = ( bitOffset + bits ) % dataSize;
         /*
         System.out.println( "size " + Integer.toString(bits)
           + " value " + Integer.toString(field)
           + " offset " + Integer.toString(bitOffset)
           + " currentWord " + Integer.toString( currentWord)
         );
         */
       }
     }
    

     return field;  
  } 


    private int getInt( byte[] pBytes)
    {
	long field = 0;
	field = pBytes[0]<<24 | pBytes[1]<<16 | pBytes[2]<<8 | pBytes[3];
	return (int) field;
    }


    /**
     * doesn't work correctly as readInt
     */
    private int readInt()
    throws IOException
    {
       byte bytes[]=new byte[4];
       int nr=0;
       int index = 0;
       /* read at least 4 bytes */
       while ( ( nr != -1) && ( index < 4 ) )
	   {
	       nr = inputStream.read( bytes, index, 4 - index);
	       index +=nr;
	   }
       return getInt( bytes);
    }
    

   private void nextWord() throws java.io.IOException {
     // read next word
       //  currentWord = readInt();
       currentWord= inputStream.readInt();
     bitOffset = 0;
   }

   /*
     go to next entire word.
    */
   public void padToWord() throws java.io.IOException {
     if ( bitOffset != 0) {
       nextWord();
     }
   }

   /* set InputStream */
   public void setInputStream( InputStream input) {
     inputStream = new DataInputStream(input);
   }

}
