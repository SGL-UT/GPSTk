#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/plot/SeriesList.hpp#6 $"

/// @file SeriesList.hpp Used to maintain series information for some types of
/// plots. Class declarations.

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

#ifndef VPLOT_SERIESLIST_H
#define VPLOT_SERIESLIST_H

#include <string>
#include <vector>
#include <cfloat>

#include "Frame.hpp"
#include "StrokeStyle.hpp"
#include "Marker.hpp"

using namespace std;
using namespace vdraw;

namespace vplot
{
  /**
   * This class will maintain a list of series including their title and
   * strokestyle.  This allows the plot creator to choose a specific stroke
   * style for any or all of the lines.
   */
  class SeriesList
  {
    public:
      /**
       * Constructor.
       */
      SeriesList() 
      {
      }

      /// Destructor
      ~SeriesList()
      {
      }

      /**
       * @param title Title of this series for the legend
       * @param points Points contained in this series
       * @param ss StrokeStyle for this series
       */
      void addSeries(std::string& title, vector< pair<double,double> >& points, StrokeStyle& ss)
      {
        Marker m = Marker::clear();
        addSeries(title,points,ss,m);
      }

      /**
       * @param title Title of this series for the legend
       * @param points Points contained in this series
       * @param m Marker for this series
       */
      void addSeries(std::string& title, vector< pair<double,double> >& points, Marker& m)
      {
        StrokeStyle ss = StrokeStyle::clear();
        addSeries(title,points,ss,m);
      }

      /**
       * @param title Title of this series for the legend
       * @param points Points contained in this series
       * @param ss StrokeStyle for this series
       * @param m Marker for this series
       */
      void addSeries(std::string& title, vector< pair<double,double> >& points, StrokeStyle& ss, Marker& m)
      {
        titles.push_back(title);
        pointlists.push_back(points);
        styles.push_back(ss);
        markers.push_back(m);
      }

      /// Set the title of the last-added series
      bool setLastTitle(std::string& newtitle)
      {
        return setTitle(titles.size()-1,newtitle);
      }

      /// Set the title of the idx-th series to newtitle
      bool setTitle(unsigned int idx, std::string& newtitle)
      {
        if(idx>=titles.size())
          return false;          
        titles[idx]=newtitle;
        return true;
      }

      /// Set the StrokeStyle of the last-added series
      bool setLastStyle(StrokeStyle& ss)
      {
        return setStyle(styles.size()-1,ss);
      }

      /// Set the StrokeStyle of the idx-th series to ss
      bool setStyle(unsigned int idx, StrokeStyle& ss)
      {
        if(idx>=styles.size())
          return false; 
        styles[idx]=ss;
        return true;
      }

      /// Set the Marker of the last-added series
      bool setLastMarker(Marker& m)
      {
        return setMarker(markers.size()-1,m);
      }

      /// Set the Marker of the idx-th series to m
      bool setMarker(unsigned int idx, Marker& m)
      {
        if(idx>=styles.size())
          return false; 
        markers[idx]=m;
        return true;
      }

      /// Get the total number of series
      int getNumSeries() { return pointlists.size(); }

      /// Return the title 
      std::string getTitle(int idx) { return titles[idx]; }

      /// Return the strokestyle
      StrokeStyle getStyle(int idx) { return styles[idx]; }

      /// Return the      void f(Point& p)
      Marker getMarker(int idx)     { return markers[idx]; }

      /// Return a pointer to the list of points
      vector< pair<double,double> >& getPointList(int idx) { return pointlists[idx]; }

      /// Return the minimums and maximum of all the data.
      void findMinMax(double& minX, double &maxX, double& minY, double& maxY);

      /// Draw all of the series in innerFrame
      void drawInFrame(Frame& innerFrame, double minX, double maxX, double minY, double maxY);

    protected:


    private:
      /// List of titles indexed by number
      vector< string > titles;

      /// List of pointers to point lists indexed by number
      vector< vector< pair<double,double> > > pointlists;

      /// List of stroke styles indexed by number
      vector< StrokeStyle > styles;

      /// List of markers indexed by number
      vector< Marker > markers;

      struct map_object
      {
        double multX,minX,multY,minY;
        map_object(double multX,double minX,double multY,double minY)
        {
          this->multX = multX;
          this->minX = minX;
          this->multY = multY;
          this->minY = minY;
        }
        void operator() (Point& p)
        {
          p.x = multX*(p.x-minX);
          p.y = multY*(p.y-minY);
        }
      };
  };

}

#endif
