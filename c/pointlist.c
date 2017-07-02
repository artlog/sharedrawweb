#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sdpoint.h"
#include "pointlist.h"

struct pointlist * new_pointlist()
{
  return malloc(sizeof(struct pointlist));
}

struct pointarray * pointarray_new(int initialsize )
{
  struct pointarray * newarray = malloc( sizeof( struct pointarray) + sizeof( struct sdpoint) * (initialsize -1) );
  return newarray;
}

void pointlist_init(struct pointlist * this, int initialsize)
{
  struct pointarray * newarray = pointarray_new(initialsize);
  this->count=0;
  this->array=newarray;
  this->array->count=initialsize;
}

struct sdpoint * pointarray_add(struct pointarray * this,struct sdpoint * point, int index)
{
  struct sdpoint * added = NULL;
  if ( index < 0 )
    {
      fprintf(stderr,"[FATAL] access to negative index of point pointarray %p index %i\n",this, index);      
    }
  if (index < this->count )
    {
      added=&(this->pointarray[index]);      
      memcpy( added, point, sizeof(struct sdpoint));
    }
  return added;
}

struct sdpoint * pointlist_add(struct pointlist * this,struct sdpoint * point)
{
  struct pointarray * pointarray = this->array;
  struct sdpoint * added = NULL;

  if ( this->count == pointarray->count )
    {
      // double internal array
      printf("DOUBLE %p pointarray %p  %i\n",this, pointarray,  pointarray->count);
      struct pointarray * newarray = pointarray_new(pointarray->count *2);
      if ( newarray == NULL )
	{
	  fprintf(stderr,"[ERROR] record point count exceed maximum size %u", this->count);
	  return NULL;
	}
      else
	{
	  memcpy( newarray, pointarray, sizeof( struct pointarray) + sizeof( struct sdpoint) * ( pointarray->count - 1));
	  newarray->count = pointarray->count *2;
	  free( pointarray);
	  this->array = newarray;
	  pointarray = newarray;
	}
    }
  else
    if ( this->count > pointarray->count )
      {
	fprintf(stderr,"[FATAL] record point count exceed maximum index %u > %u (coding error or memory corruption)", this->count, pointarray->count);
	return NULL;
      }
      
  added=pointarray_add(pointarray, point, this->count);
  if ( added != NULL )
    {
      ++ this->count;
    }
  return added;
}

struct sdpoint * pointlist_getlast(struct pointlist * this)
{
  if ( (this->count > 0) && (this->count <= this->array->count ) )
    {
      return &this->array->pointarray[this->count - 1];
    }      
  return NULL;
}

void pointlist_dump(struct pointlist * this)
{
  struct sdpoint * pointarray = this->array->pointarray;
  printf("int[%u][2]={\n",this->count);
  for(int i =0; i< this->count; i++)
    {
      
      printf("{%i,%i},\n", pointarray[i].x, pointarray[i].y);
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
      update_min_max( min, max, &this->array->pointarray[i]);
    }  
}

void pointlist_foreach(struct pointlist * this, struct sdadapter * adapter)
{
  struct sdpoint * pointarray = this->array->pointarray;
  if ( adapter->f_before != NULL ) {
    (*adapter->f_before)( this, adapter, this->count);
  }
  for(int i =0; i< this->count; i++)
    {
      (*adapter->f_for_each)( this, &pointarray[i], adapter);
    }
  if ( adapter->f_after != NULL ) {
    (*adapter->f_after)( this, adapter);
  }
}

void pointlist_release(struct pointlist * this)
{
  if (this->array != NULL )
    {
      free(this->array);
      this->array = NULL;
      this->count = 0;
    }      
}
