#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "drawlineexpander.h"

struct vectlist {
  struct vectlist * next;
  int index;
  float vector[1][3];
};

struct sdlines {
  int lines;  
  struct vectlist * first;
  struct vectlist * last;
};

struct vectlist* vectlist_new(int count)
{
  struct vectlist *vect;
  vect=malloc(sizeof(struct vectlist) + (count-1) * sizeof(vect->vector));
  if ( vect != NULL)
    {
      vect->index=0;
      vect->next=NULL;
    }
}
  
void setup_adapter(struct pointlist * this, struct sdadapter * adapter, int count)
{
  if (adapter == NULL)
    {
      fprintf(stderr,"adapter is NULL\n");
      return;
    }
  struct sdlines * lines = adapter->data;
  if ( lines == NULL)
    {
      fprintf(stderr, "data is NULL should have been created before");
      return;
    }
    {
      struct vectlist * vect = vectlist_new(count);
      if ( lines->last == NULL )
	{
	  lines->first = vect;
	  lines->last = vect;
	  lines->lines=1;
	}
      else
	{
	  lines->last->next = vect;
	  lines->last=vect;
	  lines->lines++;
	}
    }
}

void close_adapter(struct pointlist * this, struct sdadapter * adapter)
{
}

void dump_sdlines(struct sdlines * lines, char * varname)
{
  struct vectlist * vect = lines->first;
  for (int i=0; (i < lines->lines) && (vect != NULL); i++)
    {
      float* v;  
      printf("float %s_l%i[%i][2]={",varname,i,vect->index);
      v=&vect->vector[0][0];
      printf("{%f,%f}",v[0],v[1]);
      for ( int j=1; j<vect->index;j++)
	{
	  v=&vect->vector[j][0];
	  printf(",\n{%f,%f}",v[0],v[1]);
	}
      printf("};\n");
      vect=vect->next;
      if ( vect !=NULL)
	{
	  printf("\n");
	}
    } 
  vect=lines->first;
  printf("struct sdlines { int points; float (*vector)[2];};\n");
  printf("struct sdlines %s[%i]={\n",varname, lines->lines);
  for (int i=0; (i < lines->lines) && (vect != NULL); i++)
    {      
      printf("{.points=%i,.vector=%s_l%i}",vect->index,varname,i);
      vect=vect->next;
      if ( vect != NULL)
	{
	  printf(",\n");
	}
    }
  if ( vect != NULL )
    {
      printf("??? points=%i ????",vect->index);
    }
  printf("};\n");
}

void set_vector( float v[3], struct sdpoint * point, struct sdadapter * adapter)
{    
  v[0] = (float) (point[0].x - adapter->cx) / adapter->width;
  v[1] = (float) (point[0].y - adapter->cy) / adapter->height;
  v[2]=.0;
}

void adapt_point(struct pointlist * this, struct sdpoint * point, struct sdadapter * adapter)
{
  struct sdlines * lines = adapter->data;
  struct vectlist *vect = lines->last;
  set_vector( &vect->vector[vect->index][0], point, adapter);
  vect->index++;
}

int main(int argc, char ** argv)
{ 
  if ( argc > 1)
    {
      struct drawlineexpander expander;
      struct inputstream input;
      struct sdpoint min,max;
      struct sdlines sdlines={
	.lines=0,
	.first=NULL,	
	.last=NULL
      };
      struct sdadapter adapter={
	.cx=0,
	.cy=0,
	.width=600,
	.height=600,
	.f_before=setup_adapter,
	.f_for_each=adapt_point,
	.f_after=close_adapter,
	.data=&sdlines
      };
      char* varname;

      if ( argc > 2 )
	{
	  varname=argv[2];
	}
      else
	{
	  varname="default";
	}
      int fd = open( argv[1], 0);
      if ( fd != - 1 )
	{
	  inputstream_init(&input, fd);
	  int lines = inputstream_readuint32(&input);
	  if (lines < 10000)
	    {
	      for (int i=0; i< lines; i++)
		{
		  // fprintf(stderr, "Line %u/%u\n", (i+1),lines);
		  drawlineexpander_init(&expander);
		  // expander.debug=1;
		  drawlineexpander_expand(&expander, &input);
		  // pointlist_dump(expander.expandedLines);
		  pointlist_update_min_max(expander.expandedLines,&min,&max);
		  pointlist_foreach(expander.expandedLines, &adapter);
		}
	      {
		dump_sdlines(&sdlines,varname);
	      }
	      sdpoint_dump(&max,"// max");
	      sdpoint_dump(&min,"// min");
	    }
	  else
	    {
	      fprintf(stderr, "Too many lines %xh", lines);
	    }
	  close(fd);
	}
    }
}
