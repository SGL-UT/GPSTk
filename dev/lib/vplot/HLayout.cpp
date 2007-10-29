
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

   HLayout::HLayout(const Frame& frame, double fraction)
         : Layout(frame), targetList(2)
   {
      Frame parent=getParentFrame();

      double xlow=parent.lx();
      double xhigh=parent.ux();

      targetList[0].setHeight(parent.getHeight());
      targetList[0].setWidth(parent.getWidth()*fraction);
      targetList[0].nest(parent, xlow, 0);
      targetList[1].setHeight(parent.getHeight());
      targetList[1].setWidth(parent.getWidth()*(1.-fraction));
      targetList[1].nest(parent, xlow+parent.getWidth()*fraction,0);
   }

}
