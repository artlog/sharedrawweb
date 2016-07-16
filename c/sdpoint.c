#include <stdlib.h>
#include <stdio.h>
#include "sdpoint.h"

struct sdpoint * new_sdpoint()
{
  return malloc(sizeof(struct sdpoint));
}

void sdpoint_set( struct sdpoint* this, int x, int y)
{
  this->x=x;
  this->y=y;
}

void sdpoint_translate(struct sdpoint* this, int x, int y)
{
  this->x = this->x + x;
  this->y = this->y + y;
}

void sdpoint_free(struct sdpoint* this)
{
  free(this);
}

void sdpoint_dump(struct sdpoint * point, char * prefix)
{
  printf("%s(%i,%i)\n",prefix, point->x, point->y);
}
