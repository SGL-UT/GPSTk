
/// @file VLayout.cpp Class to lay out graphic elements vertially (definitions).
#include <iostream>
#include "VLayout.hpp"

namespace vplot
{
   VLayout::VLayout(const Frame& frame, int nframes) : 
     Layout(frame), targetList(nframes)
   {
      Frame parent=getParentFrame();
      double ylow=parent.ly();
      double yhigh=parent.uy();
      double height = parent.getHeight()/nframes;
      for (int i=0;i<nframes;i++)
      {
         double yloc = ylow + i*(yhigh-ylow)/nframes;
         targetList[i].setWidth(parent.getWidth());
	 targetList[i].setHeight(height);
	 targetList[i].nest(parent, 0, yloc);
      }


   }
}
