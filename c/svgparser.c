/**
 svgparser ( initialy derived from xmlReader of  Daniel Veillard )
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlreader.h>

#ifdef LIBXML_READER_ENABLED

int svgparser_debug=0;

#define MAX_RECTS 54

struct matrix6 {
  float a;
  float b;
  float c;
  float d;
  float e;
  float f;
};

struct svg_transform_matrix {
  float coeff[6];
};

struct svg_rect {
  char * id;
  float width;
  float height;
  float x;
  float y;
  float ry;
  struct svg_transform_matrix * transform;
  char * path; // ugly but quick shortcut
};

struct sp {
  float x;
  float y;
};
  
struct rect4 {
  struct sp point[4];
};

struct rect_context
{
  int rects;
  // too bad hard coded value for inital dev for cube.
  struct svg_rect rect[MAX_RECTS];
};

struct rect_context r_context;


void usage()
{
  printf(" dump a svg into pathes, if object is rect convert it to path\n");
  printf("usage:\n");
  printf("parameter <svgfile to parse> (path|id)\n");
}
/*

https://www.w3.org/TR/SVG/coords.html#TransformMatrixDefined

[ a c e 
  b d f
  0 0 1 ]
  
xn = a*x + c*y + e
yn = b*x + d*y + f

Allow that origin == dest ( use a temporary local variable ).
*/
void svg_transform_matrix_apply_matrix(struct svg_transform_matrix * transform, struct svg_rect * orig, struct svg_rect * dest )
{
  struct matrix6 * matrix = (struct matrix6 *) transform;
  struct svg_rect temp;
  temp.x=(matrix->a * orig->x) + (matrix->c * orig->y) + matrix->e;
  temp.y=(matrix->b * orig->x) + (matrix->d * orig->y) + matrix->f;
  dest->x=temp.x;
  dest->y=temp.y;
}

/**
apply transfrom to svg_rect to construct a rect4 structure of a line of points 
*/
void svg_transform_matrix_rect_to_rect4(struct svg_transform_matrix * transform, struct svg_rect * orig, struct rect4 * dest )
{
  struct matrix6 * matrix = (struct matrix6 *) transform;
  dest->point[0].x=(matrix->a * orig->x) + (matrix->c * orig->y) + matrix->e;
  dest->point[0].y=(matrix->b * orig->x) + (matrix->d * orig->y) + matrix->f;

  dest->point[1].x=(matrix->a * ( orig->x + orig->width) ) + (matrix->c * orig->y) + matrix->e;
  dest->point[1].y=(matrix->b * ( orig->x + orig->width) ) + (matrix->d * orig->y) + matrix->f;

  dest->point[2].x=(matrix->a * ( orig->x + orig->width) ) + (matrix->c * ( orig->y + orig->height ) ) + matrix->e;
  dest->point[2].y=(matrix->b * ( orig->x + orig->width) ) + (matrix->d * ( orig->y + orig->height ) ) + matrix->f;

  dest->point[3].x=(matrix->a * orig->x) + (matrix->c * ( orig->y + orig->height ) ) + matrix->e;
  dest->point[3].y=(matrix->b * orig->x) + (matrix->d * ( orig->y + orig->height ) ) + matrix->f;
}

void rect4_dump( struct rect4 * rect, FILE * out)
{
  if ( rect != NULL )
    {
      for (int i=0; i< 4;i++)
	{
	  fprintf(out, "%i:(x=%f,y=%f),",rect->point[i].x,rect->point[i].y);
	}
      fprintf(out,"\n");
    }
}

void rect4_dump_svgpath( struct rect4 * rect, FILE * out)
{
  if ( rect != NULL )
    {
      fprintf(out, "\"M ");
      for (int i=0; i< 4;i++)
	{
	  fprintf(out, "%f,%f ",rect->point[i].x,rect->point[i].y);
	}
      fprintf(out,"z\" ");
    }
}

void svg_rect_dump( struct svg_rect * rect, FILE * out)
{
  if ( rect != NULL)
    {
      if ( rect->id != NULL)
	{
	  fprintf(out,"id=%s\n",rect->id);
	}
      fprintf(out,"width=%f\nheight=%f\nx=%f\ny=%f\nry=%f\n",rect->width,rect->height,rect->x,rect->y,rect->ry);
      if ( rect->transform != NULL)
	{
	  struct svg_transform_matrix * mat = rect->transform;
	  fprintf(out,"transform='matrix(%f,%f,%f,%f,%f,%f)'\n", mat->coeff[0],mat->coeff[1],mat->coeff[2],mat->coeff[3],mat->coeff[4],mat->coeff[5]);
	}
    }
}

