#pragma ident "$Id: ReferenceFrame.hpp 2009-06-19 10:51:15 tvarney $"

#include "ReferenceFrame.hpp"

namespace gpstk
{
      //Print's the name of the reference frame to the specified
      //ostream.
   std::ostream& operator<<(std::ostream& os,
                              ReferenceFrame& rf)
   {
      return os << rf.asString();
   }
   
   ReferenceFrame::ReferenceFrame(FramesEnum reference = Unknown)
      throw()
   {
      setReferenceFrame(reference);
   }
   
   ReferenceFrame::ReferenceFrame(int index)
      throw()
   {
         //We use names.size() as our upper bound so we can
         //dynamically load new reference frames into this class
      if(index < Unknown || index > names.size())
         frame = Unknown;
      else
         frame = static_cast<FramesEnum>(index);
   }
   
   void ReferenceFrame::setReferenceFrame(const FramesEnum reference)
      throw()
   {
      if(reference < Unknown || reference > names.size())
         frame = reference;
   }
   FramesEnum ReferenceFrame::getFrame() const
      throw()
   {
      return frame;
   }
   
   ReferenceFrame& createReferenceFrame(std::string& name)
      throw()
   {
      if(!names.count(name))
      {
         //Ta da! magic!
         names[(FramesEnum)names.size()] = name;
         
      }
      else
      {
         ReferenceFrame ret;
            //Since it already exists, just use it
         ret.fromString(name);
         return ret;
      }
   }
   
   std::string& ReferenceFrame::asString() const
      throw()
   {
      return names[frame];
   }
   void ReferenceFrame::fromString(std::string& name) const
      throw()
   {
      for(int i = 0; i < names.size(); ++i)
      {
         if(names[(FramesEnum)i] == name)
         {
            frame = static_cast<FramesEnum>(i);
            return;
         }
      }
      frame = Unknown;
   }
   
   bool ReferenceFrame::operator==(const ReferenceFrame right) const
      throw()
   {
      return frame == right.frame;
   }
   bool ReferenceFrame::operator!=(const ReferenceFrame right) const
      throw()
   {
      return frame != right.frame;
   }
   bool ReferenceFrame::operator>(const ReferenceFrame right) const
      throw()
   {
      return frame > right.frame;
   }
   bool ReferenceFrame::operator<(const ReferenceFrame right) const
      throw()
   {
      return frame < right.frame;
   }
   bool ReferenceFrame::operator>=(const ReferenceFrame right) const
      throw()
   {
      return frame >= right.frame;
   }
   bool ReferenceFrame::operator<=(const ReferenceFrame right) const
      throw()
   {
      return frame <= right.frame;
   }
}
