#pragma ident "$Id: ReferenceFrame.hpp 2009-06-19 10:51:15 tvarney $"

#include "ReferenceFrame.hpp"

namespace gpstk
{
	
	std::map<ReferenceFrame::FramesEnum, std::string> ReferenceFrame::names;
   
      //Print's the name of the reference frame to the specified
      //ostream.
   std::ostream& operator<<(std::ostream& os,
                              ReferenceFrame& rf)
   {
      return os << rf.asString();
   }
   
   ReferenceFrame::ReferenceFrame(FramesEnum reference)
      throw()
   {
   	initialize();
      setReferenceFrame(reference);
   }
   
   ReferenceFrame::ReferenceFrame(int index)
      throw()
   {
   	initialize();
         //We use names.size() as our upper bound so we can
         //dynamically load new reference frames into this class
      if(index < Unknown || index > names.size())
         frame = Unknown;
      else
         frame = static_cast<FramesEnum>(index);
   }
   
   ReferenceFrame::ReferenceFrame(const std::string str)
   	throw()
   {
   	fromString(str);
	}
   
   ReferenceFrame::ReferenceFrame(const char str[])
   	throw()
   {
   	fromString(str);
	}
   
   void ReferenceFrame::setReferenceFrame(const FramesEnum reference)
      throw()
   {
      if(reference < Unknown || reference > names.size())
         frame = Unknown;
      else
      	frame = reference;
   }
   ReferenceFrame::FramesEnum ReferenceFrame::getFrame() const
      throw()
   {
      return frame;
   }
   
   ReferenceFrame& ReferenceFrame::createReferenceFrame(std::string& name)
      throw()
   {
   	int index;
      for(index = 0; index < names.size(); ++index)
      {
      	if(names[(FramesEnum)index] == name)
      	{
      		frame = (FramesEnum)index;
      		return (*this);
			}
		}
		   //The specified frame dne, create it
		index = names.size();
		names[(FramesEnum)index] = name;
		frame = (FramesEnum)index;
		return (*this);
   }
   
   std::string& ReferenceFrame::asString() const
      throw()
   {
      return names[frame];
   }
   void ReferenceFrame::fromString(const std::string& name)
      throw()
   {
      for(int i = 0; i < names.size(); ++i)
      {
         if(names[(FramesEnum)i] == name)
         {
            frame = (FramesEnum)i;
            return;
         }
      }
      frame = Unknown;
   }
   
   void ReferenceFrame::fromString(const char name[])
      throw()
   {
      for(int i = 0; i < names.size(); ++i)
      {
         if(names[(FramesEnum)i] == name)
         {
            frame = (FramesEnum)i;
            return;
         }
      }
      frame = Unknown;
   }
   
   bool ReferenceFrame::operator==(const ReferenceFrame right) const
      throw()
   {
      return (frame == right.frame);
   }
   bool ReferenceFrame::operator!=(const ReferenceFrame right) const
      throw()
   {
      return (frame != right.frame);
   }
   bool ReferenceFrame::operator>(const ReferenceFrame right) const
      throw()
   {
      return (frame > right.frame);
   }
   bool ReferenceFrame::operator<(const ReferenceFrame right) const
      throw()
   {
      return (frame < right.frame);
   }
   bool ReferenceFrame::operator>=(const ReferenceFrame right) const
      throw()
   {
      return (frame >= right.frame);
   }
   bool ReferenceFrame::operator<=(const ReferenceFrame right) const
      throw()
   {
      return (frame <= right.frame);
   }
   
   void ReferenceFrame::initialize()
   	throw()
   {
   	   //Use this to make sure it is only initialized once
   	static bool initialized = false;
   	if(initialized)
   	   return;
   	
   	names[Unknown] = "Unknown";
   	names[WGS84] = "WGS84";
   	names[PZ90] = "PZ90";
	}
}
