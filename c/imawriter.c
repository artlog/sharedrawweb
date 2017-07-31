#include "imawriter.h"

/**

2 pass : collect size to write, then write it.

**/

struct dump_sdlines_data {
  struct outputstream * stream;
  struct sdlines_matrix6 transform;
  int debug;
  int bytes;
  int points;
};

// for each line collect number of byte it will take to convert it
// TODO collect (x,y) x (min,max) to generate transform.
void write_ima_collect(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  struct dump_sdlines_data * sdlines_data =  (struct dump_sdlines_data *) data;
  sdlines_data->bytes+=( 4+(line->index)*2);
  ++sdlines_data->points;
}

void write_ima_content_ref_header(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  struct dump_sdlines_data * sdlines_data =  (struct dump_sdlines_data *) data;
  struct outputstream * stream = sdlines_data->stream;
  if (stream->debug)
    {
      printf("{\"count\":%i",this->lines);
      printf(",\"lines\":\n");
    }

  fprintf(stream->file,"%c%c",(char) ((sdlines_data->bytes)%256),(char) ((sdlines_data->bytes)/256));  
}

void write_ima_content_ref(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  struct dump_sdlines_data * sdlines_data =  (struct dump_sdlines_data *) data;
  struct outputstream * stream = sdlines_data->stream;
  float* v;
  XPoint dest;
  v=&line->vector[0][0];
  if ( sdlines_data->debug) {
    printf("[");
    printf("{\"x\"=%f,\"y\"=%f}",v[0],v[1]);
  }  
  fprintf(stream->file,"%c%c",IMA_X_BEAN_CONTROL,IMA_Y_BEAN_OFF);
  sdlines_matrix6_apply_matrix(& sdlines_data->transform,v,&dest);
  fprintf(stream->file,"%c%c",(unsigned char)dest.x,(unsigned char)dest.y);
  fprintf(stream->file,"%c%c",IMA_X_BEAN_CONTROL,IMA_Y_BEAN_ON);
  for ( int j=1; j<line->index;j++)
    {
      v=&line->vector[j][0];
      if (sdlines_data->debug)
	{
	  printf(",\n{\"x\"=%f,\"y\"=%f}",v[0],v[1]);
	}
      sdlines_matrix6_apply_matrix(& sdlines_data->transform,v,&dest);
      fprintf(stream->file,"%c%c",(unsigned char)dest.x,(unsigned char)dest.y);
    }
  if (sdlines_data->debug)
    {
      printf("]");
      if ( next !=NULL)
	{
	  printf(",\n");
	}
    }
}

void write_ima_content_ref_footer(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  struct dump_sdlines_data * sdlines_data =  (struct dump_sdlines_data *) data;
  if (sdlines_data->debug)
    {
      printf("};\n");
    }
}

/** 
collect all lines size
**/
struct sdlines_iterator_callback write_ima_callback_collect = {
  .f_before=NULL,
  .f_for_each=write_ima_collect,
  .f_after=NULL
};

/**
write it 
**/
struct sdlines_iterator_callback write_ima_callback_content_ref = {
  .f_before=write_ima_content_ref_header,
  .f_for_each=write_ima_content_ref,
  .f_after=write_ima_content_ref_footer
};

int write_ima(struct outputstream * stream, struct sdlines * lines)
{
  struct dump_sdlines_data  write_ima_data;
  write_ima_data.stream = stream;
  write_ima_data.bytes=0;
  write_ima_data.points=0;

  // revert y
  write_ima_data.transform.a=1;
  write_ima_data.transform.b=0;
  write_ima_data.transform.c=0;
  write_ima_data.transform.d=-1;
  write_ima_data.transform.e=0;
  write_ima_data.transform.f=255;

  write_ima_data.debug = 0;

  dump_generic_callbacks( lines, &write_ima_callback_collect, &write_ima_callback_content_ref, (void *) &write_ima_data);

  return 1;
}
