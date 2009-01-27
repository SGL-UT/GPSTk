#pragma ident "$Id$"

///@file Markable.hpp Interface for all markable basic shapes.  

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


#ifndef VDRAW_MARKABLE_H
#define VDRAW_MARKABLE_H

#include "BasicShape.hpp"
#include "Marker.hpp"

namespace vdraw
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This is an interface for markable basic shapes.  Anything that can contain
    * a marker.  The only use of this will be by a line which is not fillable as
    * that is the only shape where markers make sense.  So this will inherit
    * from BasicShape and will be inherited by Line.
    */

   class Markable : public BasicShape {
 
   public:

      /**
       * Constructors and Destructor
       */

      /**
       * Default constructor.
       */
      Markable(void)
         : BasicShape(), hasMarker(false)
      {}

      /**
       * Constructor.  Defines a basic shape by a StrokeStyle.
       * @param istyle appearance of the stroke
       */
      Markable(const StrokeStyle& istyle)
         : BasicShape(istyle), hasMarker(false)
      {}

      /**
       * Constructor.  Defines a basic shape by a Marker.
       * @param imarker appearance of the stroke
       */
      Markable(const Marker& imarker)
         : BasicShape(), marker(imarker), hasMarker(true)
      {}

      /**
       * Constructor.  Defines a basic shape by a StrokeStyle and a Marker.
       * @param istyle appearance of the stroke
       * @param imarker appearance of the stroke
       */
      Markable(const StrokeStyle& istyle, const Marker& imarker)
         : BasicShape(istyle), marker(imarker), hasMarker(true)
      {}

      /// Accessor. Does this markable have a marker?
      inline bool hasOwnMarker(void) const { return hasMarker; }

      /// Accessor. What is the marker for this markable?
      inline Marker getMarker(void) const { return marker; }
      
      /// Mutator. Set the marker.
      inline void setMarker(const Marker& imarker) { marker=imarker;hasMarker=true; }  

      /// Mutator. Remove the marker...use default(or none)
      inline void removeMarker(void) { hasMarker=false; }

   protected:

      /// Default marker
      Marker marker;

      /// A marker has been set?
      bool hasMarker;

   private:


   }; // class Markable

   //@} group BasicVectorGraphics

} // namespace vdraw

#endif //VDRAW_MARKABLE_H



