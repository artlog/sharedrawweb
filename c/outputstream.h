#ifndef _OUTPUTSTREAM_H_
#define _OUTPUTSTREAM_H_

struct outputstream {
  int fd;
  int debug;
};

void outputstream_init(struct outputstream * stream, int fd);

void outputstream_writeint32(struct outputstream * stream, int word);

#endif
