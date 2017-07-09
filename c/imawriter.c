#include "imawriter.h"

/**

WARNING NOT GENERATING IMA YET
NOT EVEN TESTED WITH TEXT DUMP

**/

struct dump_sdlines_data {
  struct outputstream * stream;
  struct sdlines_matrix6 transform;
};

void write_ima_content_ref_header(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  struct outputstream * stream = ((struct dump_sdlines_data *) data)->stream;
  printf("{\"count\":%i",this->lines);
  printf(",\"lines\":\n");

  // write 32bits little endian count of bytes ( 2 * points ) ( start / stop beam included ) with this size included ( ie + 4 ).
  // FIXME, length is not yet known , we have to collect number of points for each line ...
  fprintf(stream->file,"%c%c",IMA_X_BEAN_CONTROL,IMA_Y_BEAN_OFF);  
}

void write_ima_content_ref(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  struct dump_sdlines_data * sdlines_data =  (struct dump_sdlines_data *) data;
  struct outputstream * stream = sdlines_data->stream;
  float* v;
  XPoint dest;
  printf("[");
  v=&line->vector[0][0];
  printf("{\"x\"=%f,\"y\=%f}",v[0],v[1]);
  fprintf(stream->file,"%c%c",IMA_X_BEAN_CONTROL,IMA_Y_BEAN_OFF);
  sdlines_matrix6_apply_matrix(& sdlines_data->transform,v,&dest);
  fprintf(stream->file,"%c%c",(unsigned char)dest.x,(unsigned char)dest.y);
  fprintf(stream->file,"%c%c",IMA_X_BEAN_CONTROL,IMA_Y_BEAN_ON);
  for ( int j=1; j<line->index;j++)
    {
      v=&line->vector[j][0];
      printf(",\n{\"x\"=%f,\"y\=%f}",v[0],v[1]);
      sdlines_matrix6_apply_matrix(& sdlines_data->transform,v,&dest);
      fprintf(stream->file,"%c%c",(unsigned char)dest.x,(unsigned char)dest.y);
    }
  printf("]");
  if ( next !=NULL)
    {
      printf(",\n");
    }
}

void write_ima_content_ref_footer(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  printf("};\n");  
}

int write_ima(struct outputstream * stream, struct sdlines * lines)
{
  // should write header containing the number of points.

  // then points with special stop and start beam

  struct dump_sdlines_data  write_ima_data;
  write_ima_data.stream = stream;

  write_ima_data.transform.a=1;
  write_ima_data.transform.b=0;
  write_ima_data.transform.c=0;
  write_ima_data.transform.d=1;
  write_ima_data.transform.e=0;
  write_ima_data.transform.f=0;
  
  struct sdlines_iterator_callback write_ima_callback_content_ref;
  write_ima_callback_content_ref.f_before=write_ima_content_ref_header;
  write_ima_callback_content_ref.f_for_each=write_ima_content_ref;
  write_ima_callback_content_ref.f_after=write_ima_content_ref_footer;

  sdlines_foreach(lines, &write_ima_callback_content_ref, (void *) &write_ima_data);

}
