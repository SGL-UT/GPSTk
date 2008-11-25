#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/plot/SeriesList.cpp#5 $"

/// @file SeriesList.cpp Used to maintain series information for some types of
/// plots. Class definitions.

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

#include <algorithm>

#include "SeriesList.hpp"
#include "Splitter.hpp"

using namespace std;
using namespace vdraw;

namespace vplot
{
  void SeriesList::findMinMax(double& minX, double &maxX, double& minY, double& maxY)
  {
    minX = DBL_MAX;
    maxX = DBL_MIN;
    minY = DBL_MAX;
    maxY = DBL_MIN;

    for(unsigned int i=0; i<pointlists.size(); i++)
    {
      vector<pair<double, double> >& series = pointlists[i];
      vector<pair<double, double> >::iterator i;
      for (i = series.begin(); i!=series.end(); i++)
      {
        double x = i->first;
        double y = i->second;
        maxX = (x>maxX ? x : maxX); 
        minX = (x<minX ? x : minX); 
        maxY = (y>maxY ? y : maxY); 
        minY = (y<minY ? y : minY); 
      } 
    } 
  }

  void SeriesList::drawInFrame(Frame& innerFrame, double minX, double maxX, double minY, double maxY)
  {
    double multX = innerFrame.getWidth()/(maxX-minX);
    double multY = innerFrame.getHeight()/(maxY-minY);

    // Draw lines
    for(int i=0;i<getNumSeries();i++)
    {
      innerFrame.push_state();

      StrokeStyle s = getStyle(i);          
      Marker m = getMarker(i);

      if(m.getColor().isClear() && s.getColor().isClear())
      {
        innerFrame << Comment("Plot contained data with clear stroke and marker.  Skipping.");
        continue;
      }

      vector< pair<double,double> >& vec = getPointList(i);
      Path curve(vec,innerFrame.lx(), innerFrame.ly());

      // What I'd give for a line of haskell...
      // map (\(x,y) -> (multX*(x-minX), multY*(y-minY))) vector
      map_object map_instance(multX,minX,multY,minY);

      innerFrame.setMarker(m);
      innerFrame.setLineStyle(s);

      if(s.getColor().isClear())
      {
        // crop
        auto_ptr< Path > cropX = Splitter::cropToBox(minX,maxX,minY,maxY,curve);

        // Fit it to the box.
        std::for_each(cropX->begin(), cropX->end(), map_instance);

        // Draw the line
        innerFrame.line(*cropX);
      }
      else 
      {
        // interpolate
        auto_ptr< std::list<Path> > interpX = Splitter::interpToBox(minX,maxX,minY,maxY,curve);

        for(std::list<Path>::iterator i=interpX->begin();i!=interpX->end();i++)
        {
          // Fit it to the box
          std::for_each(i->begin(), i->end(), map_instance);

          // Draw the line
          innerFrame.line(*i);
        }
      }
      innerFrame.pop_state();
    }
  }
}
