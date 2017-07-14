#include "drawlinecommon.h"

/**
common constants
**/

// code for size encodin
  /* 11 : next, 10 : previous ; 0* : same
     6 -> 14->30
     ^        |
     |        v
     3   <-  64
  */
int global_scode[SCODE_MAX+1]={3,6,14,30,64};
