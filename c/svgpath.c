#include "svgpath.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXPATH 65536
int debug_svgpath=1;

void dump_progress(struct pathparser * pathparser)
{
  printf("pathparser index %i bezier %i lastpoint (x,y)=(%lf,%lf) \n",
	 pathparser->index,pathparser->bezier,
	 pathparser->lastpoint.x,pathparser->lastpoint.y	 
	 );
}

/**
returns
 0: if all went fine
 <0 if error
 >0 => this should not occur.
*/
int svgpath_parse(struct svgpath * svgpath, char * path, int length, struct svgpoint * start)
{
  struct pathparser pathparser;
  int parsecode=0;
  int watchguard=0;
  if (svgpath->svgpathcontext == NULL )
    {
      svgpath->svgpathcontext = new_allistcontext(1);
    }
  if ( svgpath->svgpathcontext != NULL )
    {
      struct allistof * svglist =  new_allistof(svgpath->svgpathcontext);
      if ( debug_svgpath==1) { printf("svglist allocated %p\n", svglist);};
    }
  else
    {
      return -3;
    }

  pathparser_init(&pathparser, path, length, start );
  while ( ( (parsecode=pathparser_nexttoken(&pathparser,svgpath)) > 0 ) && (watchguard < MAXPATH))
    {
      // CoOl
      dump_progress(&pathparser);
      ++watchguard;
    }

  if (watchguard >= MAXPATH)
    {
      if (debug_svgpath) { fprintf(stderr,"[ERROR] too many path, loop protection activated for '%s'", path);};
      return -4;
    }

  if ( start != NULL)
    {
      start->x=pathparser.lastpoint.x;
      start->y=pathparser.lastpoint.x;
    }
  
  return parsecode;
}

int pathparser_init(struct pathparser * pathparser, char * path, int length, struct svgpoint * start)
{
  pathparser->start=path;
  pathparser->index=0;
  pathparser->bezier=0;
  pathparser->absolute=1;
  if ( start == NULL )
    {
      pathparser->lastpoint.x=0;
      pathparser->lastpoint.y=0;
    }
  else
    {
      pathparser->lastpoint.x=start->x;
      pathparser->lastpoint.y=start->y;
    }
  return 0;
}

int pathparser_parsepoint(struct pathparser * pathparser, struct svgpoint * svgpoint)
{
  double x;
  double y;
  char * s = &pathparser->start[pathparser->index];
  // coma or space are valid !
  char * next = strchr(s, ' ');
  char * coma = strchr(s, ',');
  if ( next != NULL)
    {
      int tokens = 0;
      // coma before space
      if ((coma > 0) && ( coma < next ))
	{
	  tokens=sscanf(s,"%lf,%lf ",&x,&y);	  
	}
      else
	{
	  tokens=sscanf(s,"%lf %lf ",&x,&y);
	  next = strchr(next+1, ' ');
	}
      if ( tokens == 2 )
	{
	  printf("(%f,%f),",x,y);
	  if ( next != NULL)
	    {
	      pathparser->index=(int) (1 + (next - pathparser->start));
	    }
	  else
	    {
	      return -1;
	    }
	  if (pathparser->absolute == 1 )
	    {
	      svgpoint->x=x;
	      svgpoint->y=y;
	    }
	  else
	    {
	      svgpoint->x=x+pathparser->lastpoint.x;
	      svgpoint->y=y+pathparser->lastpoint.y;
	    }
	  return 0;
	}
    }
  return -2;
}

/**
allocate a new bezier and add it in list.
*/
int svgpath_addbezier(struct svgpath * svgpath, struct bezier_cubic * bezier)
{
  struct allistof * bezier_list = allistcontext_get_membership(svgpath->svgpathcontext,0);
  struct bezier_cubic * new_bezier = malloc(sizeof(struct bezier_cubic));
  memcpy(new_bezier,bezier,sizeof(*new_bezier));
  struct allistelement * element = allistcontext_new_allistelement(svgpath->svgpathcontext, new_bezier);
  struct allistelement * elementadded = allistelement_add_in(element, bezier_list);
  if ( debug_svgpath==1) { printf("element added %p\n", elementadded);};
  return -2;
}

/**
returns
0 : end
1 : ok token read, please continue
< 0 : error
**/
int pathparser_nexttoken(struct pathparser * pathparser, struct svgpath * svgpath)
{
  int index = pathparser->index;

  if (debug_svgpath>0) { printf("parse next token %s\n", &pathparser->start[index]);}
  
  if (pathparser->start[index] == 'z' )
    {
      // END !
      pathparser->bezier=0;
      pathparser->absolute=1;
      // ZERO -> END
      return 0;
    }
  if (pathparser->start[index+1] == ' ')
    {
      switch(pathparser->start[index])
	{
	case 'm':
	  pathparser->absolute=1; // testing... 
	  pathparser->bezier=0;
	  pathparser->index=index+2;
	  break;
	case 'M':
	  pathparser->absolute=1;
	  pathparser->bezier=0;
	  pathparser->index=index+2;
	  break;
	case 'c':
	  pathparser->absolute=0;
	  pathparser->bezier=1;
	  pathparser->index=index+2;
	  break;
	case 'C':
	  pathparser->absolute=1;
	  pathparser->bezier=1;
	  pathparser->index=index+2;
	  break;
	default:
	  if ( debug_svgpath) {fprintf(stderr, "Unexpected char '%c' at %i\n",pathparser->start[index],index);}
	  return -1;
	}
      // continue
      return 1;
    }
  else
    {
      // subtle 'c' : current point for relative is NOT affected by intermediate handles.
      struct svgpoint thispoint;
      if ( pathparser_parsepoint(pathparser, &thispoint) != 0 )
	{
	  if ( debug_svgpath>0) {fprintf(stderr, "parsing failed at %i\n", index);}
	  return -1;
	}
      if (( pathparser->bezier >= 0) && ( pathparser->bezier < 4))
	{
	  pathparser->bezier_cubic.point[pathparser->bezier].x=thispoint.x;
	  pathparser->bezier_cubic.point[pathparser->bezier].y=thispoint.y;
	}
      else
	{
	  // should never be negative or > 3.
	  if ( debug_svgpath>0) {fprintf(stderr, "bezier index exceed accepted limits 0<= %i <4\n", pathparser->bezier);}
	  return -1;
	}
      if ( pathparser->bezier > 0)
	{
	  ++pathparser->bezier;
	  // we got a full bezier definition
	  if (pathparser->bezier >3)
	    {
	      // should add this cubic bezier
	      svgpath_addbezier(svgpath,&pathparser->bezier_cubic);
	      pathparser->bezier=1;
	      pathparser->lastpoint.x=thispoint.x;
	      pathparser->lastpoint.y=thispoint.y;
	      pathparser->bezier_cubic.point[0].x=pathparser->lastpoint.x;
	      pathparser->bezier_cubic.point[0].y=pathparser->lastpoint.y;
	    }
	}
      else
	{
	  // bezier == 0 ?
	  pathparser->lastpoint.x=thispoint.x;
	  pathparser->lastpoint.y=thispoint.y;
	  pathparser->bezier=1;
	  // lineto ??
	}
      return 1;
    }
}

