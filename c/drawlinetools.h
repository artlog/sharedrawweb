#ifndef _DRAWLINETOOLS_H_
#define _DRAWLINETOOLS_H_

#include "drawlineexpander.h"
#include "sdlines.h"

int drawlineexpander_read_sdlines( struct drawlineexpander *this, struct inputstream * input,struct sdlines * sdlines);

#endif
