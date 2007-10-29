
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

   VLayout::VLayout(const Frame& frame, double fraction)
         : Layout(frame), targetList(2)
   {
      Frame parent=getParentFrame();

      double ylow=parent.ly();
      double yhigh=parent.uy();

      targetList[0].setHeight(parent.getHeight()*fraction);      
      targetList[0].setWidth(parent.getWidth());
      targetList[0].nest(parent, 0, ylow);
      targetList[1].setHeight(parent.getHeight()*(1.-fraction));      
      targetList[1].setWidth(parent.getWidth());
      targetList[1].nest(parent, 0, ylow+parent.getHeight()*fraction);

   }
   
}
