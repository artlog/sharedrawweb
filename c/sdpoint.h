#ifndef __SDPOINT_HEADER__
#define __SDPOINT_HEADER__
struct sdpoint
{
  int x;
  int y;
};

struct sdpoint * new_sdpoint();

void sdpoint_set( struct sdpoint* point, int x, int y);

void sdpoint_translate( struct sdpoint* point, int x, int y);

void sdpoint_dump( struct sdpoint * point, char* prefix);

#endif
