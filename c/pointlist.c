#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sdpoint.h"
#include "pointlist.h"

struct pointlist * new_pointlist()
{
  return malloc(sizeof(struct pointlist));
}

void pointlist_init(struct pointlist * this)
{
  this->count=0;  
}

void pointlist_add(struct pointlist * this,struct sdpoint * point)
{
  // really lazzy unchecked implementation
  if ( this->count < POINTSBYRECORD )
    {
      memcpy( &(this->point[this->count]), point, sizeof(struct sdpoint));
      this->count ++;
    }
  else
    {
      fprintf(stderr,"record point count exceed maximum size %u", this->count);
    }
}

struct sdpoint * pointlist_getlast(struct pointlist * this)
{
  if (this->count > 0)
    {
      return &this->point[this->count - 1];
    }      
  return NULL;
}

void pointlist_dump(struct pointlist * this)
{
  printf("int[%u][2]={\n",this->count);
  for(int i =0; i< this->count; i++)
    {
      printf("{%i,%i},\n", this->point[i].x, this->point[i].y);
    }
  printf("};\n");
}

// void collect(int index, struct sdpoint*, struct pointcollector *)
  
void update_min_max( struct sdpoint * min, struct sdpoint * max, struct sdpoint *point)
{
  if (min->x > point->x)
    {
      min->x=point->x;
    }
  if (min->y > point->y)
    {
      min->y=point->y;
    }
  if (max->x < point->x)
    {
      max->x=point->x;
    }
  if (max->y < point->y)
    {
      max->y=point->y;
    }
}

void update_sum( struct sdpointsum * pointsum, struct sdpoint * point)
{
  pointsum->x = pointsum->x + point->x;
  pointsum->y = pointsum->y + point->y;
  pointsum->count++;
}


void pointlist_update_min_max(struct pointlist * this, struct sdpoint * min, struct sdpoint * max)
{
  for(int i =0; i< this->count; i++)
    {
      update_min_max( min, max, &this->point[i]);
    }  
}

void pointlist_foreach(struct pointlist * this, struct sdadapter * adapter)
{
  if ( adapter->f_before != NULL ) {
    (*adapter->f_before)( this, adapter, this->count);
  }
  for(int i =0; i< this->count; i++)
    {
      (*adapter->f_for_each)( this, &this->point[i], adapter);
    }
  if ( adapter->f_after != NULL ) {
    (*adapter->f_after)( this, adapter);
  }
}
