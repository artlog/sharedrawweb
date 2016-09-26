#ifndef __IMAREADER_HEADER__
#define __IMAREADER_HEADER__

#include "sdlines.h"
#include "inputstream.h"

int filename_is_ima(char * filename);

int read_ima(struct inputstream * stream, struct sdlines * lines);

#endif
