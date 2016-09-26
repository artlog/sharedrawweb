#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "drawlineexpander.h"
#include "sdlines.h"
#include "imareader.h"

void usage()
{
  printf("read a .imc or .ima and convert it into c data to include in by example opengl\n");
  printf("params : <input filename> <optional name of c data structure generated, default to 'default'.>\n");
  printf(" in input filename .imc .ima  extension should be included, it is used for type detection\n");
  printf("ex:\n");
  printf("expander ../flat3.imc flat\n");
  printf("will create a c file named flat containing struct xline flat content\n wihtin current directory");
}

int debug_expander=0;

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
      sdlines_add_vectlist(lines,vect);
    }
}

void close_adapter(struct pointlist * this, struct sdadapter * adapter)
{
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
  int gensdlines=0;
  int genxlines=1;
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
	.cx=-100,
	.cy=-100,
	.width=200, 
	.height=-200,// negative to match standard y axis
	.f_before=setup_adapter,
	.f_for_each=adapt_point,
	.f_after=close_adapter,
	.data=&sdlines
      };
      char* varname;
      char* inputfilename;

      if ( genxlines == 1 )
	{
	  adapter.width=1;
	  adapter.height=1;
	}
      if ( argc > 2 )
	{
	  varname=argv[2];
	}
      else
	{
	  varname="default";
	}
      inputfilename=argv[1];

      FILE * genfile = fopen( varname, "w");
      if ( genfile == NULL )
	{
	  fprintf(stderr,"[ERROR] can't create %s\n", varname);
	  exit(1);
	}

      int fd = open( inputfilename, 0);
      if ( fd != - 1 )
	{
	  int readok = 0;
	  
	  inputstream_init(&input, fd);
	  input.debug=1;

	  if ( filename_is_ima(inputfilename) )
	    {
	      printf("Ima file detected '%s'\n", inputfilename);
	      read_ima( &input, &sdlines);
	      readok = 1;
	    }
	  else
	    {
	      int lines = inputstream_readuint32(&input);
	      if (lines < 10000)
		{
		  for (int i=0; i< lines; i++)
		    {
		      if ( debug_expander > 0) { fprintf(stderr, "Line %u/%u\n", (i+1),lines); }
		      drawlineexpander_init(&expander);
		      expander.debug=1;
		      drawlineexpander_expand(&expander, &input);
		      if (debug_expander > 0 ) { pointlist_dump(expander.expandedLines); }
		      pointlist_update_min_max(expander.expandedLines,&min,&max);
		      pointlist_foreach(expander.expandedLines, &adapter);
		    }
		  sdpoint_dump(&max,"// max");
		  sdpoint_dump(&min,"// min");
		  readok = 1;
		}
	      else
		{
		  fprintf(stderr, "Too many lines %xh", lines);
		}
	    }
	  if (readok)
	  {
	    if ( gensdlines )
	      {
		dump_sdlines(&sdlines,varname);
	      }
	    if ( genxlines )
	      {
		dump_xlines(genfile,&sdlines,varname);
	      }
	  }
	  close(fd);
	  fclose(genfile);
	}
      else
	{
	  fprintf(stderr,"[ERROR] can't open %s file", argv[1]);
	}
    }
  else
    {
      usage();
    }

}
