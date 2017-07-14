#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "drawlinetools.h"
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


int main(int argc, char ** argv)
{
  int gensdlines=0;
  int genxlines=1;
  if ( argc > 1)
    {
      struct drawlineexpander expander;
      struct inputstream input;
       struct sdlines sdlines={
	.lines=0,
	.first=NULL,	
	.last=NULL
      };
      char* varname;
      char* inputfilename;

      /** WHY ?
      if ( genxlines == 1 )
	{
	  adapter.width=1;
	  adapter.height=1;
	}
      **/
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
	      readok = drawlineexpander_read_sdlines( &expander, &input, &sdlines);
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
