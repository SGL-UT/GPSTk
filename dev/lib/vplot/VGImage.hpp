
///@file VectorGraphicsStreamBase.hpp Common interface to all vector plot formats. Class declarations.

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


#ifndef VPLOT_VGIMAGE_H
#define VPLOT_VGIMAGE_H

#include <string>
#include <stdarg.h>
#include <vector>

#include "VPlotException.hpp"
#include "Path.hpp"
#include "StrokeStyle.hpp"
#include "Line.hpp"
#include "Rectangle.hpp"
#include "Circle.hpp"
#include "Polygon.hpp"
#include "Text.hpp"
#include "Marker.hpp"
#include "Comment.hpp"

namespace vplot
{
  /** \defgroup BasicVectorGraphics Basic Vector Graphics*/ 
  //@{

  /**
   * This is a generic, format- and device-independent interface to 
   * vector plotting. Any subclass can be "swapped out" with another.
   * The default unit of length is  points (1/72 of an inch, about 
   * 1/28 of a centimeter). 
   */

  class VGImage {

    // Comments for these are contained with the code.

    friend VGImage& 
      operator<<(VGImage& vgstrm, const Line& line);

    friend VGImage& 
      operator<<(VGImage& vgstrm, const StrokeStyle& style);

    friend VGImage& 
      operator<<(VGImage& vgstrm, 
          const Marker& marker);

    friend VGImage& 
      operator<<(VGImage& vgstrm, 
          const Comment& comment);

    friend VGImage& 
      operator<<(VGImage& vgstrm, const Rectangle& rect); 

    friend VGImage& 
      operator<<(VGImage& vgstrm, const Circle& circle); 

    friend VGImage& 
      operator<<(VGImage& vgstrm, const Text& text); 

    friend VGImage& 
      operator<<(VGImage& vgstrm, const Polygon& polygon); 
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
      canvasWidth(width), canvasHeight(height),
      footerHasBeenWritten(false), ll(iloc==LOWER_LEFT)
    { 
      defaults = new VGState();
      //std::cout << height << " " << canvasHeight << "\n";
    }

    /// Destructor.
    virtual ~VGImage() {}   

        /// Access width
     double getWidth(void) const 
     {
        return canvasWidth;
     }
 
        /// Access height
     double getHeight(void) const
     {
        return canvasHeight;
     }
     
     
    /**
     * Set the default line style. Doesn't necessarily
     * generate output on the stream until a new line is drawn.
     * @param newStyle New appearance of the line
     */
    void setLineStyle( const StrokeStyle& newStyle ) 
    {defaults->useSS = true;defaults->strokeStyle = newStyle;}

    /**
     * Removes default line style so lines are drawn in the default
     * style for the stream.
     */
    void removeLineStyle(void) 
    {defaults->useSS = false;defaults->strokeStyle = StrokeStyle();}

    /**
     * Sets the default marker style. Doesn't necessarily
     * generate output on the stream until a new line is drawn.
     * @param newMarker New default marker appearance
     */
    virtual void setMarker( const Marker& newMarker ) 
    {defaults->useM = true;defaults->marker = newMarker;}

    /**
     * Removes default marker style so lines are drawn in the default
     * style for the stream.
     */
    virtual void removeMarker(void) 
    {defaults->useM = false;defaults->marker = Marker();}

    /**
     * Sets the default text style. Doesn't necessarily
     * generate output on the stream until new text is drawn.
     * @param newStyle New appearance of the text
     */
    void setTextStyle( const TextStyle& newStyle ) 
    {defaults->useTS = true;defaults->textStyle = newStyle;}

    /**
     * Removes default text style so text is drawn in the default
     * style for the stream.
     */
    void removeTextStyle(void) 
    {defaults->useTS = false;defaults->textStyle = TextStyle();}

    /**
     * Sets the default fill color. Doesn't necessarily
     * generate output on the stream until something filled is drawn.
     * @param newColor New fill color
     */
    void setFillColor( const Color& newColor ) 
    {defaults->useFC = true;defaults->fillColor = newColor;}

    /**
     * Removes default fill color so shapes are drawn empty.
     */
    void removeFillColor(void) 
    {defaults->useFC = false;defaults->fillColor = Color();}

