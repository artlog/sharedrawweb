#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdlines.h"
#include "alcommon.h"
#include "todo.h"

void set_vector( float v[3], struct sdpoint * point, struct sdadapter * adapter)
{
  if (adapter != NULL)
    {
      v[0] = (float) (point[0].x - adapter->cx) / adapter->width;
      v[1] = (float) (point[0].y - adapter->cy) / adapter->height;
      v[2]=.0;
    }
  else
    {
      v[0] = (float) point[0].x;
      v[1] = (float) point[0].y;
      v[2]=.0;
    }
}

struct vectlist* vectlist_new(int count)
{
  struct vectlist *vect;
  vect=malloc(sizeof(struct vectlist) + (count-1) * sizeof(vect->vector));  
  if ( vect != NULL)
    {
      vect->count=count;
      vect->index=0;
      vect->next=NULL;
      vect->flags=SDLINE_DYNAMIC|SDLINE_GROWABLE;
      vect->vectorp=&vect->vector[0][0];
    }
  return vect;
}

void vectlist_free(struct vectlist * vect)
{
  if ( vect != NULL )
    {
      if ( FLAG_IS_SET(vect->flags,SDLINE_DYNAMIC)  )
	{
	  // easy guard...
	  vect->index=0;
	  vect->flags=SDLINE_ROTTEN;
	  vect->vectorp=(float *) 0xdeadbeef;
	  if ( vect->next != NULL)
	    {
	      todo("vect_list_free support freing a list of vectlist");
	      // memory leak ...
	    }
	  vect->next=NULL;
	  free(vect);
	}
    }     
}

int vectlist_add_point(struct vectlist * vect, struct sdpoint * point)
{
  int index = 0;
  if ( FLAG_IS_SET(vect->flags,SDLINE_GROWABLE) )
    {
      if ( vect->index < vect->count )
	{
	  set_vector( vectlist_get(vect,vect->index), point, NULL);
	  ++vect->index;
	  index=vect->index;
	}
    }
    return index;
}


void sdlines_add_vectlist(struct sdlines * lines, struct vectlist * vect)
{
  if ( FLAG_IS_SET(lines->flags,SDLINE_CONCRETE)  )
    {
      // mixing concrete and growable...
      lines->flags|=SDLINE_MIXED;
    }
  if ( vect->next != NULL)
    {
      todo("sdlines_add_vectlist support inserting a full list of lines");
      // current implemetnation : break it appart.
      vectlist_free(vect->next);
      vect->next=NULL;
    }
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
      ++ lines->lines;
    }
}

struct dump_sdlines_data {
  char * varname;
};

float * vectlist_get(  struct vectlist * vectlist , int index)
{
  return &vectlist->vectorp[3*index]; 
}

void dump_sdlines_content(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  float* v;
  char * varname = ((struct dump_sdlines_data *) data)->varname;
  printf("float %s_l%i[%i][2]={",varname,index,line->index);
  v=vectlist_get(line,0);
  printf("{%f,%f}",v[0],v[1]);
  for ( int j=1; j<line->index;j++)
    {
      v=vectlist_get(line,j);
      printf(",\n{%f,%f}",v[0],v[1]);
    }
  printf("};\n");
  if ( next !=NULL)
    {
      printf("\n");
    }
}

void dump_sdlines_content_ref_header(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  char * varname = ((struct dump_sdlines_data *) data)->varname;
  printf("struct sdlines { int points; float (*vector)[2];};\n");
  printf("struct sdlines %s[%i]={\n",varname, this->lines);
}

void dump_sdlines_content_ref(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  char * varname = ((struct dump_sdlines_data *) data)->varname;
  printf("{.points=%i,.vector=%s_l%i}",line->index,varname,index);
  if ( next != NULL)
    {
      printf(",\n");
    }  
}

void dump_sdlines_content_ref_footer(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  printf("};\n");  
}


struct dump_xlines_data {
  char * varname;
  FILE * file;
};


void dump_xlines_content(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  float* v;
  struct dump_xlines_data * xlines_data = (struct dump_xlines_data *) data;
  char * varname = xlines_data->varname;
  FILE * f = xlines_data->file;

  fprintf(f,"XPoint %s_l%i[%i]={\n",varname,index,line->index);
  v=vectlist_get(line,0);
  fprintf(f,"{%i,%i}",(int) v[0],(int) v[1]);
  for ( int j=1; j<line->index;j++)
    {
      if ( j % 5 == 0 )
	{
	  fprintf(f,"\n");
	}
      v=vectlist_get(line,j);
      fprintf(f,",{%i,%i}",(int) v[0],(int) v[1]);
    }
  fprintf(f,"\n};\n");
  if ( next !=NULL)
    {
      fprintf(f,"\n");
    }
}

