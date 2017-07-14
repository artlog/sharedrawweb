#ifndef __POINTLIST_HEADER__
#define __POINTLIST_HEADER__

struct pointarray {
  // allocated size of pointarray
  int count;
  // will grow dynamically
  struct sdpoint pointarray[1];
};

struct pointlist {
  // current count ( == next index ) in pointarray
  int count;
  struct pointarray * array;
};

struct sdadapter {
  // center.x
  int cx;
  // center.y
  int cy;
  // width divider ( ie vector [0..1] <=> point [0..witdh] )
  int width;
  // height divider
  int height;
  int count;
  // run once before looping on each element (header) can be NULL
  void (*f_before) (struct pointlist * this, struct sdadapter * adapter, int count);
  // run for each element (content) SHOULD be set (ie non NULL)
  void (*f_for_each) (struct pointlist * this, struct sdpoint * point, struct sdadapter * adapter);
  // run after for each (footer/collector) can be NULL
  void (*f_after) (struct pointlist * this, struct sdadapter * adapter);
  void * data;  
};

struct sdpointsum {
  long x;
  long y;
  int count;
  int weight;
};

struct pointlist * new_pointlist();

/** use pointlist_release to free internals */
void pointlist_init(struct pointlist * this, int initialsize);

/** reverse of init */
void pointlist_release(struct pointlist * this);
  
/** return address of internal sdpoint copied from point 
return NULL on error
*/
struct sdpoint * pointlist_add(struct pointlist * this,struct sdpoint * point);

struct sdpoint * pointlist_getlast(struct pointlist * pointlist);

void pointlist_dump(struct pointlist * this);

void pointlist_update_min_max(struct pointlist * this, struct sdpoint * min, struct sdpoint * max);

void pointlist_foreach(struct pointlist * this,struct sdadapter * adapter);

#endif
