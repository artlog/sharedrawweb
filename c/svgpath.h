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

#define SVGPATH_LINE_POINTS 8

struct svgpath_line {
  int points;
  struct svgpoint point[SVGPATH_LINE_POINTS]; // could make it bigger, but would impact svgpath_element union memory allocation waste
};

struct svgpath_element {
  char mode; // 'c' or 'C' for bezier curve, anything else is a line; see is_bezier
  union {
    struct bezier_cubic bezier;
    struct svgpath_line line;
  };
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
  struct svgpath_line line;
  char mode;
};

int svgpath_parse(struct svgpath * svgpath, char * path, int length, struct svgpoint * start);

int pathparser_init(struct pathparser * pathparser, char * path, int length, struct svgpoint * start);

int pathparser_nexttoken(struct pathparser * pathparser, struct svgpath * svgpath);

/**
 returns 1 whether svgpath_element is a bezier_cubic element or 0 if not
*/
int is_bezier(struct svgpath_element * svgpath_element);

#endif
