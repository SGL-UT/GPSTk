#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Fillable.hpp#1 $"

///@file Fillable.hpp Interface for all fillable basic shapes.  

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


#ifndef VDRAW_FILLABLE_H
#define VDRAW_FILLABLE_H

#include "BasicShape.hpp"

namespace vdraw
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This is an interface for fillable basic shapes to group them together.  These 
    * include circles, rectangles, and polygons.  All of these objects can contain
    * a fill color.
    */

   class Fillable : public BasicShape {
 
   public:

      /**
       * Constructors and Destructor
       */

      /**
       * Default constructor.
       */
      Fillable(void)
         : BasicShape(), hasFillColor(false)
      {}

      /**
       * Constructor.  Defines a basic shape by a StrokeStyle.
       * @param istyle appearance of the stroke
       */
      Fillable(const StrokeStyle& istyle)
         : BasicShape(istyle), hasFillColor(false)
      {}

      /**
       * Constructor.  Defines a basic shape by a fill color.
       * @param ifillColor appearance of the stroke
       */
      Fillable(const Color& ifillColor)
         : BasicShape(), fillColor(ifillColor), hasFillColor(true)
      {}

      /**
       * Constructor.  Defines a basic shape by a StrokeStyle and a fill color.
       * @param istyle appearance of the stroke
       * @param ifillColor appearance of the stroke
       */
      Fillable(const StrokeStyle& istyle, const Color& ifillColor)
         : BasicShape(istyle), fillColor(ifillColor), hasFillColor(true)
      {}

      /// Accessor. Does this fillable have a preferred fill color?
      inline bool hasOwnFillColor(void) const { return hasFillColor; }

      /// Accessor. What is the fill color for this fillable?
      inline Color getFillColor(void) const { return fillColor; }
      
      /// Mutator. Set the fill color
      inline void setFillColor(const Color& ifillColor) { fillColor=ifillColor;hasFillColor=true; }  

      /// Mutator. Remove the fill color...use default
      inline void removeFillColor(void) { hasFillColor=false; }

   protected:

      /// Default fill color
      Color fillColor;

      /// A fill color has been set?
      bool hasFillColor;

   private:


   }; // class Fillable

   //@} group BasicVectorGraphics

} // namespace vdraw

#endif //VDRAW_FILLABLE_H