/**
  extract information of rect :

   <rect id="FU"
       width="56.498253"
       height="56.747768"
       x="442.04553"
       y="96.053566"
       ry="7.9066749"
       transform="matrix(0.83368189,0.55224498,-0.84812865,0.52979033,0,0)" />

**/
static void
processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
	name = BAD_CAST "--";

    if (strncmp(name, "rect",4) == 0 )
      {
	int attributesc=xmlTextReaderAttributeCount(reader);
	if ( svgparser_debug > 0 )
	  {
	    printf("rect %d\n",r_context.rects); 
	  }
	if ( ( attributesc > 0 ) && ( r_context.rects < MAX_RECTS ))
	  {
	    struct svg_rect * this_rect=&(r_context.rect[r_context.rects]);
	    this_rect->transform=NULL;
	    char * dummy;
	    xmlChar * xmlChar =	xmlTextReaderGetAttribute(reader,"id");
	    if ( xmlChar != NULL)
	      {
		if ( svgparser_debug > 0 )
		  {
		    printf(" id='%s' \n", xmlChar);
		  }		    
		char * newid=strdup(xmlChar);
		if (newid != NULL)
		  {
		    this_rect->id=newid;
		  }
		else
		  {
		    fprintf(stderr,"Heap shortage while duplicating id '%s'\n", xmlChar);
		    exit(1);
		  }	       
	      }
	    xmlChar=xmlTextReaderGetAttribute(reader,"width");
	    if ( xmlChar != NULL)
	      {
		this_rect->width=strtof(xmlChar,&dummy);
		if ( svgparser_debug > 0 )
		  {
		    printf(" width='%s' %f \n", xmlChar, this_rect->width);
		  }
	      }	    
	    xmlChar=xmlTextReaderGetAttribute(reader,"height");
	    if ( xmlChar != NULL)
	      {
		this_rect->height=strtof(xmlChar,&dummy);
		if ( svgparser_debug > 0 )
		  {
		    printf(" height='%s' %f \n", xmlChar, this_rect->height);
		  }
	      }	    
	    xmlChar=xmlTextReaderGetAttribute(reader,"x");
	    if ( xmlChar != NULL)
	      {
		this_rect->x=strtof(xmlChar,&dummy);
		if ( svgparser_debug > 0 )
		  {
		    printf(" x='%s' %f \n", xmlChar, this_rect->x);
		  }
	      }	    
	    xmlChar=xmlTextReaderGetAttribute(reader,"y");
	    if ( xmlChar != NULL)
	      {
		this_rect->y=strtof(xmlChar,&dummy);
		if ( svgparser_debug > 0 )
		  {
		    printf(" y='%s' %f \n", xmlChar, this_rect->y);
		  }
	      }	    
	    xmlChar=xmlTextReaderGetAttribute(reader,"ry");
	    if ( xmlChar != NULL)
	      {		
		this_rect->ry=strtof(xmlChar,&dummy);
		if ( svgparser_debug > 0 )
		  {
		    printf(" ry='%s' %f \n", xmlChar, this_rect->ry);
		  }
	      }
	    xmlChar=xmlTextReaderGetAttribute(reader,"transform");
	    if ( xmlChar != NULL)
	      {
		if ( svgparser_debug > 0 )
		  {
		    printf(" transform='%s' \n", xmlChar);
		  }
		struct svg_transform_matrix * mat = malloc(sizeof( struct svg_transform_matrix));
		if ( mat != NULL )
		  {
		    // https://www.w3.org/TR/SVG/coords.html#TransformAttribute
		    int parsed = sscanf(xmlChar,"matrix(%f,%f,%f,%f,%f,%f)", &mat->coeff[0],&mat->coeff[1],&mat->coeff[2],&mat->coeff[3],&mat->coeff[4],&mat->coeff[5]);
		    if ( parsed != 6 )
		      {
			fprintf(stderr,"[ERROR] fail to parse transform %s for id %s\n", xmlChar, (this_rect->id == NULL) ? "-" : this_rect->id);
			
		      }
		    // svg_transform_matrix_apply_matrix( mat, this_rect, this_rect);
		    this_rect->transform=mat;
		  }
		else
		  {
		    fprintf(stderr,"Heap shortage while allocating svg_matrix\n");
		    exit(1);
		  }
		
	      }
	    if ( svgparser_debug > 0 )
	      {
		svg_rect_dump(this_rect,stdout);
	      }	      
	    ++r_context.rects;
	  }	
      }
      else if (strncmp(name, "path",4) == 0 )
      {
	int attributesc=xmlTextReaderAttributeCount(reader);
	if ( svgparser_debug > 0 )
	  {
	    printf("path %d\n",r_context.rects); 
	  }
	if ( ( attributesc > 0 ) && ( r_context.rects < MAX_RECTS ))
	  {
	    struct svg_rect * this_rect=&(r_context.rect[r_context.rects]);
	    this_rect->transform=NULL;
	    char * dummy;
	    xmlChar * xmlChar =	xmlTextReaderGetAttribute(reader,"id");
	    if ( xmlChar != NULL)
	      {
		if ( svgparser_debug > 0 )
		  {
		    printf(" id='%s' \n", xmlChar);
		  }
		char * newid=strdup(xmlChar);
		if (newid != NULL)
		  {
		    this_rect->id=newid;
		  }
		else
		  {
		    fprintf(stderr,"Heap shortage while duplicating id '%s'\n", xmlChar);
		    exit(1);
		  }
	      }
	    xmlChar=xmlTextReaderGetAttribute(reader,"d");
	    if ( xmlChar != NULL)
	      {
		if ( svgparser_debug > 0 )
		  {
		    printf(" d='%s' \n", xmlChar);
		  }
		char * newpath=strdup(xmlChar);
		if (newpath != NULL)
		  {
		    this_rect->path=newpath;
		  }

	      }
	    ++r_context.rects;
	  }
      }
}

