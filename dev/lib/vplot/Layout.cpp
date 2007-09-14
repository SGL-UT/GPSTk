
/// @file Layout.cpp Class to a layout or frame. Class definitions.

#include "Layout.hpp"

namespace vplot
{
   void Layout::setMarginSmart(void)
   {
      double length;
      (width<height) ? length=width : length=height;
      margin = 0.05*length;
   }

   void Layout::setSpacingSmart(void)
   {
      double length;
      (width<height) ? length=width : length=height;
      spacing = 0.05*length;
   }   
   
}
