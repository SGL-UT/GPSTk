#pragma ident "$Id: ReferenceFrame.hpp 2009-06-25 14:11:00 tvarney $"

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

#ifndef GPSTK_REFERENCE_FRAME_HPP
#define GPSTK_REFERENCE_FRAME_HPP

#include <iostream>
#include <string>
#include <map>

#include "Exception.hpp"

namespace gpstk{
   
   class ReferenceFrame;
   std::ostream& operator<<(std::ostream& os, ReferenceFrame& rf);
   
   class ReferenceFrame{
      public:
      
      enum FramesEnum{
         Unknown = 0,   /**< Reference frame is unknown*/
         WGS84,   /**< The WGS84 Reference Frame*/
         PZ90   /**< The PZ90 Reference Frame*/
      };
      
      ReferenceFrame(FramesEnum reference = Unknown)
         throw();
      
      ReferenceFrame(int index)
         throw();
      
      ReferenceFrame(const char str[])
         throw();
      
      ReferenceFrame(const std::string str)
         throw();
      
      virtual ~ReferenceFrame() {   };
      
      void setReferenceFrame(const FramesEnum reference)
         throw();
      void setReferenceFrame(const int index)
         throw();
      FramesEnum getFrame() const
         throw();
      ReferenceFrame& createReferenceFrame(const char str[])
         throw();
      ReferenceFrame& createReferenceFrame(std::string& name)
         throw();
      
      std::string& asString() const
         throw();
      void setReferenceFrame(const char name[])
         throw();
      void setReferenceFrame(const std::string& name)
         throw();
      
      bool operator==(const ReferenceFrame& right) const
         throw();
      bool operator!=(const ReferenceFrame& right) const
         throw();
      bool operator>(const ReferenceFrame& right) const
         throw();
      bool operator<(const ReferenceFrame& right) const
         throw();
      bool operator>=(const ReferenceFrame& right) const
         throw();
      bool operator<=(const ReferenceFrame& right) const
         throw();
      
      private:
      
      void initialize()
         throw();
      
      FramesEnum frame;
      static std::map<FramesEnum, std::string> names;
   };
}

#endif