/**
 * streamFile:
 * @filename: the file name to parse
 *
 * Parse and print information about an XML file.
 */
static void
streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            fprintf(stderr, "%s : failed to parse\n", filename);
        }
    } else {
        fprintf(stderr, "Unable to open %s\n", filename);
    }
}

void convert_to_svgpath()
{
  struct rect4 rectangle[MAX_RECTS];
  
  for (int i=0 ; i < r_context.rects; i++)
    {
      struct svg_rect * rect = &r_context.rect[i];
      if ( rect->path == NULL)
	{
	  svg_transform_matrix_rect_to_rect4(rect->transform,rect,&rectangle[i]);
	  rect4_dump_svgpath(&rectangle[i],stdout);
	}
      else
	{
	  fprintf(stdout,"\"");
	  fprintf(stdout,rect->path);
	  fprintf(stdout,"\" ");
	}
    }
}

void dump_ids()
{
  struct rect4 rectangle[MAX_RECTS];
  
  for (int i=0 ; i < r_context.rects; i++)
    {
      struct svg_rect * rect = &r_context.rect[i];
      if ( rect->id != NULL)
	{
	  int l = strlen(rect->id);
	  if ( l == 3 )
	    {
	      fprintf(stdout,"corner %s %i",rect->id, i);
	    }
	  else if ( l == 2)
	    {
	      fprintf(stdout,"edge   %s  %i",rect->id, i);
	    }
	  else if (l == 1)
	    {
	      fprintf(stdout,"center %s   %i",rect->id, i);
	    }
	  else
	    {
	      fprintf(stdout,"?????? %s %i",rect->id, i);
	    }
	}
      fprintf(stdout,"\n");
    }
}

int main(int argc, char **argv)
{
  char * svgfilename = NULL;
  int dump_id=0;
  if (argc < 2)
    {
      usage();      
      return(1);
    }

  if ( argc > 2 )
    {
      dump_id=( strncmp(argv[2],"id",4) == 0);
    }
      
    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    r_context.rects= 0;

    svgfilename=argv[1];
    streamFile(svgfilename);

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();

    if ( dump_id)
      {
	dump_ids();
      }
    else
      {
	convert_to_svgpath();
      }
    
    return(0);
}

#else
int main(void) {
    fprintf(stderr, "XInclude support not compiled in\n");
    exit(1);
}
#endif
