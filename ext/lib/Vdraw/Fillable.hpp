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

///@file Fillable.hpp Interface for all fillable basic shapes.  

#ifndef VDRAW_FILLABLE_H
#define VDRAW_FILLABLE_H

#include "BasicShape.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * This is an interface for fillable basic shapes to group them
       * together.  These include circles, rectangles, and polygons.
       * All of these objects can contain a fill color.
       */

   class Fillable : public BasicShape
   {
 
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
          * Constructor.  Defines a basic shape by a StrokeStyle and a
          * fill color.
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

      //@}

} // namespace vdraw

#endif //VDRAW_FILLABLE_H
