
/// @file Rectangle.hpp Defines a simple rectangle. Class declarations.

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


#ifndef VPLOT_RECT_H
#define VPLOT_RECT_H

#include<string>
#include<list>

#include "StrokeStyle.hpp"
#include "Fillable.hpp"

namespace vplot
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This class defines a simple rectangle. The rectangle can
    * have a user defined appearance, specified using a StrokeStyle. 
    */
   class Rectangle : public Fillable {

   public:

      /// X coordinate of first corner
      double x1; 

      /// Y coordinate of first corner
      double y1;

      /// X coordinate of opposite corner
      double x2;

      /// Y coordinate of opposite corner
      double y2;

     /**
      * Constructor. Defines the rectangle by two opposite points.
      * @param ix1 x coordinate of first corner
      * @param iy1 y coordinate of first corner
      * @param ix2 x coordinate of opposite corner
      * @param iy2 y coordinate of opposite corner
      */
      Rectangle(double ix1, double iy1, double ix2, double iy2)
                : Fillable() 
                { x1=ix1; y1=iy1; x2=ix2; y2=iy2; fixPoints(); };

     /**
      * Constructor. Defines the rectangle by teo opposite points and its 
      * appearance.
      * @param ix1 x coordinate of first corner
      * @param iy1 y coordinate of first corner
      * @param ix2 x coordinate of opposite corner
      * @param iy2 y coordinate of opposite corner
      * @param istyle appearance of the stroke
      */
      Rectangle(double ix1, double iy1, double ix2, double iy2,
                const StrokeStyle& istyle)
                : Fillable(istyle)
                { x1=ix1; y1=iy1; x2=ix2; y2=iy2; fixPoints(); };

     /**
      * Constructor. Defines the rectangle by teo opposite points and its 
      * appearance.
      * @param ix1 x coordinate of first corner
      * @param iy1 y coordinate of first corner
      * @param ix2 x coordinate of opposite corner
      * @param iy2 y coordinate of opposite corner
      * @param istyle appearance of the stroke
      * @param ifillcolor fill color of the rectangle
      */
      Rectangle(double ix1, double iy1, double ix2, double iy2,
                const StrokeStyle& istyle, const Color& ifillcolor)
                : Fillable(istyle,ifillcolor)
                { x1=ix1; y1=iy1; x2=ix2; y2=iy2; fixPoints(); };

     /**
      * Constructor. Defines the rectangle by teo opposite points and its 
      * appearance.
      * @param ix1 x coordinate of first corner
      * @param iy1 y coordinate of first corner
      * @param ix2 x coordinate of opposite corner
      * @param iy2 y coordinate of opposite corner
      * @param ifillcolor fill color of the rectangle
      */
      Rectangle(double ix1, double iy1, double ix2, double iy2,
                const Color& ifillcolor)
                : Fillable(ifillcolor)
                { x1=ix1; y1=iy1; x2=ix2; y2=iy2; fixPoints(); };

   protected:

   private:
      
      /// Private function call to make the first coordinate (x1,y1) the bottom left corner
      /// and make the opposite corner(x2,y2) reflect the change.
      void fixPoints();

   }; // class Rectangle

   //@}

} // namespace vplot

#endif //VPLOT_RECT_H