    /**
     * Draws a simple line segment connecting two points.
     * @param x1 x-coordinate of the starting point
     * @param y1 y-coordinate of the starting point
     * @param x2 x-coordinate of the end point
     * @param y2 y-coordinate of the end point
     */
    void line (double x1, double y1, double x2, double y2)
    {
      line(Line(x1,y1,x2,y2));
    };

    /**
     * Draws a series of line segments, defined by the path variable
     * @param line Line containing list of points that define the line
     */  
    virtual void line (const Line& line)=0;

    /**
     * Draws a simple rectangle using two points
     * @param rect Rectangle to draw
     */
    virtual void rectangle (const Rectangle& rect)=0;

    /**
     * Draws a simple circle using a point and a radius
     * @param circle Circle to draw
     */
    virtual void circle (const Circle& circle)=0;

    /**
     * Draws text.
     * @param text Text to draw
     */
    virtual void text (const Text& text)=0;

    /**
     * Draws a polygon, defined by the path variable.
     * @param polygon Polygon containing points that define the edges of the polygon
     */  
    virtual void polygon(const Polygon& polygon)=0;

    /**
     * Adds a comment.
     * @param comment Comment to be added to the file
     */
    virtual void comment(const Comment& comment)=0;

    /**
     * Adds a comment
     * @param format Format string (printf style)
     * @param ... Other arguments for printf.
     */
    virtual void comment(const char * format, ...)
    {
      va_list ap;
      va_start(ap,format);
      comment(Comment(format,ap));
      va_end(ap);
    }

    /**
     * Draws an arc of user defined radius, center, over a stated 
     * angular span.
     * @param cx X location of the center of the arc
     * @param cy Y value of the center of the arc
     * @param r Radius of the arc
     * @param startAngle Angle in degrees of the start of the arc
     * @param endAngle End angle of the arc.
     */
    //virtual void arc (double cx, double cy, double r, 
    //                  double startAngle, double endAngle);

    /**
     * Pushes the current state information on the stack.  This is so helper
     * methods and classes can push the state and pop the state in order to not
     * interfere or accidentally use the defaults of the calling function.
     */
    void push_state()
    {
      stateStack.push_back(defaults);
      defaults = new VGState();
    }

    /**
     * Pops the topmost state off of the state stack and returns it to the
     * current default settings. If the state stack is empty, default is reset.
     */
    void pop_state()
    {
      if(stateStack.size() > 0)
      {
        delete defaults;
        defaults = stateStack.back();
        stateStack.pop_back();
      }
      else
        defaults = new VGState();
    }

    /**
     * Write the footer, flush the stream, and view the output.
     */
    virtual void view(void) throw (VPlotException)
    { throw VPlotException("No appropriate viewer has been defined.");}

    protected:

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

    /// Make sure to only write the footer once.
    bool footerHasBeenWritten;

    /// Width of the drawable area in points
    double canvasWidth;  

    /// Height of the drawable area in points 
    double canvasHeight;

    /**
     * Is the origin lower left? If not it is upper left.
     * This boolean is used to shorten expressions when placing 
     * a y-coordinate in a file.
     */
    bool ll; 

    /// This structor holds the current default state.
    struct VGState 
    {
      /// The default StrokeStyle (if one is set)
      StrokeStyle strokeStyle;
      /// The default Marker (if one is set)
      Marker marker;
      /// The default TextStyle (if one is set)
      TextStyle textStyle;
      /// The default fill Color (if one is set)
      Color fillColor;

      /// Whether or not to use the default StrokeStyle
      bool useSS;
      /// Whether or not to use the default Marker
      bool useM;
      /// Whether or not to use the default TextStyle
      bool useTS;
      /// Whether or not to use the default fill Color      
      bool useFC;

      /// Make sure the defaults are with nothing set.
      VGState()
      {
        useSS = useM = useTS = useFC = false;
      }
    };

    /// The state stack
    std::vector<VGState*> stateStack;

    /// The current default state settings
    VGState *defaults;

    private:


  }; // class VGImage

  //@} group BasicVectorGraphics

} // namespace vplot

#endif //VPLOT_VECTORGRAPHICSSTREAMBASE_H

