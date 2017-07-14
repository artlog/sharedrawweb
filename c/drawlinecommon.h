#ifndef _DRAWLINECOMMON_H_
#define _DRAWLINECOMMON_H_

#include "sdpoint.h"
#include "pointlist.h"

// maximum index of global_scode [0-4]
#define SCODE_MAX 4

// necessarily 0 ( would break code if not ).
#define SCODE_SAME 0
#define SCODE_PREVIOUS 0b10
#define SCODE_NEXT 0b11

// code for size encoding
extern int global_scode[];

#endif
