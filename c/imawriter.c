#include "imawriter.h"

/**

WARNING NOT GENERATING IMA YET
NOT EVEN TESTED WITH TEXT DUMP

**/

struct dump_sdlines_data {
  struct outputstream * stream;
};

void dump_sdlines_content_ref_header(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  char * stream = ((struct dump_sdlines_data *) data)->stream;
  printf("{\"count\":%i",this->lines);
  printf(",\"lines\":\n");
}

void dump_sdlines_content_ref(struct sdlines * this, struct sdlines_iterator_callback * callback, struct vectlist * line, int index, struct vectlist * next, void * data)
{
  char * stream = ((struct dump_sdlines_data *) data)->stream;
  float* v;
  printf("[");
  v=&line->vector[0][0];
  printf("{\"x\"=%f,\"y\=%f}",v[0],v[1]);
  for ( int j=1; j<line->index;j++)
    {
      v=&line->vector[j][0];
      printf(",\n{\"x\"=%f,\"y\=%f}",v[0],v[1]);
    }
  printf("]");
  if ( next !=NULL)
    {
      printf(",\n");
    }
}

void dump_sdlines_content_ref_footer(struct sdlines * this, struct sdlines_iterator_callback * callback, void * data)
{
  printf("};\n");  
}

int write_ima(struct outputstream * stream, struct sdlines * lines)
{
  // should write header containing the number of points.

  // then points with special stop and start beam

  struct dump_sdlines_data  dump_sdlines_data;
  dump_sdlines_data.stream = stream;

  struct sdlines_iterator_callback dump_sdlines_callback_content_ref;
  dump_sdlines_callback_content_ref.f_before=dump_sdlines_content_ref_header;
  dump_sdlines_callback_content_ref.f_for_each=dump_sdlines_content_ref;
  dump_sdlines_callback_content_ref.f_after=dump_sdlines_content_ref_footer;

  sdlines_foreach(lines, &dump_sdlines_callback_content_ref, (void *) &dump_sdlines_data);

}