void dump_xlines_content_ref_header(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  struct dump_xlines_data * xlines_data = (struct dump_xlines_data *) data;
  char * varname = xlines_data->varname;
  FILE * f = xlines_data->file;
  
  fprintf(f,"\nstruct xlines { int points; XPoint *vector;};\n");
  fprintf(f,"\nstruct xlines %s[%i]={\n",varname, this->lines);

}

void dump_xlines_content_ref(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  struct dump_xlines_data * xlines_data = (struct dump_xlines_data *) data;
  char * varname = xlines_data->varname;
  FILE * f = xlines_data->file;

  fprintf(f,"{.points=%i,.vector=%s_l%i}",line->index,varname,index);
  if ( next != NULL)
    {
      fprintf(f,",\n");
    }

}

void dump_xlines_content_ref_footer(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  struct dump_xlines_data * xlines_data = (struct dump_xlines_data *) data;
  // char * varname = xlines_data->varname;
  FILE * f = xlines_data->file;

  fprintf(f,"};\n");  
}


void dump_generic_callbacks(struct sdlines * lines, struct sdlines_iterator_callback * dump_sdlines_callback_content, struct sdlines_iterator_callback * dump_sdlines_callback_content_ref, void * data)
{

  if ( dump_sdlines_callback_content != NULL )
    {
      sdlines_foreach(lines, dump_sdlines_callback_content, data);
    }

  if ( dump_sdlines_callback_content_ref != NULL )
    {
      sdlines_foreach(lines, dump_sdlines_callback_content_ref, data);
    }
}

struct sdlines_iterator_callback dump_sdlines_callback_content = {
  .f_before=NULL,
  .f_for_each=dump_sdlines_content,
  .f_after=NULL
};

struct sdlines_iterator_callback dump_sdlines_callback_content_ref = {
  .f_before=dump_sdlines_content_ref_header,
  .f_for_each=dump_sdlines_content_ref,
  .f_after=dump_sdlines_content_ref_footer
};

void dump_sdlines(struct sdlines * lines, char * varname)
{
  struct dump_sdlines_data  dump_sdlines_data;
  dump_sdlines_data.varname = varname;

  dump_generic_callbacks(lines,&dump_sdlines_callback_content,&dump_sdlines_callback_content_ref, (void *) &dump_sdlines_data);
}

struct sdlines_iterator_callback dump_xlines_callback_content = {
  .f_before=NULL,
  .f_for_each=dump_xlines_content,
  .f_after=NULL
};

struct sdlines_iterator_callback dump_xlines_callback_content_ref = {
  .f_before=dump_xlines_content_ref_header,
  .f_for_each=dump_xlines_content_ref,
  .f_after=dump_xlines_content_ref_footer
};

void dump_xlines(FILE * f, struct sdlines * lines, char * varname)
{
  struct dump_xlines_data  dump_xlines_data;
  dump_xlines_data.varname = varname;
  dump_xlines_data.file = f;

  dump_generic_callbacks(lines,&dump_xlines_callback_content,&dump_xlines_callback_content_ref, (void *) &dump_xlines_data);
}


int free_xlines(struct xlines ** built)
{
  if ( built != NULL )
    {
      if ( built[0] != NULL )
	{
	  free(built[0]);
	  built[0]=NULL;
	  return 0;
	}
    }
  return -1;
}
		

struct xlines * build_xlines(struct sdlines * lines, int start, int count, struct sdlines_matrix6 * matrix)
{
  struct xlines * xlines = NULL;  
  struct vectlist * vect = NULL;

  if ( lines != NULL )
    {
      vect=lines->first;
    }
  else
    {
      return NULL;
    }

  if ((start >= 0) && ( count > 0 ))
    {    
      xlines=calloc(count,sizeof(struct xlines));
      if (xlines != NULL )
	{
	  for (int i=0; (i < lines->lines) && (vect != NULL); i++)
	    {
	      if (( i >= start ) && ( i < start + count))
		{
		  float* v;
		  XPoint * points= calloc(vect->index, sizeof(XPoint));
		  if ( points != NULL )
		    {
		      for ( int j=0; j<vect->index;j++)
			{
			  v=&vect->vectorp[j*3];
			  if ( matrix == NULL )
			    {
			      points[j].x=(int)v[0];
			      points[j].y=(int)v[1];
			    }
			  else
			    {
			      sdlines_matrix6_apply_matrix(matrix,v,&points[j]);
			    }
			}
		      xlines[i-start].points=vect->index;
		      xlines[i-start].vector=points;
		      vect=vect->next;
		    }
		}
	    }      
	}
 
    }
  else
    {
      todo("build_xlines called with start or count <= 0");
    }
  return xlines; 
}

