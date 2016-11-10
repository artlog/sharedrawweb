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

void dump_xlines(FILE * f, struct sdlines * lines, char * varname)
{
  struct vectlist * vect = lines->first;
  for (int i=0; (i < lines->lines) && (vect != NULL); i++)
    {
      float* v;  
      fprintf(f,"XPoint %s_l%i[%i]={\n",varname,i,vect->index);
      v=&vect->vector[0][0];
      fprintf(f,"{%i,%i}",(int) v[0],(int) v[1]);
      for ( int j=1; j<vect->index;j++)
	{
	  if ( j % 5 == 0 )
	    {
	      fprintf(f,"\n");
	    }
	  v=&vect->vector[j][0];
	  fprintf(f,",{%i,%i}",(int) v[0],(int) v[1]);
	}
      fprintf(f,"\n};\n");
      vect=vect->next;
      if ( vect !=NULL)
	{
	  fprintf(f,"\n");
	}
    } 
  vect=lines->first;
  fprintf(f,"\nstruct xlines { int points; XPoint *vector;};\n");
  fprintf(f,"\nstruct xlines %s[%i]={\n",varname, lines->lines);
  for (int i=0; (i < lines->lines) && (vect != NULL); i++)
    {      
      fprintf(f,"{.points=%i,.vector=%s_l%i}",vect->index,varname,i);
      vect=vect->next;
      if ( vect != NULL)
	{
	  fprintf(f,",\n");
	}
    }
  if ( vect != NULL )
    {
      printf("??? points=%i ????",vect->index);
    }
  fprintf(f,"};\n");
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
