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

      FILE * file = fopen( inputfilename, "r");
      if ( file != NULL )
	{
	  int readok = 0;
	  
	  inputstream_init(&input, fileno(file));
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
	    outputstream_init(&output,genfile);	   
	    outputstream_writeint32(&output,sdlines.lines);
	    for (int i=0; i< sdlines.lines; i++)
	      {
		if ( debug_compressor > 0) { fprintf(stderr, "Line %u/%u\n", (i+1),sdlines.lines); }
		struct vectlist * vectlist = sdlines_get_vectlist(&sdlines,i);
		if (vectlist != NULL )
		  {
		    struct pointlist * pointlist = vectlist_to_pointlist(vectlist);
		    if (pointlist != NULL )
		      {
			drawlinecompressor_init(&compressor, pointlist);
			compressor.debug=debug_compressor;
			drawlinecompressor_compress(&compressor, &output);
			free(pointlist);
		      }		    
		  }
		else
		  {
		    fprintf(stderr,"[ERROR] can't get %ith  line\n", i);
		    exit(1);
		  }
	      }
	    fclose(genfile);
	    readok = 1;
	  }
	  fclose(file);
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
