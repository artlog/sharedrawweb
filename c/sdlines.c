#include <stdio.h>
#include <stdlib.h>
#include "sdlines.h"

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
      vect->index=0;
      vect->next=NULL;
    }
  return vect;
}

void sdlines_add_vectlist(struct sdlines * lines, struct vectlist * vect)
{
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

struct dump_sdlines_data {
  char * varname;
};

void dump_sdlines_content(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  float* v;
  char * varname = ((struct dump_sdlines_data *) data)->varname;
  printf("float %s_l%i[%i][2]={",varname,index,line->index);
  v=&line->vector[0][0];
  printf("{%f,%f}",v[0],v[1]);
  for ( int j=1; j<line->index;j++)
    {
      v=&line->vector[j][0];
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
  v=&line->vector[0][0];
  fprintf(f,"{%i,%i}",(int) v[0],(int) v[1]);
  for ( int j=1; j<line->index;j++)
    {
      if ( j % 5 == 0 )
	{
	  fprintf(f,"\n");
	}
      v=&line->vector[j][0];
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
			  v=&vect->vector[j][0];
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
      point.x=(int) this->vector[i][0];
      point.y=(int) this->vector[i][1];
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
	  (*callback->f_for_each)(this,callback,vect,i,vect->next,data);
	  vect=vect->next;
	}
    }
  if (callback->f_after != NULL)
    {
      (*callback->f_after)(this,callback,data);
    }  
}
