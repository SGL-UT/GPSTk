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

/// @file Line.hpp Defines a simple line. Class declarations.

#ifndef VDRAW_LINE_H
#define VDRAW_LINE_H

#include<string>
#include<list>

#include "StrokeStyle.hpp"
#include "Path.hpp"
#include "Markable.hpp"
#include "Marker.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * This class defines a simple line between a set of points. The
       * line can have a user defined appearance, specified using a
       * StrokeStyle.  It can also contain its own Marker which will
       * show how each point is displayed.
       */
   class Line: public Path, public Markable
   {

   public:
 
         /**
          * Constructor. Defines the start and end points of the line.
          * @param ix1 x coordinate of start point
          * @param iy1 y coordinate of start point
          * @param ix2 x coordinate of end point
          * @param iy2 y coordinate of end point
          */
      Line(double ix1, double iy1, double ix2, double iy2);

         /**
          * Constructor. Defines the start and end points of the line and its 
          * appearance.
          * @param ix1 x coordinate of start point
          * @param iy1 y coordinate of start point
          * @param ix2 x coordinate of end point
          * @param iy2 y coordinate of end point
          * @param istyle appearance of the stroke
          */
      Line(double ix1, double iy1, double ix2, double iy2,
           const StrokeStyle& istyle);

         /**
          * Constructor. Defines a line of several segments.
          * @param iPath the set of points to connect
          */
      Line(const Path& iPath)
            : Path(iPath), Markable()
      {}

         /**
          * Constructor. Defines a line of several segments.
          * @param iPath the set of points to connect
          * @param istyle appearance of the stroke
          */
      Line(const Path& iPath, const StrokeStyle& istyle)
            : Path(iPath), Markable(istyle)
      {}

         /**
          * Constructor. Defines a line of several segments.
          * \note If this constructor is used and there is no default
          * linestyle set for the stream in which this line is drawn,
          * then there will only be marks at the points and there will
          * be no lines connecting them.  This also applies if the
          * selected stroke style has a clear color.
          * @param iPath the set of points to connect
          * @param imarker the marker for the points of the line
          */
      Line(const Path& iPath, const Marker& imarker)
            : Path(iPath), Markable(imarker)
      {}

         /**
          * Constructor. Defines a line of several segments.
          * @param iPath the set of points to connect
          * @param istyle appearance of the stroke
          * @param imarker the marker for the points of the line
          */
      Line(const Path& iPath, const StrokeStyle& istyle, const Marker& imarker)
            : Path(iPath), Markable(istyle,imarker)
      {}

   protected:

   private:
      
   }; // class Line

      //@}

} // namespace vdraw

#endif //VDRAW_LINE_H
