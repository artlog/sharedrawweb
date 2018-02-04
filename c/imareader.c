/**
.IMA format is an old proprietary format for laser show program
it contain a list of point the laser beam should goe through,
x=255 has a special meaning to hide (255,254) ( send beam into the box) or show it (255,255): let is flow outside.

very old code : 

int LitGraph(char *chemin,BYTE* buffer)
{
int nboct,i;
int fimage;
fimage=open(chemin,O_RDONLY|O_BINARY);
nboct = filelength(fimage);
if (!(eof(fimage))) read(fimage,buffer,nboct);
close(fimage);
return(nboct);
}

int SauvGraph(char *chemin,BYTE* buffer)
{
int nboct,i;
int fimage;
nboct=4+*(WORD*)buffer;
fimage=open(chemin,O_WRONLY|O_BINARY|O_CREAT,S_IREAD|S_IWRITE);
write(fimage,buffer,nboct);
close(fimage);
return(0);
}

void NewImage(BYTE* buffer)
{
*((WORD*)buffer)=2;
buffer[2]=0;
buffer[3]=0xC8;
buffer[4]=255;
buffer[5]=254;
}

setcolor(couleur2);
maff= 0;
nboct=4+*(WORD*)buffer;
if (nboct>4)
{
	for(i=4;i<nboct;i+=2)
	{
	x = buffer[i];
	y = 255-buffer[i+1];
	if (x==255)
	   {
	   if (y==1) setcolor(couleur1) ;
	   if (y==0) setcolor(couleur2) ;
	   }
	else
	   {
	   if (compress) {x=transp(x-dcx,lx);y=transp(y-dcy,ly);}
	   if (maff==0) moveto(x,y); else lineto(x,y);
	   maff=1;
	   }
	}      // fin du loop d'affichage de points
}

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "imareader.h"

// Might look weird because core part was ported from java.

typedef int bool;
const int FALSE=0;
const int TRUE=1;

struct sharedrawingpoint {
  int x;
  int y;
  struct sharedrawingpoint * next;
};

struct sharedrawingline {
  int points;
  struct sharedrawingpoint * firstpoint;
  struct sharedrawingpoint * lastpoint;
  struct sharedrawingline * next;
};

struct sharedrawing {
  int lines;
  struct sharedrawingline * firstline;
  struct sharedrawingline * lastline;
};

struct imaimporter {
  struct alinputstream * mStream;
  bool mBeanOn;
  int mIndex;
  bool mDebug;
};

int filename_is_ima(char * filename)
{
  int length=strlen(filename);
  if ( length > 3 )
    {
      return strncasecmp(&filename[length-4],".ima",4) == 0;
    }
  return 0;
}

struct alinputstream * newInputstream(int fd)
{
  struct alinputstream * stream=malloc(sizeof(struct alinputstream));
  if ( stream != NULL )
    {
      alinputstream_init(stream,fd);
    }
  return stream;
}

unsigned char readUnsignedByte(struct alinputstream * stream)
{
  return alinputstream_readuchar(stream);
}

struct sharedrawing * newShareDrawing()
{
  return calloc(1,sizeof(struct sharedrawing));
}

struct sharedrawingline * newShareDrawingLine()
{
  return calloc(1,sizeof(struct sharedrawingline));
}

struct sharedrawingpoint * newPoint(int x, int y)
{
  struct sharedrawingpoint * newpoint = calloc(1,sizeof(struct sharedrawingpoint));
  if ( newpoint != NULL )
    {
      newpoint->x=x;
      newpoint->y=y;
    }
  return newpoint;
}

void addPoint(struct sharedrawingline * line,  struct sharedrawingpoint * point)
{
  if ( line->firstpoint == NULL )
    {
      line->firstpoint = point;
    }
  else
    {
      line->lastpoint->next=point;
    }
  line->lastpoint=point;
  line->points ++;
}

int addPointXY(struct sharedrawingline * line,  int x, int y)
{
  if ( line->firstpoint != NULL )
    {
      // skip duplicates.
      if ( (line->lastpoint->x == x) && (line->lastpoint->y == y))
	{
	  return 0;
	}
    }
  addPoint(line, newPoint(x,y));
  return 1;
}

void addLine(struct imaimporter * this, struct sharedrawing * drawing, struct sharedrawingline * line )
{  
  if ( drawing->firstline == NULL )
    {
      drawing->firstline = line;
    }
  else
    {
      drawing->lastline->next = line;
    }
  drawing->lastline=line;
  if ( this->mDebug )
    {
      printf("line[%i] %i points \n", drawing->lines, line->points);
    }
  drawing->lines ++;
}
	     
void imaimporter_init(struct imaimporter * this, struct alinputstream * inputStream)
{
  this->mStream=inputStream;
  this->mBeanOn=FALSE;
  this->mIndex=0;
  this->mDebug=FALSE;
}

void imaimporter_importInto_sharedrawing (struct imaimporter * this, struct sharedrawing * drawing)
{
  int x,y;
  struct sharedrawingline * line = NULL;
  this->mBeanOn = FALSE;

  // little endian unsigned 16bits
  int byteCount = readUnsignedByte(this->mStream);
  byteCount = ( 256 * readUnsignedByte(this->mStream) ) + byteCount;
  
  // here should read 0xC8 then 0x00
  
  if ( this->mDebug )
    {
      printf("byte count: %i\n",byteCount);
    }
  for (int j = 0 ; j < byteCount; j+=2)
    {
      this->mIndex=j;
      x = readUnsignedByte(this->mStream);
      y = readUnsignedByte(this->mStream);
      // special beam on/off
      if ( ( x == IMA_X_BEAN_CONTROL ) &&
	   (( y == IMA_Y_BEAN_OFF ) || ( y == IMA_Y_BEAN_ON )) )
	{
	  this->mBeanOn = ( y == IMA_Y_BEAN_ON );
	  if ( this->mDebug )
	    {
	      printf("beam change at %i %i\n",this->mIndex,this->mBeanOn);
	    }
	}
      else
	{
	  y = 255-y;
	  if ( this->mDebug )
	    {
	      printf("point[%i]={%i}(%i,%i)\n",this->mIndex,this->mBeanOn,x,y);
	    }					
	  if ( this->mBeanOn )
	    {
	      if (line == NULL )
		{
		  line = newShareDrawingLine();
		}
	      int added=addPointXY(line,x,y);
	      if ((!added) && ( this->mDebug ))
		{
		  printf("point (%i,%i) duplicate, skipped\n", x,y);
		}
	    }
	  else
	    {
	      if ( line != NULL )
		{
		  addLine(this,drawing,line);
		  line = NULL;
		}
	    }
	}
    }
  if ( line != NULL )
    {
      addLine(this,drawing,line);
      line = NULL;
    }
  if ( this->mDebug )
    {
      printf("lines: %i\n",drawing->lines);
    }  
}

int convert_sharedrawing_to_sdlines(struct imaimporter * importer, struct sharedrawing * drawing, struct sdlines * lines)
{
  int line_idx=0;
  int point_idx=0;
  struct sharedrawingline * line = drawing->firstline;
  struct sharedrawingpoint * point = NULL;

  // walk lines
  for (line_idx = 0; (line !=NULL) && (line_idx < drawing->lines); line_idx ++)
    {      
      if (line != NULL)
	{
	  struct vectlist* vect = vectlist_new(line->points);
	  if ( vect != NULL )
	    {
	      // walk points
	      point=line->firstpoint;
	      for (point_idx =0; (point !=NULL) && (point_idx < line->points); point_idx ++)
		{
		  // got point point_idx in line line_idx;
		  //
		  if ( importer->mDebug)
		    {
		      printf("add (%i,%i)\n", point->x,point->y);
		    }
		  vect->vector[point_idx][0]=point->x;
		  vect->vector[point_idx][1]=point->y;
		  vect->vector[point_idx][2]=0;
 
		  point=point->next;
		}
	      if ( importer->mDebug)
		{
		  printf("add %i points in line %i\n",line->points,line_idx);
		}
	      vect->index=line->points;
	      sdlines_add_vectlist(lines,vect);
	    }
	}
      line=line->next;
    }
  lines->lines=drawing->lines;
  return -2;  
}

void dispose_sharedrawing_content(struct sharedrawing * drawing)
{
  int line_idx=0;
  int point_idx=0;
  struct sharedrawingline * line = drawing->firstline;
  struct sharedrawingpoint * point = NULL;

  // walk lines
  for (line_idx = 0; (line !=NULL) && (line_idx < drawing->lines); line_idx ++)
    {
      struct sharedrawingline * nextline = line->next;
      if (line != NULL)
	{
	  struct vectlist* vect = vectlist_new(line->points);
	  if ( vect != NULL )
	    {
	      // walk points
	      point=line->firstpoint;
	      for (point_idx =0; (point !=NULL) && (point_idx < line->points); point_idx ++)
		{		  
		  struct sharedrawingpoint * nextpoint = point->next;
		  free(point);
		  point=nextpoint;
		}
	    }
	}
      free(line);
      line=nextline;
    } 
}

int read_ima(struct alinputstream * stream, struct sdlines * lines)
{
  struct imaimporter imaimporter;
  struct sharedrawing drawing;
  int result = 0;

  drawing.firstline=NULL;
  drawing.lastline=NULL;
  drawing.lines=0;

  imaimporter_init(&imaimporter,stream);
  imaimporter.mDebug = ( stream->debug > 0 );
  imaimporter_importInto_sharedrawing (&imaimporter, &drawing);
  result=convert_sharedrawing_to_sdlines(&imaimporter,&drawing,lines);
  dispose_sharedrawing_content(&drawing);
  return result;
}
