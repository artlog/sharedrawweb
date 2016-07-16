#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "fieldreader.h"

void inputstream_init(struct inputstream * stream, int fd)
{
  stream->fd=fd;
  stream->eof=0;
  stream->debug=0;
}

unsigned int inputstream_readuint32(struct inputstream * stream)
{
  unsigned char v[4];
  unsigned char result[4];
  size_t total = 0;
  size_t r = 0;

  // handle case where 4 bytes are read in multiple chunks (often network issues)
  while (total < 4)
    {      
      r = read(stream->fd, &v + total, 4 - total);
      // 4 bytes ints
      if ( r > 0 )
	{
	  total = total + r;
	}
      else
	{
	  stream->eof=1;
	  return 0;
	}
    }
  // reverse big endian => little endian ( internal intel int )
  result[0]=v[3];
  result[1]=v[2];
  result[2]=v[1];
  result[3]=v[0];
  if ( stream->debug )
    {
      fprintf(stderr,"%u %x %x %x %x\n",total, v[0], v[1], v[2], v[3]);
    }  
  return (*(unsigned int*) result);
}

struct bitfieldreader * new_fieldreader()
{
  return malloc(sizeof(struct bitfieldreader));
}

void fieldreader_init(struct bitfieldreader * this)
{
  // in bits size read
  this->dataSize=32;
}

void fieldreader_free(struct bitfieldreader * this)
{
  free(this);
}

void fieldreader_nextword(struct bitfieldreader * this)
{
  // read next word
  this->currentWord= inputstream_readuint32(this->stream);
  this->bitOffset = 0;
}

/*
  go to next entire word.
*/
void fieldreader_padtoword(struct bitfieldreader * this)
{
  if ( this->bitOffset != 0) {
    fieldreader_nextword(this);
  }
}

void fieldreader_setinput( struct bitfieldreader * this, struct inputstream * inputstream )
{
  this->stream=inputstream;
}

int fieldreader_read( struct bitfieldreader * this, int bits )
{
  int field = 0;
  int head = 0;

  // a new word is needed
  if ( this->bitOffset == 0 ) {
    if ( this->stream->eof )
      {
	return field;
      }
    fieldreader_nextword(this);    
  }
  // if more bits needed that word currently used.
  if ( ( bits  + this->bitOffset ) > this->dataSize)
    {
      // warning recursive call ( not terminal )
      int bitsize = this->dataSize - this->bitOffset;
      head = fieldreader_read( this, bitsize);
      // current word had entirely been read, need a new one
      // don't do that... nextword() will be done by next read...
      field = fieldreader_read( this, bits - bitsize);
      // reconstruct all
      // more significant bits in first word, least in last
      field = field | ( head << (bits - bitsize));
    }
  else {
    // terminal part do the job
    if ( bits == this->dataSize) {
      // special case to keep sign
      field = this->currentWord;
      this->currentWord = 0;
      this->bitOffset = 0;
    }
    else {       
      field = ( this->currentWord >> ( this->dataSize - bits) )
	& ( 0x7fffffff >> ( this->dataSize - 1 - bits) );
      // bits are read then remove them from currentWord
      this->currentWord <<= bits;
      this->bitOffset = ( this->bitOffset + bits ) % this->dataSize;
    }
  }  
  return field;  
}

