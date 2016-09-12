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

///@file Canvas.hpp Common interface to all canvases that can be draw in. 
///  Class declarations.

#ifndef VDRAW_CANVAS_H
#define VDRAW_CANVAS_H

#include <vector>
#include "VGState.hpp"
#include "Path.hpp"
#include "StrokeStyle.hpp"
#include "Line.hpp"
#include "Rectangle.hpp"
#include "Circle.hpp"
#include "Polygon.hpp"
#include "Text.hpp"
#include "Marker.hpp"
#include "Comment.hpp"
#include "Bitmap.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * This is an abstract class for canvases in which you can draw.
       * This will be extended by VGImage and Frame.
       */
   class Canvas 
   {
         /// Changes the appearance of lines in the stream  
      friend Canvas& operator<<(Canvas& canvas, const StrokeStyle& style)
      {
         canvas.setLineStyle(style);
         return canvas;
      }

         /// Changes the appearance of line points in the stream
      friend Canvas& operator<<(Canvas& canvas, const Marker& marker)
      {
         canvas.setMarker(marker);
         return canvas;
      }

         /// Changes the appearance of text in the stream  
      friend Canvas& operator<<(Canvas& canvas, const TextStyle& style)
      {
         canvas.setTextStyle(style);
         return canvas;
      }

         /// Changes the fill color of objects in the stream  
      friend Canvas& operator<<(Canvas& canvas, const Color& color)
      {
         canvas.setFillColor(color);
         return canvas;
      }

         /// Outputs the specified line to the stream
      friend Canvas& operator<<(Canvas& canvas, const Line& line)
      {
         canvas.line(line);
         return canvas;
      }

         /// Outputs a comment to the stream
      friend Canvas& operator<<(Canvas& canvas, const char *comment)
      {
         canvas.comment(Comment(comment));
         return canvas;
      }

         /// Outputs a comment to the stream
      friend Canvas& operator<<(Canvas& canvas, const Comment& comment)
      {
         canvas.comment(comment);
         return canvas;
      }

         /// Outputs the specified rectangle to the stream
      friend Canvas& operator<<(Canvas& canvas, const Rectangle& rect)
      {
         canvas.rectangle(rect);
         return canvas;
      }

         /// Outputs the specified circle to the stream
      friend Canvas& operator<<(Canvas& canvas, const Circle& circle)
      {
         canvas.circle(circle);
         return canvas;
      }

         /// Outputs the specified text to the stream
      friend Canvas& operator<<(Canvas& canvas, const Text& text)
      {
         canvas.text(text);
         return canvas;
      }

         /// Outputs the specified polygon to the stream
      friend Canvas& operator<<(Canvas& canvas, const Polygon& polygon)
      {
         canvas.polygon(polygon);
         return canvas;
      }

         /// Outputs the specified polygon to the stream
      friend Canvas& operator<<(Canvas& canvas, const Bitmap& bitmap)
      {
         canvas.bitmap(bitmap);
         return canvas;
      }


   public:
         /**
          * Set up the VGState
          */
      Canvas();

      ~Canvas();

         /**
          * Set the default line style. Doesn't necessarily
          * generate output on the stream until a new line is drawn.
          * @param newStyle New appearance of the line
          */
      void setLineStyle( const StrokeStyle& newStyle ) 
      {defaults->setSS(newStyle);}

         /**
          * Removes default line style so lines are drawn in the default
          * style for the stream.
          */
      void removeLineStyle(void) 
      {defaults->setSS();}

         /**
          * Sets the default marker style. Doesn't necessarily
          * generate output on the stream until a new line is drawn.
          * @param newMarker New default marker appearance
          */
      virtual void setMarker( const Marker& newMarker ) 
      {defaults->setM(newMarker);}

         /**
          * Removes default marker style so lines are drawn in the default
          * style for the stream.
          */
      virtual void removeMarker(void) 
      {defaults->setM();}

         /**
          * Sets the default text style. Doesn't necessarily
          * generate output on the stream until new text is drawn.
          * @param newStyle New appearance of the text
          */
      void setTextStyle( const TextStyle& newStyle ) 
      {defaults->setTS(newStyle);}

         /**
          * Removes default text style so text is drawn in the default
          * style for the stream.
          */
      void removeTextStyle(void) 
      {defaults->setTS();}

         /**
          * Sets the default fill color. Doesn't necessarily
          * generate output on the stream until something filled is drawn.
          * @param newColor New fill color
          */
      void setFillColor( const Color& newColor ) 
      {defaults->setFC(newColor);}

         /**
          * Removes default fill color so shapes are drawn empty.
          */
      void removeFillColor(void) 
      {defaults->setFC();}

         /**
          * Draws a simple line segment connecting two points.
          * @param x1 x-coordinate of the starting point
          * @param y1 y-coordinate of the starting point
          * @param x2 x-coordinate of the end point
          * @param y2 y-coordinate of the end point
          */
      virtual void line (double x1, double y1, double x2, double y2)
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
          * @param polygon Polygon containing points that define the
          *   edges of the polygon
          */  
      virtual void polygon(const Polygon& polygon)=0;

         /**
          * Adds a bitmap.
          * @param bitmap Rasterized image to be added to the file
          */
      virtual void bitmap(const Bitmap& bitmap)=0;

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
          * Pushes the current state information on the stack.  This
          * is so helper methods and classes can push the state and
          * pop the state in order to not interfere or accidentally
          * use the defaults of the calling function.
          */
      void push_state()
      {
         stateStack.push_back(defaults);
         defaults = new VGState();
      }

         /**
          * Pops the topmost state off of the state stack and returns
          * it to the current default settings. If the state stack is
          * empty, default is reset.
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
          * newy = y + c.up()*dy
          * @return a multiplier that is 1 if up is positive and -1 if not
          */
      inline double up() { return (isLL()?1:-1); }

         /**
          * @return true if the origin is lower left, false otherwise
          */
      virtual bool isLL()=0;

         /// The state stack
      std::vector<VGState*> stateStack;

         /// The current default state settings
      VGState *defaults;

   private:

   }; // class Canvas

      //@}

} // namespace vdraw

#endif //VDRAW_CANVAS_H
