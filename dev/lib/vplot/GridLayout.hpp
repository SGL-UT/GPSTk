
/// @file GridLayout.hpp Class to lay out graphic elements vertially (declarations).

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


#ifndef VPLOT_GRIDLAYOUT_H
#define VPLOT_GRIDLAYOUT_H

#include <vector>

#include "GraphicsConstants.hpp"
#include "Frame.hpp"
#include "Layout.hpp"

namespace vplot
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

     /**
      * Assists in arranging frames in a vertical stack.
      */
   class GridLayout : public Layout
   {
      
   public:

      /**
       * Constructor. 
       * @param frame Frame to create layout within
       * @param rows Number of rows of frames to make
       * @param columns Nums of columns of frames to make
       */   
      GridLayout(const Frame& frame, int rows, int columns);

      virtual int getFrameCount(void)
	{return targetList.size();}

      virtual Frame getFrame(int fnum)
	{return targetList[fnum];}

      virtual Frame getFrame(int row, int col)
         {return targetList[col+row*ncols];}
            
            

   protected:

      std::vector<Frame> targetList;
      int nrows, ncols;
   };

  //@}
   
} // namespace vplot

#endif
