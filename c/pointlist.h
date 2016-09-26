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
  void (*f_before) (struct pointlist * this, struct sdadapter * adapter, int count);
  void (*f_for_each) (struct pointlist * this, struct sdpoint * point, struct sdadapter * adapter);
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
