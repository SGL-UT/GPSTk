#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Path.cpp#2 $"

/// @file Path.cpp Class to represent a series of points. Class definitions.

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

#include <cmath>
#include "Path.hpp"

namespace vdraw
{
   void Path::addPointRelative(double X, double Y)
   { 
      Point p(X,Y);
      this->push_back(p);
      return;
   }

   void Path::addPointAbsolute(double X, double Y)
   {
      addPointRelative(X - originX, Y-originY);
      return;
   }

   void Path::addPointDelta(double DX, double DY) throw(VDrawException)
   { 
      using namespace std;

      if (this->empty())
         throw VDrawException("Using relative operation on empty path list.");
  
      Path::iterator lastPair = this->end();
      lastPair--;
      Point p(lastPair->first-DX,lastPair->second-DY);
      this->push_back(p);
      return;
   }

   void Path::rotate(double angleDegrees, double rx, double ry)
   {
      using namespace std;

      double theta = angleDegrees*PI/180,
         costheta = cos(theta),
         sintheta = sin(theta);
      
      for (Path::iterator i=this->begin(); i!=this->end(); i++)
      {
         double dx = (i->first  + originX) - rx;
         double dy = (i->second + originY) - ry;

         i->first  = rx + dx*costheta - dy*sintheta;
         i->second = ry + dx*sintheta + dy*costheta;
      }
   }
   
   void Path::rotate(double angleDegrees)
   {
      this->rotate(angleDegrees, originX, originY);
   }

   void Path::translate(double deltaX, double deltaY)
   {
      originX += deltaX;
      originY += deltaY;      
   }

   std::auto_ptr<Path> Path::asAbsolute(void) const
   {
     std::auto_ptr<Path> absPath(new Path(*this));
      
      for (Path::iterator i=absPath->begin(); i!=absPath->end(); i++)
      {
         i->first  = i->first + originX;
         i->second = i->second + originY;
      }      

      return absPath;
   }
   
} // namespace vdraw
