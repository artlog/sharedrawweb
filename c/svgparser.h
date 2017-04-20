
struct svgparser_matrix6 {
  float a;
  float b;
  float c;
  float d;
  float e;
  float f;
};

struct svg_transform_matrix {
  float coeff[6];
};

struct svg_rect {
  char * id;
  float width;
  float height;
  float x;
  float y;
  float ry;
  struct svg_transform_matrix * transform;
  char * path; // ugly but quick shortcut
};

struct svgparser_sp {
  float x;
  float y;
};
  
struct svgparser_rect4 {
  struct svgparser_sp point[4];
};

struct svgparser_rect_context
{
  int max_rects;
  int rects;
  // value to be malloc at svgparser_init_context
  struct svg_rect rect[1];
};

struct svgparser_rect_context * svgparser_init_context(int rects);
