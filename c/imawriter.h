#ifndef _IMAWRITER_H_
#define _IMAWRITER_H_

#include "imacommon.h"
#include "sdlines.h"
#include "outputstream.h"

int write_ima(struct outputstream * stream, struct sdlines * lines);

#endif
