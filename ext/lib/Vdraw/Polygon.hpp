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

/// @file Polygon.hpp Defines a simple line. Class declarations.

#ifndef VDRAW_POLYGON_H
#define VDRAW_POLYGON_H

#include<string>
#include<list>

#include "StrokeStyle.hpp"
#include "Color.hpp"
#include "Path.hpp"
#include "Fillable.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * This class defines a simple line between a set of points. The line can
       * have a user defined appearance, specified using a StrokeStyle. 
       */
   class Polygon : public Path, public Fillable {

   public:
 
         /**
          * Constructor. Creates a polygon from given set of
          * points. No fill color nor line style has been specified.
          * @param iPath List of vertices
          */
      Polygon(const Path& iPath) 
            : Path(iPath), Fillable()
      {}

         /**
          * Constructor. Creates a polygon from given set of points, and 
          * specifies the appearance of the polygon edges.
          * No fill color is specified.
          * @param iPath List of vertices
          * @param istyle appearance of the edges
          */
      Polygon(const Path& iPath, 
              const StrokeStyle& istyle) 
            : Path(iPath), Fillable(istyle)
      {}

         /**
          * Constructor. Creates a polygon from given set of points,
          * and specifies the appearance of the polygon edges and the
          * fill color.
          * @param iPath List of vertices
          * @param istyle appearance of the edges
          * @param iFillColor color inside the polygon
          */
      Polygon(const Path& iPath, 
              const StrokeStyle& istyle,
              const Color& iFillColor) 
            : Path(iPath), Fillable(istyle,iFillColor)
      {}

         /**
          * Constructor. Creates a polygon from given set of points,
          * and specifies the appearance of the fill color.
          * @param iPath List of vertices
          * @param iFillColor color inside the polygon
          */
      Polygon(const Path& iPath, 
              const Color& iFillColor) 
            : Path(iPath), Fillable(iFillColor)
      {}


   protected:

   private:
      
   }; // class Polygon

      //@}

} // namespace vdraw

#endif //VDRAW_POLYGON_H
