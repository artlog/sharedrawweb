#ifndef __SVGPATH_HEADER__
#define __SVGPATH_HEADER__

#include <allist.h>

struct svgpoint {
  double x;
  double y;
};

struct svgpath {
  // a list of basic path, every path is a membership.
  // membership 0 hardcoded for bezier elements.
  struct allistcontext * svgpathcontext;  
};

struct bezier_cubic {
  struct svgpoint point[4];
};
  
struct pathparser {
  char * start;
  int index;
  int length;
  int absolute;
  int relative;
  int bezier;
  struct svgpoint lastpoint;
  struct bezier_cubic bezier_cubic;
};

int svgpath_parse(struct svgpath * svgpath, char * path, int length, struct svgpoint * start);

int pathparser_init(struct pathparser * pathparser, char * path, int length, struct svgpoint * start);

int pathparser_nexttoken(struct pathparser * pathparser, struct svgpath * svgpath);
  
#endif
