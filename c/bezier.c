#include <X11/Xlib.h>
#include "bezier.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "svgpath.h"

void usage()
{
  printf("convert svgpath with bezier curves into a .ima file\n");
  printf("usage\n");
  printf("parameters : <number_of_points> \"<from_svgpathindex> to <to_svgpathindex>\" <ima file> <svgpath1> <svgpath2> ... \n");
}

void calc_coeff(struct bezier_cubic_param * param, int index, int points)
{
  double t=( (double) index/points);
  double t2 = t*t;
  double t3 = t2*t;
  double mt= (1-t);
  double mt2= mt*mt;
  double mt3= mt2*mt;

  param->coeff[0]=mt3;
  param->coeff[1]=3*t*mt2;
  param->coeff[2]=3*t2*mt;
  param->coeff[3]=t3;
}

void calc_XPoint(struct bezier_cubic_param * param, XPoint p[4], XPoint * result)
{
  result->x =
      p[0].x*param->coeff[0]
    + p[1].x*param->coeff[1]
    + p[2].x*param->coeff[2]
    + p[3].x*param->coeff[3];

  result->y =
      p[0].y*param->coeff[0]
    + p[1].y*param->coeff[1]
    + p[2].y*param->coeff[2]
    + p[3].y*param->coeff[3];
}

void calc_Point(struct bezier_cubic_param * param, struct bezier_cubic * bezier_cubic, struct svgpoint * result)
{
  int x = 
      bezier_cubic->point[0].x*param->coeff[0]
    + bezier_cubic->point[1].x*param->coeff[1]
    + bezier_cubic->point[2].x*param->coeff[2]
    + bezier_cubic->point[3].x*param->coeff[3];

  int y = 
      bezier_cubic->point[0].y*param->coeff[0]
    + bezier_cubic->point[1].y*param->coeff[1]
    + bezier_cubic->point[2].y*param->coeff[2]
    + bezier_cubic->point[3].y*param->coeff[3];

  if ( ((int) ((short) x )) !=  x)
    {
      exit(2);
    }
  if ( ((int) ((short) y )) !=  y)
    {
      exit(2);
    }
  result->x = (short) x;
  result->y = (short) y;
}


void keep_min(struct svgpoint *p, struct svgpoint *min)
{
  if ( p->x < min->x )
    {
      min->x=p->x;
    }
  if ( p->y < min->y )
    {
      min->y=p->y;
    }
}

void keep_max(struct svgpoint *p,struct svgpoint *max)
{
  if ( p->x > max->x )
    {
      max->x=p->x;
    }
  if ( p->y > max->y )
    {
      max->y=p->y;
    }
}

/**
 save number of points of this .ima
**/
void saveheader(FILE* saveto, int points)
{
  // .IMA format
  // header is number of points in little endian 2 bytes.
  if (saveto != NULL)
    {
      unsigned short usp=(unsigned short) points*2;
      fseek(saveto,0L,SEEK_SET);
      fwrite(&usp,2,1,saveto);
    }
}

void writexy(FILE* saveto,unsigned char x, unsigned char y)
{
  if (saveto !=NULL)
    {
      unsigned char o[2];
      o[0]=(unsigned char) x;
      o[1]=(unsigned char) y;
      fwrite(o,1,2,saveto);
    }
}
  
void startbeam(FILE* saveto)
{
  writexy(saveto,0xff,0xff);
}

void stopbeam(FILE* saveto)
{
  writexy(saveto,0xff,0xfe);
  writexy(saveto,0,0);
}

void savepoint(FILE* saveto,struct svgpoint* p, struct svgpoint * max, struct svgpoint * min)
{
  int px=(int) (255.0 * ((p->x - min->x)/(max->x - min->x)));
  int py=(int) (255.0 * ((p->y - min->y)/(max->y - min->y)));

  printf( "(%i,%i),",px,py);

  if ((px<0) || ( px > 255 )|| (py <0) || ( py > 255 ))
    {
      fprintf(stderr,"[ERROR] unexpected values px=%i py=%i p->x=%f p->y=%f min.x=%f min.y=%f max.x=%f max.y=%f   \n", px, py, p->x, p->y, min->x,min->y,max->x,max->y);
      exit(1);
    }
  // .IMA format.
  if ( saveto != NULL )
    {
      unsigned char o[2];
      o[0]=(unsigned char) px;
      // reversed y axis
      o[1]=(unsigned char) 255-py;
      // protect against beam on/off
      if (( o[0] == 255 ) & ( o[1] >= 254 ))
	{
	  o[1]=253;
	}      
      fwrite(o,1,2,saveto);      
    }
}

