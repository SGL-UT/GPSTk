#pragma ident "$Id$"

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "ReferenceFrame.hpp"

namespace gpstk
{
      //this is needed to remove undefined reference errors caused by class static members.
   std::map<ReferenceFrame::FramesEnum, std::string> ReferenceFrame::names;

      // Force the execution of the initialize() method.
   bool ReferenceFrame::initFlag = ReferenceFrame::initialize();
   
   ReferenceFrame::ReferenceFrame(FramesEnum e)
   {
      setReferenceFrame((int)e);
   }
   
   ReferenceFrame::ReferenceFrame(int index)
   {
      setReferenceFrame(index);
   }
   
   ReferenceFrame::ReferenceFrame(const std::string str)
   {
      setReferenceFrame(str);
   }
   
   ReferenceFrame::ReferenceFrame(const char str[])
   {
      setReferenceFrame(str);
   }
   
   void ReferenceFrame::setReferenceFrame(const int index)
   {
         //We use names.size() as our upper bound so we can
         //dynamically load new reference frames into this class
      if(index < Unknown || index >= int(names.size()))
         frame = Unknown;
      else
         frame = (FramesEnum)index;
   }
   
   void ReferenceFrame::setReferenceFrame(const std::string& name)
   {
      std::map<ReferenceFrame::FramesEnum, std::string>::const_iterator iter;
      for(iter = names.begin(); iter != names.end(); ++iter)
      {
         if(iter->second == name)
         {
            frame = iter->first;
            return;
         }
      }
      frame = Unknown;
   }
   
   void ReferenceFrame::setReferenceFrame(const char name[])
   {
      setReferenceFrame((std::string)name);
   }
   
   ReferenceFrame::FramesEnum ReferenceFrame::getFrame() const
      throw()
   {
      return frame;
   }
   
   ReferenceFrame& ReferenceFrame::createReferenceFrame(const char str[])
   {
      std::string name(str);
      return createReferenceFrame(name);
   }
   
   ReferenceFrame& ReferenceFrame::createReferenceFrame(std::string& name)
   {
      std::map<ReferenceFrame::FramesEnum, std::string>::const_iterator iter;
      for(iter= names.begin(); iter != names.end(); ++iter)
      {
         if(iter->second == name)
         {
            frame = iter->first;
            return (*this);
         }
      }
         //The specified frame does not exist, create it
      int size = names.size();
      names[(FramesEnum)size] = name;
      frame = (FramesEnum)size;
      return (*this);
   }
   
   std::string& ReferenceFrame::asString() const
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
   
   bool ReferenceFrame::initialize()
   {
      names[Unknown] = "Unknown";
      names[WGS84] = "WGS84";
      names[PZ90] = "PZ90";
      return true;
   }
   
      //Prints the name of the reference frame to the specified
      //ostream.
   std::ostream& operator<<(std::ostream& os,
                              const ReferenceFrame& rf)
   {
      return os << rf.asString();
   }
}
