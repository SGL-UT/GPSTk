
/// @file GridLayout.cpp Class to lay out graphic elements vertially (definitions).
#include <iostream>
#include "GridLayout.hpp"

namespace vplot
{
   GridLayout::GridLayout(const Frame& frame, int rows, int cols) : 
         Layout(frame), targetList(rows*cols), nrows(rows), ncols(cols)
   {
      Frame parent=getParentFrame();

      double ylow=parent.ly();
      double yhigh=parent.uy();
      double height = parent.getHeight()/nrows;

      double xlow=parent.lx();
      double xhigh=parent.ux();
      double width = parent.getWidth()/ncols;

      for (int r=0;r<nrows;r++)
         for (int c=0; c<ncols; c++)
         {
            double yloc = ylow + r*(yhigh-ylow)/nrows;
            double xloc = xlow + c*(xhigh-xlow)/ncols;

               // The list is linear; It's row major
            int i=c+r*ncols; 

            targetList[i].setWidth(width);
            targetList[i].setHeight(height);

	    targetList[i].nest(parent, xloc, yloc);
         }

   }
}
