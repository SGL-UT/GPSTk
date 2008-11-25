#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/BasicShape.hpp#2 $"

///@file BasicShape.hpp Interface for all basic shapes.  Manages common objects/calls.

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


#ifndef VDRAW_BASICSHAPE_H
#define VDRAW_BASICSHAPE_H

#include "StrokeStyle.hpp"

namespace vdraw
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This is an interface for basic shapes to group them together.  These include
    * circles, rectangles, lines, and polygons.  All of these objects can contain
    * a StrokeStyle.
    */

   class BasicShape {
 
   public:

      /**
       * Constructors and Destructor
       */

      /**
       * Default constructor.
       */
      BasicShape(void)
         : hasLineStyle(false)
      {}

      /**
       * Constructor.  Defines a basic shape by a StrokeStyle.
       * @param istyle appearance of the stroke
       */
      BasicShape(const StrokeStyle& istyle)
         : lineStyle(istyle), hasLineStyle(true)
      {}

      /// Accessor. Does this basic shape have a preferred appearance?
      inline bool hasOwnStrokeStyle(void) const { return hasLineStyle; }

      /// Accessor. What is the style associated with this basic shape
      inline StrokeStyle getStrokeStyle(void) const { return lineStyle; }
      
      /// Mutator. Set the stroke style
      inline void setStrokeStyle(const StrokeStyle& istyle) { lineStyle=istyle;hasLineStyle=true; }  

      /// Mutator. Remove the stroke style...use default
      inline void removeStrokeStyle(void) { hasLineStyle=false; }

   protected:

      /// Default line style
      StrokeStyle lineStyle;

      /// A lineStyle has been set?
      bool hasLineStyle;

   private:


   }; // class BasicShape

   //@} group BasicVectorGraphics

} // namespace vdraw

#endif //VDRAW_BASICSHAPE_H


