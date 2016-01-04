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

/// @file SeriesList.hpp Used to maintain series information for some types of
/// plots. Class declarations.

#ifndef VPLOT_SERIESLIST_H
#define VPLOT_SERIESLIST_H

#include <string>
#include <vector>
#include <cfloat>

#include "Frame.hpp"
#include "StrokeStyle.hpp"
#include "Marker.hpp"
#include "Text.hpp"
#include "TextStyle.hpp"
#include "Color.hpp"
#include "Line.hpp"
#include "GridLayout.hpp"


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
          * @param ss vdraw::StrokeStyle for this series
          */
      void addSeries(std::string& title,
                     std::vector< std::pair<double,double> >& points,
                     vdraw::StrokeStyle& ss)
      {
         vdraw::Marker m = vdraw::Marker::clear();
         addSeries(title,points,ss,m);
      }

         /**
          * @param title Title of this series for the legend
          * @param points Points contained in this series
          * @param m Marker for this series
          */
      void addSeries(std::string& title,
                     std::vector< std::pair<double,double> >& points,
                     vdraw::Marker& m)
      {
         vdraw::StrokeStyle ss = vdraw::StrokeStyle::clear();
         addSeries(title,points,ss,m);
      }

         /**
          * @param title Title of this series for the legend
          * @param points Points contained in this series
          * @param ss vdraw::StrokeStyle for this series
          * @param m Marker for this series
          */
      void addSeries(std::string& title,
                     std::vector< std::pair<double,double> >& points,
                     vdraw::StrokeStyle& ss, vdraw::Marker& m)
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

         /// Set the vdraw::StrokeStyle of the last-added series
      bool setLastStyle(vdraw::StrokeStyle& ss)
      {
         return setStyle(styles.size()-1,ss);
      }

         /// Set the vdraw::StrokeStyle of the idx-th series to ss
      bool setStyle(unsigned int idx, vdraw::StrokeStyle& ss)
      {
         if(idx>=styles.size())
            return false; 
         styles[idx]=ss;
         return true;
      }

         /// Set the Marker of the last-added series
      bool setLastMarker(vdraw::Marker& m)
      {
         return setMarker(markers.size()-1,m);
      }

         /// Set the Marker of the idx-th series to m
      bool setMarker(unsigned int idx, vdraw::Marker& m)
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
      vdraw::StrokeStyle getStyle(int idx) { return styles[idx]; }

         /// Return the      void f(Point& p)
      vdraw::Marker getMarker(int idx)     { return markers[idx]; }

         /// Return a pointer to the list of points
      std::vector< std::pair<double,double> >& getPointList(int idx) { return pointlists[idx]; }

         /// Return the minimums and maximum of all the data.
      void findMinMax(double& minX, double &maxX, double& minY, double& maxY);

         /// Draw all of the series in innerFrame
      void drawInFrame(vdraw::Frame& innerFrame, double minX, double maxX,
                       double minY, double maxY);

         /**
          * Draw the legend in the frame with the given point size.
          * @param frame Frame to draw in
          * @param pointsize The font's point size for the legend text
          * @param columns The number of columns to split the legend to
          */
      void drawLegend(vdraw::Frame& frame, double pointsize, unsigned int columns = 1);

   protected:


   private:
         /// List of titles indexed by number
      std::vector< std::string > titles;

         /// List of pointers to point lists indexed by number
      std::vector< std::vector< std::pair<double,double> > > pointlists;

         /// List of stroke styles indexed by number
      std::vector< vdraw::StrokeStyle > styles;

         /// List of markers indexed by number
      std::vector< vdraw::Marker > markers;

         /// This struct helps in translating coordinates for a set of points
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
         void operator() (vdraw::Point& p)
         {
            p.x() = multX*(p.x()-minX);
            p.y() = multY*(p.y()-minY);
         }
      };

         /// Draw a segment of a legend from begin for n indexes
      void drawLegendSegment(vdraw::Frame& frame, double pointsize, 
                             unsigned int begin, unsigned int n);

   };

}

#endif
