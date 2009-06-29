#pragma ident "$Id: ReferenceFrame.cpp 2009-06-25 14:11:10 tvarney $"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#include "ReferenceFrame.hpp"

namespace gpstk
{
   
   ReferenceFrame::ReferenceFrame(int index)
      throw()
   {
      initialize();
      setReferenceFrame(index);
   }
   
   ReferenceFrame::ReferenceFrame(const std::string str)
      throw()
   {
      initialize();
      setReferenceFrame(str);
   }
   
   ReferenceFrame::ReferenceFrame(const char str[])
      throw()
   {
      initialize();
      setReferenceFrame(str);
   }
   
   void ReferenceFrame::setReferenceFrame(const int index)
      throw()
   {
         //We use names.size() as our upper bound so we can
         //dynamically load new reference frames into this class
      if(index < Unknown || index >= names.size())
         frame = Unknown;
      else
         frame = (FramesEnum)index;
   }
   
   void ReferenceFrame::setReferenceFrame(const std::string& name)
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
   
   void ReferenceFrame::setReferenceFrame(const char name[])
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
   
   ReferenceFrame::FramesEnum ReferenceFrame::getFrame() const
      throw()
   {
      return frame;
   }
   
   ReferenceFrame& ReferenceFrame::createReferenceFrame(const char str[])
      throw()
   {
      std::string name(str);
      return createReferenceFrame(name);
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
         //The specified frame does not exist, create it
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
   
   bool ReferenceFrame::operator==(const ReferenceFrame& right) const
      throw()
   {
      return (frame == right.frame);
   }
   
   bool ReferenceFrame::operator!=(const ReferenceFrame& right) const
      throw()
   {
         //If frame == right.frame, only need to check one to know if both are Unknown
      return (frame != right.frame);
   }
   
   bool ReferenceFrame::operator>(const ReferenceFrame& right) const
      throw()
   {
      return (frame > right.frame);
   }
   
   bool ReferenceFrame::operator<(const ReferenceFrame& right) const
      throw()
   {
      return (frame < right.frame);
   }
   
   bool ReferenceFrame::operator>=(const ReferenceFrame& right) const
      throw()
   {
      return (frame >= right.frame);
   }
   
   bool ReferenceFrame::operator<=(const ReferenceFrame& right) const
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
      
      initialized = true;
   }
   
      //Prints the name of the reference frame to the specified
      //ostream.
   std::ostream& operator<<(std::ostream& os,
                              const ReferenceFrame& rf)
   {
      return os << rf.asString();
   }
}