void sdlines_matrix6_apply_matrix(struct sdlines_matrix6 * matrix, float * v, XPoint * dest )
{
  dest->x=(matrix->a * v[0]) + (matrix->c * v[1]) + matrix->e;
  dest->y=(matrix->b * v[0]) + (matrix->d * v[1]) + matrix->f;
}


struct vectlist * sdlines_get_vectlist(struct sdlines * this, int i)
{
  struct vectlist * current = this->first;
  int j = 0;
  
  while ( ( j < i ) && ( current != NULL ) )
    {
      current = current->next;
      ++j;
    }
  return current;
}

struct vectlist * veclist_get_next(struct vectlist * this, int i)
{
  struct vectlist * current = this;
  int j = 0;

  if ( j > i )
    {
      return NULL;
    }
	
  while ( ( j < i ) && ( current != NULL ) )
    {
      current = current->next;
      ++j;
    }
  return current;  
}

struct pointlist * vectlist_to_pointlist(struct vectlist * this)
{
  struct pointlist * pointlist = new_pointlist();
  // we already know how many points to add
  pointlist_init(pointlist,this->index);
  for ( int i=0; i<this->index; i++)
    {
      struct sdpoint point;
      float * v = vectlist_get(this,i);
      point.x=(int) v[0];
      point.y=(int) v[1];
      if ( pointlist_add(pointlist, &point) == NULL )
	{
	  fprintf(stderr,"[ERROR] can't add a vectlist point into pointlist index %i\n", i);
	  pointlist_release(pointlist);
	  free(pointlist);
	  return NULL;
	}
    }
  return pointlist;
}


void sdlines_foreach(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  if (callback->f_before != NULL)
    {
      (*callback->f_before)(this,callback,data);
    }
  if (callback->f_for_each != NULL)
    {
      struct vectlist * vect = this->first;
      for (int i=0; (i < this->lines) && (vect != NULL); i++)
	{
	  struct vectlist * vectnext = vect->next;
	  // vect->next can be altered and vect can be freed, we still did capture next before
	  (*callback->f_for_each)(this,callback,vect,i,vectnext,data);
	  vect=vectnext;
	}
    }
  if (callback->f_after != NULL)
    {
      (*callback->f_after)(this,callback,data);
    }  
}

void vectlist_free_callback_function(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  vectlist_free(line);
}

struct sdlines_iterator_callback vectlist_free_callback = {
  .f_before=NULL,
  .f_for_each=vectlist_free_callback_function,
  .f_after=NULL,
};

  
void sdlines_free( struct sdlines * this)
{
  if ( FLAG_IS_SET(this->flags,SDLINE_CONCRETE) )
    {
      // mixed case, some lines were allocated dynamically
      if ( FLAG_IS_SET(this->flags,SDLINE_MIXED) )
	{
	  sdlines_foreach(this,&vectlist_free_callback,NULL);
	}
      // one pass, there is one data block
      if ( FLAG_IS_SET(this->flags,SDLINE_DYNAMIC) )
	{
	  free(this);
	}
      else
	{
	  this->first=NULL;
	  this->last=NULL;
	  this->lines=0;
	}
    }
  else
    {
      sdlines_foreach(this,&vectlist_free_callback,NULL);
      if ( FLAG_IS_SET(this->flags,SDLINE_DYNAMIC) )
	{
	  free(this);
	}
      else
	{
	  this->first=NULL;
	  this->last=NULL;
	  this->lines=0;
	}
    }
}

struct sdlines_concrete_builder {
  int lines;
  int points;
};

// collect number of lines and number of points within this
void sdlines_setup_concrete_builder(struct sdlines * this, struct sdlines_concrete_builder * builder)
{
  struct vectlist * vect = this->first;
  int bufferindex=0;
  for (int i=0; (i < this->lines) && (vect != NULL); i++)
    {
      builder->points+=vect->index;
      vect=vect->next;
      ++bufferindex;
    }
  builder->lines=bufferindex;
}

struct sdlines * sdlines_concrete( struct sdlines * this)
{
  struct sdlines * cloned = NULL;
  struct sdlines_concrete_builder builder;

  builder.lines=0;
  builder.points=0;
  
