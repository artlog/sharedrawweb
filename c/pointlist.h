#ifndef __POINTLIST_HEADER__
#define __POINTLIST_HEADER__

#define POINTSBYRECORD 65535

struct pointlist {
  struct sdpoint point[POINTSBYRECORD];
  int count;
};

struct sdadapter {
  int cx;
  int cy;
  int width;
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

void pointlist_init(struct pointlist * this);

void pointlist_add(struct pointlist * this,struct sdpoint * point);

struct sdpoint * pointlist_getlast(struct pointlist * pointlist);

void pointlist_dump(struct pointlist * this);

void pointlist_update_min_max(struct pointlist * this, struct sdpoint * min, struct sdpoint * max);

void pointlist_foreach(struct pointlist * this,struct sdadapter * adapter);

#endif
