#include "svgpath.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXPATH 65536
int debug_svgpath=0;

void dump_progress(struct pathparser * pathparser)
{
  printf("pathparser index %i bezier %i lastpoint (x,y)=(%lf,%lf) \n",
	 pathparser->index,pathparser->bezier,
	 pathparser->lastpoint.x,pathparser->lastpoint.y	 
	 );
}

int is_bezier(struct svgpath_element * svgpath_element)
{
  return (svgpath_element->mode == 'c') || (svgpath_element->mode == 'C');
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
  pathparser->line.points=0;
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
 allocate a new bezier and add it in list as a bezier
*/
int svgpath_addbezier(struct svgpath * svgpath, struct bezier_cubic * bezier)
{
  struct allistof * element_list = allistcontext_get_membership(svgpath->svgpathcontext,0);
  struct svgpath_element * new_element = malloc(sizeof(struct svgpath_element));
  new_element->mode='c';
  memcpy(&new_element->bezier,bezier,sizeof(*bezier));
  struct allistelement * element = allistcontext_new_allistelement(svgpath->svgpathcontext, new_element);
  struct allistelement * elementadded = allistelement_add_in(element, element_list);
  if ( debug_svgpath==1) { printf("element bezier_cubic added %p\n", elementadded);};
  return 0;
}

/**
 allocate a new svgpath_line and add it in list as a bezier
*/
int svgpath_addline(struct svgpath * svgpath, struct svgpath_line * line)
{
  struct allistof * element_list = allistcontext_get_membership(svgpath->svgpathcontext,0);
  struct svgpath_element * new_element = malloc(sizeof(struct svgpath_element));
  new_element->mode='M';
  memcpy(&new_element->line,line,sizeof(*line));
  struct allistelement * element = allistcontext_new_allistelement(svgpath->svgpathcontext, new_element);
  struct allistelement * elementadded = allistelement_add_in(element, element_list);
  if ( debug_svgpath==1) { printf("element svgpath_line added %p\n", elementadded);};
  return 0;
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
      // flush remainings line
      if ( pathparser->line.points > 0)
	{
	  svgpath_addline(svgpath,&pathparser->line);
	  pathparser->line.points=0;
	}	  

      // ZERO -> END
      return 0;
    }
  if (pathparser->start[index+1] == ' ')
    {
      pathparser->mode = pathparser->start[index];
      switch(pathparser->mode)
	{
	case 'm':
	  pathparser->absolute=0; 
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
	  // flush remainings line
	  if ( pathparser->line.points > 0)
	    {
	      svgpath_addline(svgpath,&pathparser->line);
	      pathparser->line.points=0;
	    }	  

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
	  // bezier == 0 => this is a lineto
	  pathparser->lastpoint.x=thispoint.x;
	  pathparser->lastpoint.y=thispoint.y;
	  pathparser->line.point[pathparser->line.points].x=thispoint.x;
	  pathparser->line.point[pathparser->line.points].y=thispoint.y;

	  pathparser->line.points=pathparser->line.points+1;
	  if ( pathparser->line.points >SVGPATH_LINE_POINTS)
	    {
	      // should add this lineto [ of pathparser->line.points ]
	      svgpath_addline(svgpath,&pathparser->line);
	      pathparser->line.points=0;
	    }	  
	}
      return 1;
    }
}