  // collect number of lines and number of points to allocate only one buffer.
  sdlines_setup_concrete_builder(this,&builder);

  printf("builder lines=%i;points=%i;\n",builder.lines,builder.points);
  
  int sdlines_size_bytes=sizeof(struct sdlines);
  int vectlist_size_bytes=( builder.lines * sizeof(struct vectlist) );
  int total_size = sdlines_size_bytes
    + vectlist_size_bytes
    + sizeof(struct vectbuffer) + ( builder.points * sizeof( float[3]) ); // remark allocate one point in excess
  int check_size = sdlines_size_bytes+vectlist_size_bytes;
  
  // create a unique zeroed buffer for whole data 
  char * buffer = calloc(1,total_size);

  printf("sdlines_concrete allocate %i\n", total_size);

  if ( buffer != NULL )
    {
      cloned = (struct sdlines *) buffer;
      struct vectlist * vectlist = (struct vectlist *) &buffer[sdlines_size_bytes];
      struct vectbuffer * vectbuffer = (struct vectbuffer *) &buffer[sdlines_size_bytes+vectlist_size_bytes];

      cloned->flags=(int) (SDLINE_CONCRETE|SDLINE_DYNAMIC);
      
      struct vectlist * vect = this->first;
      struct vectlist * nextvect = NULL;

      vectbuffer->allocated=builder.lines;

      int bufferindex=0;
      int vectbufferindex=0;
      
      for (int i=0; (i < this->lines) && (vect != NULL); i++)
	{
	  if (vect->index == 0 )
	    {
	      todo("sdlines_concrete with empty line, should not allocate space for it");
	    }
	  else
	    {
	      float * vector=&vectbuffer->vector[vectbufferindex][0];
	      if ( bufferindex > 0 )
		{
		  vectlist[bufferindex-1].next=&vectlist[bufferindex];
		}
	      vectlist[bufferindex].flags=SDLINE_CONCRETE; // make sure to not set SDLINE_DYNAMIC.
	      vectlist[bufferindex].index=vect->index;
	      vectlist[bufferindex].count=vect->index;
	      vectlist[bufferindex].vectorp=vector;
	      memcpy(vector,vect->vectorp,vect->index * 3 *  sizeof(float));
	      check_size+=vect->index * 3 *  sizeof(float);
	      vectbufferindex+=vect->index;
	      ++bufferindex;
	    }
	  nextvect=vect->next;
	  vect=nextvect;
	}
      
      cloned->lines=bufferindex;

      if (check_size > total_size )
	{
	  fprintf(stderr,"[FATAL] memory corruption used %i > allocated %i\n",check_size, total_size);
	  exit(1);
	}
      else
	{
	  fprintf(stdout,"[OUT] memory used %i <= allocated %i\n",check_size, total_size);
	}
      if ( bufferindex > 0 )
	{
	  cloned->first=&vectlist[0];
	  cloned->last=&vectlist[bufferindex-1];
	}
      // else rely on zeroed
      
      cloned->points=vectbufferindex;
      cloned->buffer=vectbuffer;

      printf("cloned lines=%i;points=%i;\n",bufferindex,vectbufferindex);
    }

  return cloned;
}

struct sdlines * sdlines_create_growable(int count)
{
    struct sdlines * growable = calloc(1,sizeof(struct sdlines));
    if ( growable != NULL )
      {
	growable->flags = SDLINE_DYNAMIC | SDLINE_GROWABLE;
	struct vectlist * vect = vectlist_new(count);	
	if ( vect != NULL )
	  {
	    sdlines_add_vectlist(growable,vect);
	  }
      }
    return growable;
}

struct xlines * xlines_clone(struct xlines * source, int points, int offset)
{
  struct xlines * destination;
  int index=0;
  int headersize=sizeof(struct xlines) * points ;
  int needmem=0;
  int pointsdone=0;
  // compute needed memory for clone
  // create a unique allocation for xlines and XPoints array.
  needmem=headersize;
  for (index =offset; index < offset+points; index ++)
    {
      needmem+=source[index].points * sizeof(XPoint);
    }
  destination=calloc(1,needmem);
  if ( destination != NULL )
    {
      memcpy(destination,&source[offset],headersize);
      for(index=0;index<points;index++)
	{
	  void * vector = ((void*) destination) + headersize + pointsdone;
	  memcpy(vector,&source[offset],destination[index].points * sizeof(XPoint));
	  destination[index].vector=vector;
	  pointsdone+=destination[index].points;
	}
    }
  return destination;
}
