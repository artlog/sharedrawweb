#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "drawlinecompressor.h"
#include "sdlines.h"
#include "imareader.h"

void usage()
{
  printf("read a .ima and convert it into a .imc\n");
  printf("params : <ima input filename> <name of .imc file>\n");
  printf(" in input filename .ima  extension should be included\n");
  printf("ex:\n");
  printf("compressor ../flat3.ima flat3.imc\n");
  printf("will create a c file named flat containing struct xline flat content\n wihtin current directory");
}

int debug_compressor=1;

int main(int argc, char ** argv)
{
  if ( argc > 1)
    {
      struct drawlinecompressor compressor;
      struct inputstream input;
      struct outputstream output;
      struct sdlines sdlines={
	.lines=0,
	.first=NULL,	
	.last=NULL
      };

      char* varname;
      char* inputfilename;


      if ( argc > 2 )
	{
	  varname=argv[2];
	}
      else
	{
	  printf("missing an argument %s\n","output file");
	  usage();
	  exit(1);
	}

      inputfilename=argv[1];

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
	      printf("Ima file expected '%s' is not ending with correct extension\n", inputfilename);
	      usage();
	      exit(1);
	    }
	  {
	    FILE * genfile = fopen( varname, "w");
	    if ( genfile == NULL )
	      {
		fprintf(stderr,"[ERROR] can't create %s\n", varname);
		exit(1);
	      }
	    int fdout = fileno(genfile);
	    outputstream_init(&output,fdout);	   
	    outputstream_writeint32(&output,sdlines.lines);
	    for (int i=0; i< sdlines.lines; i++)
	      {
		if ( debug_compressor > 0) { fprintf(stderr, "Line %u/%u\n", (i+1),sdlines.lines); }
		struct vectlist * vectlist = sdlines_get_vectlist(&sdlines,i);
		if (vectlist != NULL )
		  {
		    drawlinecompressor_init(&compressor, vectlist_to_pointlist(vectlist));
		    compressor.debug=debug_compressor;
		    drawlinecompressor_compress(&compressor, &output);
		  }
		else
		  {
		    fprintf(stderr,"[ERROR] can't get %ith  line\n", i);
		    exit(1);
		  }
	      }
	    close(fdout);
	    readok = 1;
	  }
	  close(fd);
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