struct save_bezier_to_ima {
  int points;
  FILE * saveto;
  int total;
  int bezier;  
  struct svgpoint max;
  struct svgpoint min;
  struct svgpoint last;
  int object;
};

/**
  assumes all elements data are struct bezier_cubic *
  convert bezier element to as many points as specified in struct save_bezier_to_ima * param
**/
void * walk_bezier_list_step_2(struct allistof * list, struct allistelement * element, struct allistelement * next, int count, void * param)
{  
  struct save_bezier_to_ima * bezier_ima = (struct save_bezier_to_ima *) param;  
  
  if (param != NULL)
    {
      struct svgpoint p[4];
      struct svgpoint bezier[2];
      int useless=0;
      struct bezier_cubic_param coeff;

      FILE * saveto = bezier_ima->saveto;
      int points = bezier_ima->points;
      
      struct svgpath_element * svgpath_element = (struct svgpath_element *) element->data;
      if ( is_bezier(svgpath_element) )
      {
	struct bezier_cubic * bezier_cubic= &svgpath_element->bezier ;
            
	if ( bezier_cubic != NULL )
	  {
	    for (int i=0;i<4;i++)
	      {
		p[i].x= bezier_cubic->point[i].x;
		p[i].y= bezier_cubic->point[i].y;
	      }
	  }
   
	bezier[0].x=p[0].x;
	bezier[0].y=p[0].y;
	savepoint(saveto,&bezier[0],&bezier_ima->max,&bezier_ima->min);
	for (int j=1; j<points; j++)
	  {
	    calc_coeff(&coeff,j,points);
	    calc_Point(&coeff,bezier_cubic,&bezier[j%2]);
	    if ( ( bezier[j%2].x == bezier[(j-1)%2].x)
		 && ( bezier[j%2].y == bezier[(j-1)%2].y) )
	      {
		/*
		  Useless because very same point than previous.
		  This is mainly due to shrink in (x,y) ([0-255],[0-255]) space
		*/
		++useless;
	      }
	    else 	  
	      {
		savepoint(saveto,&bezier[j%2],&bezier_ima->max,&bezier_ima->min);
		bezier_ima->total=bezier_ima->total +1;
	      }
	  }
	savepoint(saveto,&p[3],&bezier_ima->max,&bezier_ima->min);
	fflush(stdout);
	bezier_ima->bezier=bezier_ima->bezier + 1;
	fprintf(stderr,"\n %i) points %i useless %i \n",bezier_ima->bezier, points, useless);
      }
      else
	{
	  struct svgpath_line * line= &svgpath_element->line;
	  if ( line != NULL )
	    {
	      for (int i=0;i<line->points;i++)
		{
		  savepoint(saveto,&line->point[i],&bezier_ima->max,&bezier_ima->min);
		  bezier_ima->total=bezier_ima->total +1;
		}
	    }	  
	}
      
    }
  return param;
}


// gather all informations ( full bounding box )
void * walk_bezier_list_step_1(struct allistof * list, struct allistelement * element, struct allistelement * next, int count, void * param)
{  
  struct save_bezier_to_ima * bezier_ima = (struct save_bezier_to_ima *) param;  
  
  if (param != NULL)
    {
      struct svgpath_element * svgpath_element = (struct svgpath_element *) element->data;
      if ( is_bezier(svgpath_element) )
      {
	struct bezier_cubic * bezier_cubic= &svgpath_element->bezier;
	if ( bezier_cubic != NULL )
	  {
	    for (int i=0;i<4;i++)
	      {
		keep_min(&bezier_cubic->point[i],&bezier_ima->min);
		keep_max(&bezier_cubic->point[i],&bezier_ima->max);
	      }
	  }
      }
      else
	{
	  struct svgpath_line * line= &svgpath_element->line;
	  if ( line != NULL )
	    {
	      for (int i=0;i<line->points;i++)
		{
		  keep_min(&line->point[i],&bezier_ima->min);
		  keep_max(&line->point[i],&bezier_ima->max);
		}
	    }	  
	}
    }
  return param;
}

