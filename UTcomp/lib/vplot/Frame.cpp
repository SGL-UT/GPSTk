
/// @file Frame.cpp Class to represent a region to draw in. Class definitions.

#include "Frame.hpp"

namespace vplot
{
   /// Outputs the specified text to the frame
   const Frame& operator<<(const Frame& f, const Text& text)
   {
         //TODO: add check validity
      Text tcopy(text);
      tcopy.setPosition(text.x+f.originX, text.y+f.originY);
      
      f.display->text(tcopy);
      return f;
   }


  const Frame& operator<<(const Frame& f,const Rectangle& rect)
  {
     Rectangle rcopy(rect);
     rcopy.x1 += f.originX;
     rcopy.x2 += f.originX;
     rcopy.y1 += f.originY;
     rcopy.y2 += f.originY;
     f.display->rectangle(rcopy);
     return f;
  }


   Frame::Frame(VGImage& target) 
         : display(&target),
           originX(0), originY(0),
           width(target.getWidth()), height(target.getHeight()),
           valid(true)
   {}
   
   void Frame::nest(const Frame& parentFrame, double xoffset, double yoffset)
   {
      valid=true;
      isTopLevel=false;
      // Compute global coordinates of the nested frame
      originX = parentFrame.originX + xoffset;
      originY = parentFrame.originY + yoffset;
      display = parentFrame.display;
   }   
}
