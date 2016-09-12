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

/// @file Circle.hpp Defines a simple circle. Class declarations.

#ifndef VDRAW_CIRCLE_H
#define VDRAW_CIRCLE_H

#include<string>
#include<list>

#include "StrokeStyle.hpp"
#include "Fillable.hpp"

namespace vdraw
{
   /// @ingroup BasicVectorGraphics  
   //@{

   /**
    * This class defines a simple circle. The circle can
    * have a user defined appearance, specified using a StrokeStyle 
    * and/or a fill color.
    */
   class Circle : public Fillable {

   public:

      /// The X coordinate of the center of the circle. 
      double xc;

      /// The Y coordinate of the center of the circle.
      double yc;

      /// The radius of the circle.
      double radius;

     /**
      * Constructor. Defines the circle by the coordinate location of
      * the center and a radius.
      * @param ixc x coordinate of center
      * @param iyc y coordinate of center
      * @param ir radius of the circle
      */
      Circle(double ixc, double iyc, double ir)
         : Fillable()
         { xc = ixc; yc = iyc; radius = ir; };

     /**
      * Constructor. Defines the circle by the coordinate location of
      * the center, a radius. and its appearance.
      * @param ixc x coordinate of center
      * @param iyc y coordinate of center
      * @param ir radius of the circle
      * @param istyle appearance of the stroke
      */
      Circle(double ixc, double iyc, double ir, const StrokeStyle& istyle)
         : Fillable(istyle)
         { xc = ixc; yc = iyc; radius = ir; };


     /**
      * Constructor. Defines the circle by the coordinate location of
      * the center, a radius. and its appearance.
      * @param ixc x coordinate of center
      * @param iyc y coordinate of center
      * @param ir radius of the circle
      * @param istyle appearance of the stroke
      * @param ifillcolor fill color of the circle
      */
      Circle(double ixc, double iyc, double ir,
           const StrokeStyle& istyle, const Color& ifillcolor)
         : Fillable(istyle,ifillcolor)
         { xc = ixc; yc = iyc; radius = ir; };


     /**
      * Constructor. Defines the circle by the coordinate location of
      * the center, a radius. and its appearance.
      * @param ixc x coordinate of center
      * @param iyc y coordinate of center
      * @param ir radius of the circle
      * @param ifillcolor fill color of the circle
      */
      Circle(double ixc, double iyc, double ir,
           const Color& ifillcolor)
         : Fillable(ifillcolor)
         { xc = ixc; yc = iyc; radius = ir; };

   protected:

   private:
            
   }; // class Circle

   //@}

} // namespace vdraw

#endif //VDRAW_CIRCLE_H
