#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/StrokeStyle.hpp#3 $"

/// @file StrokeStyle.hpp Defines line appearance: width, etc. Class declarations.

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


#ifndef VDRAW_STROKESTYLE_H
#define VDRAW_STROKESTYLE_H

#include<string>
#include<list>

#include "Color.hpp"

namespace vdraw
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{


   /**
    * This class defines how a line, path, or edge appears. The stroke can
    * have a width, a color, and a dash pattern. 
    */
   class StrokeStyle {

   public:
      
      /** 
       * This type lists the dash lengths. Each entry is interpreted 
       * as a stroke length and distance to the next stroke. If there are an
       * odd number of entries, then the list is repeated.
       */
     typedef std::list<double> dashLengthList;
   
     /**
      * Default constructor. 
      */
     StrokeStyle(void);

     /**
      * Creates a solid line StrokeStyle with defined color and width.
      * @param icolor type vdraw::Color
      * @param iwidth Width in points
      */
     StrokeStyle( const Color& icolor, 
                  double iwidth=1 ); 

     /**
      * Creates a StrokeStyle with a defined color, width, and stroke pattern.
      * @param icolor type vdraw::Color
      * @param iwidth Width in points
      * @param dll List of stroke and space lengths
      */
     StrokeStyle( const Color& icolor, 
                  double iwidth,
		  const dashLengthList& dll  ); 

     /**
      * Creates a StrokeStyle with attributes defined by a single string.
      * The idea is to mimic line specifications supported by MATLAB or 
      * Octave, but with extensions to add a greater variety
      * of colors and line widths.
      * The format of the string is "[color name][dash descriptor][width]".
      * Each descriptor is optional.
      * The color name is one that can be interpreted as a Color class.
      * Examples are 'r' for red, or the name of a color, such as
      * "burnt orange".
      * The dash symbol is any combination of dashes and dots. A single
      * "-" represents a solid line. A single "." represents a stream of dots.
      * If dots and dashes are combined, the stroke resembles that
      * appearance. The default dashing is a solid line, if unspecified.
      * The width is a number representing the width of the line in points.
      * The default width, if unspecified, is one point.
      * Some examples: "r-" would define a solid red line of default width;
      * "b.2.5" would define a dotted blue line of width 2.5 points; 
      * "olive-.0.5" would define an olive line of width one half points,
      * composed of dashes and dots.
      */ 
     StrokeStyle(const std::string& strSpec) {}

     /// Get a clear strokestyle
     static StrokeStyle clear() { return StrokeStyle(Color::CLEAR); };

     /// Operator ==
     bool operator==(const StrokeStyle& rhs) const
     { return ((color==rhs.color)&&(width==rhs.width)&&(dashList==rhs.dashList)); }

     /// Operator !=
     bool operator!=(const StrokeStyle& rhs) const
     { return !((*this)==rhs); }

     /// Color accessor
     Color getColor(void) const { return color; }

     /// Width accessor
     double getWidth(void) const { return width; }

     /// Stroke solidity accessor
     bool getSolid(void) const { return solid; }

     /// Stroke pattern accessors
     dashLengthList getDashList(void) const { return dashList; }

   protected:

      /// Line Color 
      Color color;

      /// Line width in points
      double width;   

      /// Is the line solid? If not it is dashed
      bool solid;

      /// List of dash lengths in points (if dashed)
      dashLengthList dashList;

   private:


   }; // class StrokeStyle

   //@}

} // namespace vdraw

#endif //VDRAW_STROKESTYLE_H

