#ifndef __IMAREADER_HEADER__
#define __IMAREADER_HEADER__

#include "sdlines.h"
#include "alinput.h"
#include "imacommon.h"

int filename_is_ima(char * filename);

int read_ima(struct alinputstream * stream, struct sdlines * lines);

#endif