int main(int argc, char* args[])
{
  struct save_bezier_to_ima bezier_ima;
  FILE * saveto;
  int start_param = 4;
  
  int points = atoi(args[1]);
  if ( argc > start_param )
    {
      int start=0;
      int stop=0;

      if ( sscanf(args[2],"%i to %i",&start,&stop) != 2 )
	{
	  fprintf(stderr,"[ERROR] second argument expected to be <start_index> to <end_index>\n");
	  exit(1);
	}

      saveto = fopen(args[3],"w");
      if ( saveto == NULL )
	{
	  fprintf(stderr,"[ERROR] Can't open %s to write", args[2]);
	  exit(1);
	}
      else
	{
	  // will be rewritten anyway, but this allows to reserve enough place.
	  saveheader(saveto,points);
	}      
	
      bezier_ima.total=0;
      bezier_ima.bezier=0;
      bezier_ima.object=0;
      bezier_ima.points=points;
      bezier_ima.saveto=saveto;
      
      bezier_ima.min.x=4096;
      bezier_ima.min.y=4096;
      bezier_ima.max.x=-4096;
      bezier_ima.max.y=-4096;

      bezier_ima.last.x=0;
      bezier_ima.last.y=0;

      // first pass just to get min/max
      for (int path=start; (path+start_param<argc) && (path < stop) ;path++)
      {
	struct svgpath svgpath;
	svgpath.svgpathcontext=NULL;

	bezier_ima.last.x=0;
	bezier_ima.last.y=0;

	int result = svgpath_parse(&svgpath, args[path+start_param], strlen(args[path+start_param]), &bezier_ima.last);
	if ( result != 0 )
	  {
	    fprintf(stderr,"unrecognized / parse failure (%i) for svg path specifiation '%s'\n",result,args[path+start_param]);
	    exit(1);
	  }	
	struct allistof * list_bezier=allistcontext_get_membership(svgpath.svgpathcontext,0);
	if ( list_bezier != NULL )
	  {
	    allist_for_each(list_bezier,
			    NULL, // start from head
			    walk_bezier_list_step_1, // function to capture bounding box
			    &bezier_ima, // parameter to fill
			    1,// walk one by one
			    0 // start at first
			    );
	    printf("min %f %f\n", bezier_ima.min.x, bezier_ima.min.y);
	    printf("max %f %f\n", bezier_ima.max.x, bezier_ima.max.y);	    
	  }
	else
	  {
	    fprintf(stderr,"[ERROR] svg path parsing of '%s' did not find any bezier", args[path+start_param]);
	    exit(1);
	  }
	// FIXME : should dispose MEMORY of list_bezier HERE
	fprintf(stderr,"[WARNING] ask dev to release unused memory here ....");
	printf("IMAGE PATH %i DONE \n", path);
      }
      // second pass does it actualy.
      bezier_ima.last.x=0;
      bezier_ima.last.y=0;

      // workaround, seems bounding bezier is is an accurate evaluation of real bezier points, rounding effects ?
      bezier_ima.min.x=bezier_ima.min.x-1;
      bezier_ima.min.y=bezier_ima.min.y-1;


      for (int path=start; (path+start_param<argc) && (path < stop) ;path++)
      {
	struct svgpath svgpath;
	svgpath.svgpathcontext=NULL;
	bezier_ima.last.x=0;
	bezier_ima.last.y=0;

	int result = svgpath_parse(&svgpath, args[path+start_param], strlen(args[path+start_param]),&bezier_ima.last);
	if ( result != 0 )
	  {
	    fprintf(stderr,"unrecognized / parse failure (%i) for svg path specifiation '%s'\n",result,args[path+start_param]);
	    exit(1);
	  }	
	struct allistof * list_bezier=allistcontext_get_membership(svgpath.svgpathcontext,0);
	if ( list_bezier != NULL )
	  {
	    startbeam(saveto);
	    allist_for_each(list_bezier,
			    NULL, // start from head
			    //			    walk_bezier_list_step_2, // function to add bezier points
			    walk_bezier_list_step_2, // function to add bezier points
			    &bezier_ima, // parameter to fill
			    1,// walk one by one
			    0 // start at first
			    );
	    stopbeam(saveto);
	    bezier_ima.object=bezier_ima.object+1;
	  }
	else
	  {
	    fprintf(stderr,"[ERROR] svg path parsing of '%s' did not find any bezier", args[path+start_param]);
	    exit(1);
	  }
	// FIXME : should dispose MEMORY of list_bezier HERE
	fprintf(stderr,"[WARNING] ask dev to release unused memory here ....");
	printf("IMAGE PATH %i DONE \n", path);
      }   

      // bezier_ima.points=points;
      printf("min %f %f\n", bezier_ima.min.x, bezier_ima.min.y);
      printf("max %f %f\n", bezier_ima.max.x, bezier_ima.max.y);
      printf("total %d \n", bezier_ima.total);
      printf("object %d \n", bezier_ima.object);

      if ( saveto != NULL)
	{
	  // actualy save header at the end (does a rewind to start of file).
	  saveheader(saveto,bezier_ima.total+(bezier_ima.object*3));
	  fflush(saveto);
	  fclose(saveto);
	}
    }
}
