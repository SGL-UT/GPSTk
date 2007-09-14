
/// @file Marker.hpp Defines marker appearance. Class declarations.

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


#ifndef VPLOT_MARKER_H
#define VPLOT_MARKER_H

#include<string>
#include<list>

#include "Color.hpp"

namespace vplot
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{


  /**
   * This class defines a marker which will be drawn at every point on a
   * line.
   *
   * The user-creatable BasicShape entry was removed as it presents 
   * more difficulties than it is worth in each language.  In postscript
   * a duplicate of every draw method for a basicshape class would have
   * to be made using only relative drawing and spacing and such might
   * have to be addressed.  In svg, markers seem to need to be in the 
   * first quadrant (i.e. not centered on 0 0) and their middle has to
   * be defined there as well (think of it as a translation from (0,0)
   * to somewhere where the mark fits in the first quadrant) as my 
   * tests seemed to show tha svg (at the time at least) would cut off
   * everything out of its bounds (quadrants II, III, IV).  Also there are
   * issues with the bounds of the marker and such.
   */
  class Marker {

    public:

      /// This enumeration contains sime simple marker choices.
      enum Mark{PLUS,X,DOT};

      /**
       * Default constructor. 
       */
      Marker(void) 
        : mark(DOT), range(1), usesDefault(true) {};

      /**
       * Constructor.  
       * \see getRange For a description of how range is defined.
       * @param imark The predefined mark to use.
       * @param irange The range is the radius of the symmetrical predefined mark.
       * @param icolor The desired color of the mark.
       */
      Marker(Mark imark, double irange=1.0, Color icolor=Color::BLACK) 
        : mark(imark), range(irange), usesDefault(true), markerColor(icolor) {};

      /**
       * Constructor.
       * @param ishape The desired custom shape for the marker, in relative coordinates.
       * @param maxrange This is strongly suggested for SVG as it might not turn out correctly otherwise.
       */
      //Marker(BasicShape ishape, double maxrange=0) 
      //      : range(maxrange), usesDefault(false), markerShape(ishape) {};

      /**
       * Constructor.
       * @param ishape The desired custom shape for the marker, in relative coordinates.
       * @param icolor The color to override the colors inside the shape.
       * @param maxrange This is strongly suggested for SVG as it might not turn out correctly otherwise.
       */
      //Marker(BasicShape ishape, Color icolor, double maxrange=0) 
      //      : range(maxrange), usesDefault(false), markerColor(icolor), useMarkerColor(true), markerShape(ishape) {};

      /**
       * Accessor.
       * @return The shape that the marker is set as.
       */
      //BasicShape getMarker(void) const { return markerShape; };

      /**
       * Accessor.
       * @return The mark that the marker is set as.
       */
      Mark getMark(void) const { return mark; };

      /**
       * Accessor.
       * @return The color of the marker.
       */
      Color getColor(void) const { return markerColor; };

      /**
       * Accessor.  A range defines the radius of the mark from the point on
       * which the point is drawn.  A DOT mark with a range 0.5 will place a
       * point with a diameter of 1.0 at each point on the Line.
       * @return The range of the mark from its origin.
       */
      double getRange(void) const { return range; };

      /**
       * Mutator.
       * @param newColor The new color of the marker. 
       */
      void setColor(const Color& newColor) { markerColor=newColor; }

      /// Returns whether this marker uses a default marker.
      bool hasDefaultMark(void) const { return usesDefault; };
     
      /// Returns a unique name for this marker.
      std::string uniqueName(void) const;

      /**
       * Comparison operators:
       */

      /**
       * Compares this marker to another marker to see if they each have the
       * same Mark and Range.  This is for postscript as a marker definition
       * does not contain any references to color.
       * @return Whether or not the two markers have the same shape & size.
       */
      bool shapeEquals(const Marker& other);

      /**
       * Compares this marker to another marker.
       * @return Whether or not the two markers are equal.
       */
      bool equals(const Marker& other);

      /// Operator ==
      bool operator==(const Marker& rhs) {return this->equals(rhs);};

      /// Operator !=
      bool operator!=(const Marker& rhs) {return !this->equals(rhs);};

    protected:

    private:

      /// This is the enumeration of the mark.
      Mark mark;

      /// This is the range of the drawing (width=height=range)
      double range;

      /// Contains the marker's color.
      Color markerColor;

      /// Determines if this marker uses a default marker.
      bool usesDefault;

      /// Determines if this non-default marker is to use the marker color.
      bool useMarkerColor;

      /// Contains a user-made shape to use for the marker.
      //BasicShape markerShape;

  }; // class Marker

  //@}

} // namespace vplot

#endif //VPLOT_MARKER_H

