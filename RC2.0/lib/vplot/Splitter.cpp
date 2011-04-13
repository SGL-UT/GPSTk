#pragma ident "$Id$"

/// @file Splitter.cpp Used to help with splitting sets of points. Class 
/// definitions.

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#include "Splitter.hpp"
#include <list>

using namespace std;
using namespace vdraw;

namespace vplot
{
  pair<list<Path*>*, list<Path*>*> Splitter::splith(double splitter, Path* p, bool top, bool bottom, bool continuous)
  {
    if(!(top || bottom) || p->empty())
      return pair<list<Path* >*, list<Path*>*>((list<Path* >*)0,(list<Path* >*)0);

    list<Path*> *tl, *bl;
    if(top) 
      tl = new list<Path*> ();
    if(bottom)
      bl = new list<Path*> ();

    Path* current = new Path(0,0);

    Path::iterator i=p->begin();
    bool above = i->second > splitter;

    if(above && top) current->addPointAbsolute(i->first, i->second);
    else if(!above && bottom) current->addPointAbsolute(i->first, i->second);

    i++;

    double tdouble = 0;
    for (;i != p->end(); i++)
    {
      if (above && (i->second < splitter))
      {
        if(top)
        {
          if(continuous) 
          {
            Path::iterator j = i;
            j--;
            tdouble = intersecth(splitter,*i,*j);
            current->addPointAbsolute(tdouble,splitter);
          }

          if(!current->empty()) tl->push_back(current);
          current = new Path(0,0);

          if(continuous) 
            current->addPointAbsolute(tdouble,splitter);
        }
        above = false;
      }
      else if(!above && (i->second > splitter))
      {
        if(bottom)
        {
          if(continuous) 
          {
            Path::iterator j = i;
            j--;
            tdouble = intersecth(splitter,*i,*j);
            current->addPointAbsolute(tdouble,splitter);
          }

          if(!current->empty()) bl->push_back(current);
          current = new Path(0,0);

          if(continuous) 
            current->addPointAbsolute(tdouble,splitter);
        }
        above = true;
      }
      else if(i->second == splitter)
      {
        Path::iterator j=i;
        j++;

        /*
         * Only add, push, and make a new path if it doesn't "bounce" off of the
         * splitter.  
         * So we do a lookahead of one with j.
         * We will _skip_ this area iff:
         * - j is at the end of p
         * - (i-1) is above and j is below
         * - (i-1) is below and j is above
         */
        if( (j != p->end()) && 
            ((above && (j->second < splitter)) ||
             (!above && (j->second > splitter))) )
        {
          current->addPointAbsolute(i->first, i->second);
          if (above && top)
            tl->push_back(current);
          else if(!above && bottom)
            bl->push_back(current);
          current = new Path(0,0);
          above = !above;
        }
      }
      if ((above && top) || (!above && bottom))
        current->addPointAbsolute(i->first, i->second);
    }

    if(!current->empty())
    {
      if(above && top) tl->push_back(current);
      else if(!above && bottom) bl->push_back(current);
    }

    return pair<list<Path*>*, list<Path*>*>(tl, bl);
  }

  list<Path*>* Splitter::splitvgap(double gap, Path* p)
  {
    if((p==0)||(p->empty()))
      return 0;

    list<Path*> *paths = new list<Path*> ();

    Path* current = new Path(0,0);

    Path::iterator i=p->begin();
    Path::iterator last=i;
    current->addPointAbsolute(i->first, i->second);
    i++;

    gap = (gap<0?-gap:gap);

    for (;i != p->end(); i++, last++)
    {
      double dist = i->first - last->first;
      dist = (dist<0?-dist:dist);
      if (dist >= gap)
      {
        paths->push_back(current);
        current = new Path(0,0);
      }
      current->addPointAbsolute(i->first, i->second);
    }

    if(!current->empty())
      paths->push_back(current);

    return paths;
  }

  std::pair<double,double> Splitter::intersectBox(
      const std::pair<double,double> inside, const std::pair<double,double> outside, 
      double minX, double maxX, double minY, double maxY)
  {
    double x,y;
    if(outside.first < minX)
    {
      x = minX;        
      y = intersectv(x, inside, outside);
      if(y>minY && y<maxY) return std::pair<double,double>(x,y);        
    }
    else if(outside.first > maxX)
    {
      x = maxX;
      y = intersectv(x, inside, outside);
      if(y>minY && y<maxY) return std::pair<double,double>(x,y);
    }

    if(outside.second < minY)
    {
      y = minY;
      x = intersecth(y, inside, outside);
      return std::pair<double,double>(x,y);    
    }
    else if(outside.second > maxY)
    {
      y = maxY;
      x = intersecth(y, inside, outside);
      return std::pair<double,double>(x,y);
    }

    // outside isn't out of the box, maybe inside is...
    if(!inBox(inside,minX,maxX,minY,maxY))
      return intersectBox(outside,inside,minX,maxX,minY,maxY);

    // Neither outside nor inside are out of the box.  Just give back one of
    // the points (the first)
    return inside;
  }

  std::auto_ptr< std::list< vdraw::Path > > Splitter::interpToBox(double minX, double maxX, double minY, double maxY, const vdraw::Path& p)
  {
    using namespace vdraw;
    Path::const_iterator i=p.begin(),j=p.begin();
    bool inside = inBox(*i,minX,maxX,minY,maxY);
    bool lastinside = inside;

    double cx, cy;
    p.getOrigin(cx,cy);

    std::auto_ptr< std::list< Path > > thelist(new std::list<Path>());
    Path current(cx,cy);
    if(inside) 
      current.push_back(*i);
    i++;

    for(;i!=p.end();i++,j++,lastinside=inside)
    {
      inside = inBox(*i,minX,maxX,minY,maxY);
      if(!inside && lastinside)
      {
        current.push_back(intersectBox(*j,*i,minX,maxX,minY,maxY));
        thelist->push_back(current);
        current = Path(cx,cy);
      }
      else if(inside && !lastinside)
      {
        current.push_back(intersectBox(*i,*j,minX,maxX,minY,maxY));     
        current.push_back(*i);
      }
      else if(inside)
      {
        current.push_back(*i);
      }
    }

    if(current.size() != 0) 
      thelist->push_back(current);

    return thelist;
  }

  std::auto_ptr< vdraw::Path > Splitter::cropToBox(double minX, double maxX, double minY, double maxY, const vdraw::Path& p)
  {
    using namespace vdraw;
    Path::const_iterator i=p.begin();
    bool inside = inBox(*i,minX,maxX,minY,maxY);
    std::auto_ptr< Path >  newpath(new Path(0,0,p.size()));
    if(inside) newpath->push_back(*i);
    i++;
    for(;i!=p.end();i++)
    {
      inside = inBox(*i,minX,maxX,minY,maxY);
      if(inside) newpath->push_back(*i);
    }
    newpath->tighten();
    return newpath;
  }
}
