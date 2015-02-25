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

package lasnier.sharedraw;

/*
  simple class to write bitfields
  @see BitFieldReader
  @author Philippe Lhardy
*/

import java.io.*;

class BitFieldWriter {

   // size of nextWord
   final int dataSize = 32;

   /* bitOffset is number of bits already used in word
      should ensure bitOffset < dataSize
    */
   private int bitOffset;

   /* word used waiting to be filled to be written
      dataSize is bit size of nextWord
    */
   private int nextWord;

   private DataOutputStream outputStream;

   public void write( int field, int bits) throws java.io.IOException {
     // if more bits needed that word currently used.
     if ( bits  + bitOffset > dataSize) {
       // warning recursive call ( not terminal )
       // fill and write current word
       int bitsize = dataSize - bitOffset;
       int head;
       // most significant bits first
       // suppress at left least significant bits
       head = field >> (bits - bitsize);
       write( head, dataSize - bitOffset);
       // no need of newWord() previous write already done it
       // least significant bits last
       field = field & ( 0x7FFFFFFF >> ( bitsize - 1));
       write( field, bits - bitsize);
     }
     else {         
       if ( bits == dataSize ) {
         // special case to keep sign
         nextWord = field; 
         bitOffset = 0;
       }
       else {
         // keep only meaning part of field
         field = field & ( 0x7FFFFFFF >> ( dataSize - 1 - bits));
         nextWord = nextWord | ( field << ( dataSize - bits - bitOffset));
         bitOffset = ( bitOffset + bits ) % dataSize; 
       }       
       // a new word is needed
       if ( bitOffset == 0 ) {
         newWord();
       }
     }
     /*
     System.out.println( "nextWord " + Integer.toString( nextWord) + " size "
        + Integer.toString( bits) + " field " + Integer.toString( field));
     */
   }

    /**
     * seems to works correctly as DataOutputStream.writeInt()
     */
    private void writeInt( int i)
    throws IOException
    {
	long j = i;
	byte bytes[]=new byte[4];
	bytes[0] = (byte) ((j & 0xFF000000) >> 24);
	bytes[1] = (byte) ((j & 0x00FF0000) >> 16);
	bytes[2] = (byte) ((j & 0x0000FF00) >> 8);
	bytes[3] = (byte) (j & 0x000000FF);
	outputStream.write( bytes, 0, 4);
    }

   private void newWord() throws java.io.IOException {
     // do really write current word;
       //writeInt( nextWord);
     outputStream.writeInt(nextWord);
     // reset nextWord
     bitOffset = 0;
     nextWord = 0;
   }

   /*
    write all current datas if any
    to have a clean word ready.
    */
   public void padToWord() throws java.io.IOException {
     if ( bitOffset != 0) {
       newWord();
     }
   }

   /* set OutputStream */
   public void setOutputStream( OutputStream output) {
     outputStream = new DataOutputStream(output);
   }

}
