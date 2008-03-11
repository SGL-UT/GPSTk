/// @file BorderLayout.cpp Class to create frame within a frame, with a border or margin. (definition)
#include <iostream>
#include "BorderLayout.hpp"

namespace vplot
{
   BorderLayout::BorderLayout(const Frame& frame, double iMarginSize) : 
     Layout(frame), targetList(1)
   {
      Frame parent=getParentFrame();
      targetList[0].setHeight(parent.getHeight()-2*iMarginSize);
      targetList[0].setWidth(parent.getWidth()-2*iMarginSize);
      targetList[0].nest(parent, iMarginSize, iMarginSize);
   }

   BorderLayout::BorderLayout(const Frame& frame, 
                              double xMarginSize, double yMarginSize ) : 
     Layout(frame), targetList(1)
   {
      Frame parent=getParentFrame();
      targetList[0].setHeight(parent.getHeight()-2*yMarginSize);
      targetList[0].setWidth(parent.getWidth()-2*xMarginSize);
      targetList[0].nest(parent, xMarginSize, yMarginSize);
   }


}
