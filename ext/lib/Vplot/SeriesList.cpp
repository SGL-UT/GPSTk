//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/// @file SeriesList.cpp Used to maintain series information for some types of
/// plots. Class definitions.

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
      vector<pair<double, double> >::iterator it;
      for (it = series.begin(); it!=series.end(); it++)
      {
        double x = it->first;
        double y = it->second;
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

  void SeriesList::drawLegend(Frame& frame, double pointsize, unsigned int columns)
  {
    if(columns <= 1)
      drawLegendSegment(frame,pointsize,0,titles.size());
    else
    {
      // Make a grid with one row and the correct number of columns
      GridLayout gl(frame,1,columns);
      // The number of elements in each column will be the same for all
      // columns but the last.
      unsigned int n = (titles.size()/columns) + (titles.size()%columns?1:0);
      // Add the legend segment for each column
      for(unsigned int i=0; i<columns; i++)
      {
        Frame t(gl.getFrame(0,i));
        drawLegendSegment(t,pointsize,i*n,min(n,(unsigned int)(titles.size()-i*n)));
      }
    }
  }

  void SeriesList::drawLegendSegment(Frame& frame, double pointsize, 
      unsigned int begin, unsigned int n)
  {
    // If we aren't drawing anything, don't bother with all the effort :)
    if(n == 0)
      return;
    // Spacer is the number of points between the drawn segment and the text
    // of the label as well as the added number of spacing between each
    // series in the list
    double spacer = 5;
    // Test to see if we are dealing with a scatter plot or not
    // as this determines what is drawn on the left
    bool lines = false;
    double mwidth = 0;
    double height = pointsize;
    for(unsigned int i=0;i<styles.size();i++)
    {
      if(!markers[i].getColor().isClear())
      {
        mwidth = max(mwidth,markers[i].getRange()*2);
        height = max(height,mwidth);
      }

      if(!styles[i].getColor().isClear())
        lines = true;
    }
    // Add spacer to the height
    height += spacer;
    // width = width needed to draw this
    // lbegin = x offset where line will begin
    // lwidth = length of the lines to draw the sample
    double width = 30;
    double lbegin = 0;
    double lwidth = 30;
    if(mwidth) // If we have markers...
    {
      lbegin = mwidth/2; 
      if(lines)
      {
        width = mwidth*3;
        lwidth = mwidth*2;
      }
      else
      {
        width = mwidth;
        lwidth = 0;
      }
    }
    // TextStyle
    TextStyle style;
    style.setPointSize(pointsize);

    // Begin drawing?
    for(unsigned int i=begin;i<begin+n;i++)
    {
      // Draw the sample:
      double y = frame.getHeight() - height/2.0 - height*i;
      Line l(lbegin+spacer,y,lbegin+lwidth+spacer,y);
      l.setStrokeStyle(styles[i]);
      l.setMarker(markers[i]);
      frame << l;

      // Add the series title
      frame <<  Text(titles[i].c_str(),width+spacer*2,(y-(pointsize/2.0)),style,Text::LEFT);
    }
  }
}
