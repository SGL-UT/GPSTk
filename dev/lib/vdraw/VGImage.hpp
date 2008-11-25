#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/VGImage.hpp#2 $"

///@file VGImage.hpp Common interface to all vector plot formats. Class declarations.

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


#ifndef VDRAW_VGIMAGE_H
#define VDRAW_VGIMAGE_H

#include <string>
#include <stdarg.h>

#include "Canvas.hpp"
#include "VDrawException.hpp"

/**
 * Drawing basic shapes and text to vector graphics format.
 */
namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * This is a generic, format- and device-independent interface to 
   * vector plotting. Any subclass can be "swapped out" with another.
   * The default unit of length is  points (1/72 of an inch, about 
   * 1/28 of a centimeter). 
   */

  class VGImage : public Canvas
  {
    public:

    /// Units conversion from points to inches
    static const double PTS_PER_INCH;

    /// Units conversion from points to centimeters
    static const double PTS_PER_CM;

    /// U.S. Customary page size width given in points
    static const double US_LETTER_WIDTH_PTS;
    /// U.S. Customary page size height given in points
    static const double US_LETTER_HEIGHT_PTS;

    // For the rest of the world... there's ISO 216-series sizes
    /// A3 page size width given in points
    static const double A3_WIDTH_PTS;
    /// A3 page size height given in points
    static const double A3_HEIGHT_PTS;

    // A4 and US customary letter are of comparable size
    /// A4 page size width given in points
    static const double A4_WIDTH_PTS;
    /// A4 page size height given in points 
    static const double A4_HEIGHT_PTS;

    /// A5 page size width given in points
    static const double A5_WIDTH_PTS;
    /// A5 page size height given in points
    static const double A5_HEIGHT_PTS;

    /// Enumeration to set origin from which everything in the stream will be drawn.
    enum ORIGIN_LOCATION { 
      /// Origin in the lower-left corner.
      LOWER_LEFT, 
      /// Origin in the upper-left corner.
      UPPER_LEFT 
    };

    /**
     * Constructors and Destructor
     */

    /**
     * Defines a new canvas in which to draw.
     * @param width Width of the canvas in points
     * @param height Height of the canvas in points
     * @param iloc Location on the page of the overall origin
     */ 
    VGImage(double width=US_LETTER_WIDTH_PTS, 
        double height=US_LETTER_HEIGHT_PTS,
        ORIGIN_LOCATION iloc=LOWER_LEFT): 
      Canvas(),
      canvasWidth(width), 
      canvasHeight(height),
      footerHasBeenWritten(false), 
      ll(iloc==LOWER_LEFT)
    { 
      //std::cout << height << " " << canvasHeight << "\n";
    }

    /// Destructor.
    virtual ~VGImage() {}   

    /// Access width
    inline double getWidth(void) const 
    {
      return canvasWidth;
    }

    /// Access height
    inline double getHeight(void) const
    {
      return canvasHeight;
    }

    bool isLL() { return ll; }

    /**
     * Write the footer, flush the stream, and view the output.
     */
    virtual void view(void) throw (VDrawException)
    { throw VDrawException("No appropriate viewer has been defined.");}

    protected:

    /**
     * Take a bitmap and draw rectangles for each box. 
     */
    void rectangles(const Bitmap &bitmap);

    /**
     * This is an enumeration for the getCorrect*(...) helper methods
     */
    enum StyleType {
      /// There is no potential style.
      NONE,
      /// The selected style is clear
      CLEAR,
      /// The shape has no style but a default one is set
      DEFAULT, 
      /// The shape has its own style
      SHAPE
    };

    /**
     * Helper function.
     * \see StyleType For return values
     * @param[out] m Pointer to Marker to be set.
     * @param[in] mark Line to obtain Marker for.
     * @return What type of marker was selected
     */
    StyleType getCorrectMarker(Marker *m, const Markable& mark);

    /**
     * Helper function.
     * \see StyleType For return values
     * @param[out] s Pointer to StrokeStyle to be set.
     * @param[in] shape BasicShape to obtain the StrokeStyle for.
     * @return What type of StrokeStyle was selected
     */
    StyleType getCorrectStrokeStyle(StrokeStyle *s, const BasicShape& shape);

    /**
     * Helper function.
     * \see StyleType For return values
     * @param[out] c Pointer to Color to be set.
     * @param[in] shape Fillable to obtain the fill Color for.
     * @return What type of Color was selected
     */
    StyleType getCorrectFillColor(Color *c, const Fillable& shape);

    /**
     * Helper function.
     * \see StyleType For return values
     * @param[out] s Pointer to TextStyle to be set.
     * @param[in] text Text to obtain the TextStyle for.
     * @return What type of TextStyle was selected
     */
    StyleType getCorrectTextStyle(TextStyle *s, const Text& text);

    /**
     *  Close up the file -- must be done before this object
     *  is destroyed.
     */
    virtual void outputFooter(void) {}

    /// Width of the drawable area in points
    double canvasWidth;  

    /// Height of the drawable area in points 
    double canvasHeight;

    /// Make sure to only write the footer once.
    bool footerHasBeenWritten;

    /**
     * Is the origin lower left? If not it is upper left.
     * This boolean is used to shorten expressions when placing 
     * a y-coordinate in a file.
     */
    bool ll; 

    private:


  }; // class VGImage

  //@} group BasicVectorGraphics

} // namespace vdraw

#endif //VDRAW_VGIMAGE_H

