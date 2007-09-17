
/// @file HLayout.cpp Class to lay out graphic elements horizontally (definitions).
#include <iostream>
#include "HLayout.hpp"

namespace vplot
{
   HLayout::HLayout(const Frame& frame, int nframes) : 
     Layout(frame), targetList(nframes)
   {
      Frame parent=getParentFrame();
      double xlow=parent.lx();
      double xhigh=parent.ux();
      double width = parent.getWidth()/nframes;
      for (int i=0;i<nframes;i++)
      {
         double xloc = xlow + i*(xhigh-xlow)/nframes;
         targetList[i].setHeight(parent.getHeight());
	 targetList[i].setWidth(width);
	 targetList[i].nest(parent, xloc, 0);
      }
   }
}
