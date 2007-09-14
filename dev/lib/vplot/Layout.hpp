
/// @file Text.hpp Class to represent a layout or frame. Class declarations.

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

namespace vplot
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   class Layout
   {
      
//TODO: getglobal coords, operator << of types.
   public:

      /**
       * Constructor. 
       * @param iwidth Width of region in points.
       * @param iheight Height of region in points.
       */   
      //Layout(const Layout&);
      
      Layout(double iwidth=US_LETTER_WIDTH_PTS, 
             double iheight=US_LETTER_HEIGHT_PTS) : 
             width(iwidth), height(iheight),
             topLevel(true)
         {setMarginSmart(); setSpacingSmart();};

      double getWidth(void) const {return width;} 
      double getHeight(void) const {return height;}
      double getMargin(void) const {return margin;}
      bool   isTopLevel(void) const {return topLevel;}
      
   protected:

      void setMarginSmart(void);
      void setSpacingSmart(void);
      
      double width;
      double height;

      double margin;
      double spacing;
      
      bool topLevel;
      
   };

  //@}
   
} // namespace vplot

#endif
