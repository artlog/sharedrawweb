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
#include "imawriter.h"

void usage()
{
  printf("read a .ima and convert it into a .imc\n");
  printf("params : <ima input filename> <name of .imc file>\n");
  printf(" in input filename .ima  extension should be included\n");
  printf("ex:\n");
  printf("compressor ../flat3.ima flat3.imc\n");
  printf("will create a c file named flat containing struct xline flat content\n wihtin current directory");
}


int main(int argc, char ** argv)
{
  if ( argc > 1)
    {
      struct alinputstream input;
      struct aloutputstream output;
      int debug_compressor=0;
      
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
	  aldebug_printf(NULL,"missing an argument %s\n","output file");
	  usage();
	  exit(1);
	}

      inputfilename=argv[1];

      FILE * file = fopen( inputfilename, "r");
      if ( file != NULL )
	{
	  alinputstream_init(&input, fileno(file));
	  input.debug=debug_compressor;

	  if ( filename_is_ima(inputfilename) )
	    {
	      aldebug_printf(NULL,"Ima file detected '%s'\n", inputfilename);
	      read_ima( &input, &sdlines);

	      char * checkima = "check.ima";
	      FILE * genfile = fopen( checkima, "w");
	      if ( genfile == NULL )
		{
		  aldebug_printf(NULL,"[ERROR] can't create %s\n", checkima );
		  exit(1);
		}
	      aloutputstream_init(&output,genfile);
	      write_ima(&output,&sdlines);
	      fflush(genfile);
	      fclose(genfile);
	      
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
		aldebug_printf(NULL,"[ERROR] can't create %s\n", varname);
		exit(1);
	      }	    
	    aloutputstream_init(&output,genfile);
	    if ( ! drawlinecompressor_write_imc(&output,&sdlines,debug_compressor))
	      {
		exit(1);
	      }
	    fclose(genfile);
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
