#ifndef _BITFIELDWRITER_H_
#define _BITFIELDWRITER_H_

#include "outputstream.h"

struct bitfieldwriter {
  struct outputstream *stream;
  int dataSize;
  int bitOffset;
  int nextWord;  
};

struct bitfieldwriter * new_bitfieldwriter();

void bitfieldwriter_init(struct bitfieldwriter * this);

void bitfieldwriter_setoutputstream( struct bitfieldwriter * this, struct outputstream * outputstream);

void bitfieldwriter_write( struct bitfieldwriter * this, int value, int bitsize);

void bitfieldwriter_padtoword( struct bitfieldwriter * this);

#endif
