
/// @file Layout.hpp Base class to lay out graphic elements (declarations).

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


#ifndef VPLOT_LAYOUT_H
#define VPLOT_LAYOUT_H

#include "GraphicsConstants.hpp"
#include "Frame.hpp"

namespace vplot
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   class Layout
   {
      
   public:

      /**
       * Constructor. 
       * @param frame Frame to create layout within
       */   
      Layout(const Frame& frame) : target(frame)
         {}

      virtual int getFrameCount(void)
	{return 1;}

      virtual Frame getFrame(int fnum)
	{return target;}

      virtual Frame getParentFrame(void)
        {return target;}

   protected:

      Frame target;
   };

  //@}
   
} // namespace vplot

#endif
